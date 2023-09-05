#ifndef SYSTEMVARIABLES_HPP
#define SYSTEMVARIABLES_HPP

#include <string>

/**
 * Constants used by the Channel Processing application.
*/
namespace SystemConstants {
    const char* ChannelsFile = "./data/channels.txt";
    const char* ParametersFile = "./data/parameters.txt";
    const double ZeroThreshold = 0.000000001;
    const double EqualityThreshold = 0.00001;
    const bool isSIMDSupported = false; 
}

#endif