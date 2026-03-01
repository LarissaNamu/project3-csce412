/**
 * @file LoadBalancer.h
 * @brief Defines the LoadBalancer class that manages WebServers and the RequestQueue.
 *
 * @details The LoadBalancer is the core of the simulation. It distributes incoming
 * requests to available WebServers, dynamically scales the server pool based on
 * queue size thresholds, generates new requests over time, blocks requests from
 * banned IP ranges, logs all events, and outputs colored terminal feedback.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <vector>
#include <string>
#include <fstream>
#include "WebServer.h"
#include "RequestQueue.h"

/**
 * @class LoadBalancer
 * @brief Manages the web server pool and request queue for the simulation.
 *
 * Reads configuration from a file, runs the simulation clock cycle by cycle,
 * scales servers dynamically, and writes a detailed log file.
 */
class LoadBalancer {
private:
    std::vector<WebServer*> webServers; ///< Dynamically allocated pool of WebServers
    RequestQueue requestQueue; ///< Queue of pending requests
    int currentClock; ///< Current simulation clock cycle
    int totalCycles; ///< Total clock cycles to simulate
    int serverCount; ///< Current number of active servers
    int nextServerId; ///< Counter for assigning unique server IDs

    // Configuration values (from config.txt)
    int minTaskTime; ///< Minimum request processing time in clock cycles
    int maxTaskTime; ///< Maximum request processing time in clock cycles
    int scalingCooldown; ///< Clock cycles to wait between server add/remove actions
    int cooldownTimer; ///< Tracks remaining cooldown cycles
    int newRequestInterval; ///< How often (in cycles) new requests are added to the queue
    int newRequestBatchSize; ///< How many requests to add each interval
    int startingQueueSize; ///< Queue size after initial fill

    // Firewall / IP blocker
    std::string blockedRangePrefix; ///< IP prefix to block (e.g., "192.168.1")

    // Logging
    std::ofstream logFile; ///< Output stream for the log file
    std::string logFileName; ///< Name of the log file

    // Summary tracking
    int totalRequestsProcessed; ///< Total requests completed during the simulation
    int totalRequestsBlocked; ///< Total requests rejected by the firewall
    int peakQueueSize; ///< Largest queue size observed during the simulation
    int serversAdded; ///< Total number of times a server was added
    int serversRemoved; ///< Total number of times a server was removed

    /**
     * @brief Logs a message to both the terminal (with color) and the log file.
     * @param message  The text to log.
     * @param colorCode ANSI color escape code string (e.g., "\033[32m" for green).
     */
    void log(const std::string& message, const std::string& colorCode = "");

    /**
     * @brief Fills the request queue to its initial capacity (serverCount * 100).
     */
    void fillInitialQueue();

    /**
     * @brief Distributes queued requests to any available WebServers.
     */
    void distributeRequests();

    /**
     * @brief Ticks all active WebServers forward one clock cycle.
     * @details Also counts completed requests for the summary.
     */
    void tickServers();

    /**
     * @brief Checks queue size thresholds and adds or removes servers if needed.
     * @details Respects the scalingCooldown between consecutive scaling actions.
     */
    void adjustServers();

    /**
     * @brief Adds one new WebServer to the pool.
     */
    void addServer();

    /**
     * @brief Removes one idle WebServer from the pool.
     * @details Only removes a server that is currently available (not busy).
     */
    void removeServer();

    /**
     * @brief Adds a batch of new random requests to the queue.
     * @details Called every newRequestInterval cycles to simulate ongoing traffic.
     */
    void addNewRequests();

    /**
     * @brief Checks whether a given IP address should be blocked by the firewall.
     * @param ip The IP address string to check.
     * @return True if the IP matches the blocked range prefix, false otherwise.
     */
    bool isBlocked(const std::string& ip) const;

    /**
     * @brief Writes the simulation summary to the log file and terminal.
     * @details Called once at the end of run(). Includes all required summary fields.
     */
    void writeSummary();

public:
    /**
     * @brief Constructs a LoadBalancer by reading settings from a config file.
     * @param configFile Path to the configuration file (e.g., "config.txt").
     */
    LoadBalancer(const std::string& configFile);

    /**
     * @brief Destructor. Frees all dynamically allocated WebServer objects.
     */
    ~LoadBalancer();

    /**
     * @brief Runs the full simulation for the configured number of clock cycles.
     * @details Main simulation loop. Each cycle: ticks servers, distributes requests,
     * adds new requests on interval, checks scaling thresholds, and logs events.
     */
    void run();
};

#endif // LOADBALANCER_H