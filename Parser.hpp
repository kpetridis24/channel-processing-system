#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

template <typename DataType>
class Parser {
public:
    /**
     * Parse channel values from file and return a new vector.
    */
    inline static std::vector<DataType> parseChannels(const std::string& fileName) {
        std::vector<DataType> channels;
        std::ifstream file(fileName);

        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " + fileName << "\n";
            return channels;
        }

        char skipCharacter;
        DataType value;
        file >> skipCharacter;
        char comma; 

        while (file >> comma) {
            file >> value;
            channels.push_back(value);
        }

        file.close();
        return std::move(channels);
    }

    /**
     * Receive empty vector and fill in the channel values from the file.
    */
    inline static void parseChannels(const std::string& fileName, std::vector<DataType>& channels) {
        std::ifstream file(fileName);

        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " + fileName << "\n";
            return;
        }

        char skipCharacter;
        DataType value;
        file >> skipCharacter;
        char comma; 

        while (file >> comma) {
            file >> value;
            channels.push_back(value);
        }

        file.close();
    }

    /**
     * Read the parameters from a file and return them into a map. 
     * 
     * We use a map for robustness, handling cases with multiple different parameters.
     * We don't need to pass in the names of the parameters beforehand because the 
     * function reads the names and stores them as keys. 
    */
    inline static std::map<char, DataType> parseParameters(const std::string& fileName) {
        std::map<char, DataType> parameters;
        std::ifstream file(fileName);

        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " + fileName << "\n";
            return parameters;
        }

        char parameterName;
        DataType parameterValue;
        char comma;

        while (file >> parameterName >> comma >> parameterValue) {
            parameters.emplace(parameterName, parameterValue);
        }

        file.close();
        return std::move(parameters);
    }   

};

#endif
