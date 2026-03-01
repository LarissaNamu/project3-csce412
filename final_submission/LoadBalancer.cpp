/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer class.
 *
 * @details Drives the full simulation: reads config, fills the initial queue,
 * ticks servers each cycle, distributes work, dynamically scales the server pool,
 * enforces the IP firewall, and writes a structured log with summary.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#include "LoadBalancer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_WHITE "\033[37m"

/**
 * @brief Constructs the LoadBalancer by parsing the config file.
 * @param configFile Path to the configuration file.
 */
LoadBalancer::LoadBalancer(const std::string& configFile)
    : currentClock(0), nextServerId(1),
      cooldownTimer(0),
      totalRequestsProcessed(0), totalRequestsBlocked(0),
      peakQueueSize(0), serversAdded(0), serversRemoved(0)
{
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + configFile);
    }

    // Defaults
    serverCount = 10;
    totalCycles = 10000;
    minTaskTime = 3;
    maxTaskTime = 500;
    scalingCooldown = 100;
    newRequestInterval = 50;
    newRequestBatchSize= 5;
    blockedRangePrefix = "192.168.1";
    logFileName = "loadbalancer.log";

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == "serverCount") serverCount = std::stoi(value);
            else if (key == "totalCycles") totalCycles = std::stoi(value);
            else if (key == "minTaskTime") minTaskTime = std::stoi(value);
            else if (key == "maxTaskTime") maxTaskTime = std::stoi(value);
            else if (key == "scalingCooldown") scalingCooldown = std::stoi(value);
            else if (key == "newRequestInterval") newRequestInterval = std::stoi(value);
            else if (key == "newRequestBatchSize") newRequestBatchSize = std::stoi(value);
            else if (key == "blockedRangePrefix") blockedRangePrefix = value;
            else if (key == "logFileName") logFileName = value;
        }
    }
    file.close();

    logFile.open(logFileName);
    if (!logFile.is_open()) {
        throw std::runtime_error("Could not open log file: " + logFileName);
    }

    for (int i = 0; i < serverCount; i++) {
        webServers.push_back(new WebServer(nextServerId++));
    }
}

/**
 * @brief Frees all dynamically allocated WebServer objects and closes the log.
 */
LoadBalancer::~LoadBalancer() {
    for (WebServer* ws : webServers) {
        delete ws;
    }
    webServers.clear();
    if (logFile.is_open()) {
        logFile.close();
    }
}


/**
 * @brief Logs a message to terminal (colored) and to the log file (plain).
 * @param message   Text to output.
 * @param colorCode ANSI escape code for terminal color.
 */
void LoadBalancer::log(const std::string& message, const std::string& colorCode) {
    // Colored terminal output
    if (!colorCode.empty()) {
        std::cout << colorCode << message << COLOR_RESET << "\n";
    } else {
        std::cout << message << "\n";
    }
    // Plain text to log file
    if (logFile.is_open()) {
        logFile << message << "\n";
    }
}

/**
 * @brief Populates the initial queue with serverCount * 100 requests.
 */
void LoadBalancer::fillInitialQueue() {
    int initialSize = serverCount * 100;
    for (int i = 0; i < initialSize; i++) {
        Request r = Request::generateRandom(minTaskTime, maxTaskTime);
        if (!isBlocked(r.getIpIn())) {
            requestQueue.enqueue(r);
        }
    }
    log("[INIT] Initial queue filled with " + std::to_string(requestQueue.size()) +
        " requests.", COLOR_CYAN);
}


/**
 * @brief Ticks all servers one clock cycle and counts completions.
 */
void LoadBalancer::tickServers() {
    for (WebServer* ws : webServers) {
        bool wasBusy = !ws->isAvailable();
        ws->tick();
        // If server just finished a request
        if (wasBusy && ws->isAvailable()) {
            totalRequestsProcessed++;
            log("[CYCLE " + std::to_string(currentClock) + "] Server " +
                std::to_string(ws->getId()) + " completed a " +
                ws->getCurrentRequest().getJobType() + " request from " +
                ws->getCurrentRequest().getIpIn(), COLOR_GREEN);
        }
    }
}

/**
 * @brief Assigns queued requests to any available servers.
 */
void LoadBalancer::distributeRequests() {
    for (WebServer* ws : webServers) {
        if (ws->isAvailable() && !requestQueue.isEmpty()) {
            Request r = requestQueue.dequeue();
            ws->assignRequest(r);
            log("[CYCLE " + std::to_string(currentClock) + "] Server " +
                std::to_string(ws->getId()) + " assigned " +
                std::string(1, r.getJobType()) + " request from " +
                r.getIpIn() + " -> " + r.getIpOut() +
                " (time: " + std::to_string(r.getTime()) + ")", COLOR_WHITE);
        }
    }
}

/**
 * @brief Adds a batch of new random requests to the queue.
 */
