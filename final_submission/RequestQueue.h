/**
 * @file RequestQueue.h
 * @brief Defines the RequestQueue class, a queue of Request objects used by the LoadBalancer.
 *
 * @details Wraps std::queue<Request> to provide a clean interface for
 * enqueuing, dequeuing, and inspecting requests in the load balancer simulation.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include <queue>
#include "Request.h"

/**
 * @class RequestQueue
 * @brief A queue of Request objects managed by the LoadBalancer.
 *
 * Provides enqueue, dequeue, size, and inspection operations.
 * The LoadBalancer uses this to distribute work to WebServers.
 */
class RequestQueue {
private:
    std::queue<Request> queue; ///< Internal STL queue storing pending requests

public:
    /**
     * @brief Default constructor. Creates an empty RequestQueue.
     */
    RequestQueue();

    /**
     * @brief Adds a request to the back of the queue.
     * @param request The Request object to enqueue.
     */
    void enqueue(const Request& request);

    /**
     * @brief Removes and returns the request at the front of the queue.
     * @details Caller must check isEmpty() before calling to avoid undefined behavior.
     * @return The next Request to be processed.
     */
    Request dequeue();

    /**
     * @brief Returns the number of requests currently in the queue.
     * @return Queue size as an integer.
     */
    int size() const;

    /**
     * @brief Checks whether the queue is empty.
     * @return True if the queue has no requests, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Returns a const reference to the front request without removing it.
     * @details Useful for inspecting the next request before committing to dequeue.
     * Caller must check isEmpty() first.
     * @return Const reference to the front Request.
     */
    const Request& peek() const;
};

#endif // REQUESTQUEUE_H