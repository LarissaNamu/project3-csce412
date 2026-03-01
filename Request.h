/**
 * @file Request.h
 * @brief Defines the Request class representing a single web request in the load balancer simulation.
 *
 * @details Each request contains a source IP address, destination IP address,
 * an estimated processing time in clock cycles, and a job type indicating
 * whether the task is a processing job or a streaming job.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @class Request
 * @brief Represents a single web request to be handled by the load balancer.
 *
 * Requests are generated with random IP addresses and times, then placed
 * into the RequestQueue to be distributed to WebServers by the LoadBalancer.
 */
class Request {
private:
    std::string ipIn; ///< Source IP address of the incoming request
    std::string ipOut; ///< Destination IP address for the response
    int time; ///< Estimated processing time in clock cycles
    char jobType; ///< Job type: 'P' for Processing, 'S' for Streaming

public:
    /**
     * @brief Default constructor. Creates an empty/uninitialized request.
     */
    Request();

    /**
     * @brief Parameterized constructor. Creates a fully initialized request.
     * @param ipIn   Source IP address string (e.g., "192.168.1.1")
     * @param ipOut  Destination IP address string (e.g., "10.0.0.5")
     * @param time   Processing time in clock cycles (must be > 0)
     * @param jobType Character indicating job type: 'P' (processing) or 'S' (streaming)
     */
    Request(const std::string& ipIn, const std::string& ipOut, int time, char jobType);

    /**
     * @brief Returns the source IP address of the request.
     * @return Source IP as a string.
     */
    std::string getIpIn() const;

    /**
     * @brief Returns the destination IP address of the request.
     * @return Destination IP as a string.
     */
    std::string getIpOut() const;

    /**
     * @brief Returns the estimated processing time of the request.
     * @return Processing time in clock cycles.
     */
    int getTime() const;

    /**
     * @brief Returns the job type of the request.
     * @return 'P' for Processing, 'S' for Streaming.
     */
    char getJobType() const;

    /**
     * @brief Generates a random IPv4 address string.
     * @details Static utility used during random request generation.
     * Each octet is randomly chosen between 0 and 255.
     * @return A randomly generated IP address as a string (e.g., "172.16.45.200").
     */
    static std::string generateRandomIP();

    /**
     * @brief Creates a fully randomized Request.
     * @details Generates random IP addresses for both source and destination,
     * a random processing time within the given range, and a random job type.
     * @param minTime Minimum processing time in clock cycles (inclusive)
     * @param maxTime Maximum processing time in clock cycles (inclusive)
     * @return A new Request object with randomized fields.
     */
    static Request generateRandom(int minTime, int maxTime);
};

#endif // REQUEST_H