void LoadBalancer::addNewRequests() {
    Request blockedTest("192.168.1." + std::to_string(rand() % 256),
                        Request::generateRandomIP(), 
                        minTaskTime + (rand() % (maxTaskTime - minTaskTime + 1)),
                        (rand() % 2 == 0) ? 'P' : 'S');
    if (isBlocked(blockedTest.getIpIn())) {
        totalRequestsBlocked++;
        log("[CYCLE " + std::to_string(currentClock) + "] FIREWALL blocked request from " +
            blockedTest.getIpIn(), COLOR_RED);
    }

    int added = 0;
    int blocked = 0;
    for (int i = 0; i < newRequestBatchSize; i++) {
        Request r = Request::generateRandom(minTaskTime, maxTaskTime);
        if (isBlocked(r.getIpIn())) {
            blocked++;
            totalRequestsBlocked++;
        } else {
            requestQueue.enqueue(r);
            added++;
        }
    }
    if (blocked > 0) {
        log("[CYCLE " + std::to_string(currentClock) + "] FIREWALL blocked " +
            std::to_string(blocked) + " request(s) from range " +
            blockedRangePrefix + ".x", COLOR_RED);
    }
    log("[CYCLE " + std::to_string(currentClock) + "] Added " +
        std::to_string(added) + " new request(s) to queue. Queue size: " +
        std::to_string(requestQueue.size()), COLOR_CYAN);
}

/**
 * @brief Checks thresholds and scales server pool up or down.
 */
void LoadBalancer::adjustServers() {
    if (cooldownTimer > 0) {
        cooldownTimer--;
        return;
    }

    int qSize = requestQueue.size();
    int lowerThreshold = 50 * serverCount;
    int upperThreshold = 80 * serverCount;

    if (qSize > upperThreshold) {
        addServer();
        cooldownTimer = scalingCooldown;
    } else if (qSize < lowerThreshold && serverCount > 1) {
        removeServer();
        cooldownTimer = scalingCooldown;
    }
}

/**
 * @brief Adds one new WebServer to the pool.
 */
void LoadBalancer::addServer() {
    webServers.push_back(new WebServer(nextServerId++));
    serverCount++;
    serversAdded++;
    log("[CYCLE " + std::to_string(currentClock) + "] SERVER ADDED — pool size now: " +
        std::to_string(serverCount) + " | Queue: " +
        std::to_string(requestQueue.size()), COLOR_MAGENTA);
}

/**
 * @brief Removes one idle WebServer from the pool.
 */
void LoadBalancer::removeServer() {
    for (auto it = webServers.begin(); it != webServers.end(); ++it) {
        if ((*it)->isAvailable()) {
            log("[CYCLE " + std::to_string(currentClock) + "] SERVER REMOVED (ID: " +
                std::to_string((*it)->getId()) + ") — pool size now: " +
                std::to_string(serverCount - 1) + " | Queue: " +
                std::to_string(requestQueue.size()), COLOR_YELLOW);
            delete *it;
            webServers.erase(it);
            serverCount--;
            serversRemoved++;
            return;
        }
    }
    log("[CYCLE " + std::to_string(currentClock) +
        "] Scale-down skipped — no idle servers available.", COLOR_YELLOW);
}


/**
 * @brief Returns true if the IP starts with the blocked prefix.
 * @param ip IP address string to check.
 */
bool LoadBalancer::isBlocked(const std::string& ip) const {
    return ip.substr(0, blockedRangePrefix.size()) == blockedRangePrefix;
}


/**
 * @brief Writes the full simulation summary to terminal and log file.
 */
void LoadBalancer::writeSummary() {
    std::string sep = "================================================";
    log(sep, COLOR_CYAN);
    log("           LOAD BALANCER SIMULATION SUMMARY", COLOR_CYAN);
    log(sep, COLOR_CYAN);
    log("Total clock cycles run      : " + std::to_string(totalCycles));
    log("Starting queue size         : " + std::to_string(startingQueueSize));
    log("Ending queue size           : " + std::to_string(requestQueue.size()));
    log("Task time range             : " + std::to_string(minTaskTime) +
        " - " + std::to_string(maxTaskTime) + " cycles");
    log("Total requests processed    : " + std::to_string(totalRequestsProcessed));
    log("Total requests blocked      : " + std::to_string(totalRequestsBlocked));
    log("Peak queue size             : " + std::to_string(peakQueueSize));
    log("Servers added dynamically   : " + std::to_string(serversAdded));
    log("Servers removed dynamically : " + std::to_string(serversRemoved));
    log("Final server count          : " + std::to_string(serverCount));
    log("Blocked IP range            : " + blockedRangePrefix + ".x");
    int activeServers = 0;
    int idleServers = 0;
    for (WebServer* ws : webServers) {
        if (!ws->isAvailable()) activeServers++;
        else idleServers++;
    }
    log("Active servers at end        : " + std::to_string(activeServers));
    log("Idle servers at end          : " + std::to_string(idleServers));
    log(sep, COLOR_CYAN);
}

/**
 * @brief Runs the simulation for totalCycles clock cycles.
 */
void LoadBalancer::run() {
    log("Starting Load Balancer Simulation...", COLOR_CYAN);
    log("Servers: " + std::to_string(serverCount) +
        " | Cycles: " + std::to_string(totalCycles), COLOR_CYAN);

    fillInitialQueue();
    startingQueueSize = requestQueue.size();

    for (currentClock = 1; currentClock <= totalCycles; currentClock++) {
        tickServers();

        distributeRequests();

        if (currentClock % newRequestInterval == 0) {
            addNewRequests();
        }

        if (requestQueue.size() > peakQueueSize) {
            peakQueueSize = requestQueue.size();
        }

        adjustServers();
    }

    writeSummary();
    log("Simulation complete. Log written to: " + logFileName, COLOR_CYAN);
}