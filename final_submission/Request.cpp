/**
 * @file Request.cpp
 * @brief Implementation of the Request class.
 *
 * @details Implements constructors, getters, and static utility methods
 * for generating random IP addresses and random Request objects.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#include "Request.h"
#include <cstdlib>
#include <string>

/**
 * @brief Default constructor. Initializes all fields to safe default values.
 */
Request::Request()
    : ipIn("0.0.0.0"), ipOut("0.0.0.0"), time(1), jobType('P') {}

/**
 * @brief Parameterized constructor.
 * @param ipIn    Source IP address
 * @param ipOut   Destination IP address
 * @param time    Processing time in clock cycles
 * @param jobType 'P' for Processing or 'S' for Streaming
 */
Request::Request(const std::string& ipIn, const std::string& ipOut, int time, char jobType)
    : ipIn(ipIn), ipOut(ipOut), time(time), jobType(jobType) {}


/**
 * @brief Returns the source IP address.
 */
std::string Request::getIpIn() const {
    return ipIn;
}

/**
 * @brief Returns the destination IP address.
 */
std::string Request::getIpOut() const {
    return ipOut;
}

/**
 * @brief Returns the processing time in clock cycles.
 */
int Request::getTime() const {
    return time;
}

/**
 * @brief Returns the job type character.
 */
char Request::getJobType() const {
    return jobType;
}

/**
 * @brief Generates a random IPv4 address.
 * @details Builds a dotted-decimal IP string by randomly selecting
 * each of the four octets in the range [0, 255].
 * @return Randomly generated IP string.
 */
std::string Request::generateRandomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

/**
 * @brief Creates a fully randomized Request object.
 * @details Both IP addresses are randomly generated. Processing time
 * is randomly selected within [minTime, maxTime]. Job type is randomly
 * assigned as either 'P' or 'S'.
 * @param minTime Minimum clock cycles for processing
 * @param maxTime Maximum clock cycles for processing
 * @return A new randomized Request.
 */
Request Request::generateRandom(int minTime, int maxTime) {
    std::string ipIn = generateRandomIP();
    std::string ipOut = generateRandomIP();

    int time = minTime + (rand() % (maxTime - minTime + 1));

    char jobType;
    int randomVal = rand() % 2; 
    
    if (randomVal == 0) {
        jobType = 'P';
    } else {
        jobType = 'S';
    }
    

    return Request(ipIn, ipOut, time, jobType);
}