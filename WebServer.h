/**
 * @file WebServer.h
 * @brief Defines the WebServer class representing a single server in the load balancer simulation.
 *
 * @details Each WebServer can hold one active Request at a time. The LoadBalancer
 * assigns requests to available servers and ticks each server forward one clock
 * cycle at a time. When a server finishes its current request, it becomes
 * available again for the next one.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Represents a single web server that processes one request at a time.
 *
 * The LoadBalancer creates and destroys WebServer instances dynamically
 * based on queue size thresholds. Each server tracks its own ID,
 * busy state, current request, and remaining processing time.
 */
class WebServer {
private:
    int serverId; ///< Unique identifier for this server
    bool isBusy; ///< True if the server is currently processing a request
    Request currentRequest; ///< The request currently being processed
    int remainingTime; ///< Clock cycles remaining to finish the current request

public:
    /**
     * @brief Constructs a WebServer with a given ID.
     * @param id Unique integer identifier for this server instance.
     */
    WebServer(int id);

    /**
     * @brief Assigns a new request to this server and marks it as busy.
     * @details Sets remainingTime to the request's time value.
     * Should only be called when isAvailable() returns true.
     * @param request The Request object to process.
     */
    void assignRequest(const Request& request);

    /**
     * @brief Advances the server by one clock cycle.
     * @details If the server is busy, decrements remainingTime by one.
     * When remainingTime reaches zero, the server marks itself as free.
     */
    void tick();

    /**
     * @brief Returns whether this server is available to take a new request.
     * @return True if the server is not currently busy, false otherwise.
     */
    bool isAvailable() const;

    /**
     * @brief Returns this server's unique ID.
     * @return Integer server ID.
     */
    int getId() const;

    /**
     * @brief Returns the remaining processing time for the current request.
     * @return Clock cycles remaining. Returns 0 if the server is not busy.
     */
    int getRemainingTime() const;

    /**
     * @brief Returns a copy of the request currently being processed.
     * @details Only meaningful when isBusy is true.
     * @return The current Request object.
     */
    Request getCurrentRequest() const;
};

#endif // WEBSERVER_H