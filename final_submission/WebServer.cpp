/**
 * @file WebServer.cpp
 * @brief Implementation of the WebServer class.
 *
 * @details Handles request assignment, per-cycle processing via tick(),
 * and availability reporting. The LoadBalancer calls tick() on every
 * server each clock cycle and uses isAvailable() to find servers
 * ready for new work.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#include "WebServer.h"

/**
 * @brief Constructs a WebServer with the given ID, initially idle.
 * @param id Unique server identifier assigned by the LoadBalancer.
 */
WebServer::WebServer(int id)
    : serverId(id), isBusy(false), currentRequest(), remainingTime(0) {}
 
/**
 * @brief Assigns a request to this server and begins processing.
 * @param request The Request to process.
 */
void WebServer::assignRequest(const Request& request) {
    currentRequest = request;
    remainingTime = request.getTime();
    isBusy = true;
}

/**
 * @brief Advances this server one clock cycle.
 * @details If busy, decrements remainingTime. When it hits zero,
 * the server becomes available again for the next request.
 */
void WebServer::tick() {
    if (isBusy) {
        remainingTime--;
        if (remainingTime <= 0) {
            isBusy = false;
            remainingTime = 0;
        }
    }
}


/**
 * @brief Returns true if the server has no active request.
 */
bool WebServer::isAvailable() const {
    return !isBusy;
}

/**
 * @brief Returns this server's unique ID.
 */
int WebServer::getId() const {
    return serverId;
}

/**
 * @brief Returns remaining clock cycles for the current request.
 */
int WebServer::getRemainingTime() const {
    return remainingTime;
}

/**
 * @brief Returns the request currently assigned to this server.
 */
Request WebServer::getCurrentRequest() const {
    return currentRequest;
}