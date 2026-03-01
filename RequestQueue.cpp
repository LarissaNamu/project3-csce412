/**
 * @file RequestQueue.cpp
 * @brief Implementation of the RequestQueue class.
 *
 * @details All operations delegate directly to the internal std::queue.
 * The class exists to provide a clean, simulation-specific interface
 * and to make the queue type easily swappable in the future.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#include "RequestQueue.h"
#include <stdexcept>

/**
 * @brief Default constructor. The internal std::queue is empty by default.
 */
RequestQueue::RequestQueue() {}

/**
 * @brief Adds a request to the back of the queue.
 * @param request The Request to add.
 */
void RequestQueue::enqueue(const Request& request) {
    queue.push(request);
}

/**
 * @brief Removes and returns the front request.
 * @throws std::runtime_error if the queue is empty.
 * @return The front Request object.
 */
Request RequestQueue::dequeue() {
    if (queue.empty()) {
        throw std::runtime_error("dequeued on an empty queue.");
    }
    Request front = queue.front();
    queue.pop();
    return front;
}

/**
 * @brief Returns the current number of requests in the queue.
 * @return Queue size as int.
 */
int RequestQueue::size() const {
    return static_cast<int>(queue.size());
}

/**
 * @brief Returns true if the queue contains no requests.
 * @return Boolean empty status.
 */
bool RequestQueue::isEmpty() const {
    return queue.empty();
}

/**
 * @brief Returns a const reference to the front request without removing it.
 * @throws std::runtime_error if the queue is empty.
 * @return Const reference to the front Request.
 */
const Request& RequestQueue::peek() const {
    if (queue.empty()) {
        throw std::runtime_error("peeked on an empty queue.");
    }
    return queue.front();
}