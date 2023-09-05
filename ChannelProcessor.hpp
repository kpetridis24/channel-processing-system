#ifndef CHANNELPROCESSOR_HPP
#define CHANNELPROCESSOR_HPP

#include "VectorUtils.hpp"

/**
 * Implementation of the Channel Processing System.
 * 
 * This class provides a generic, parameterized channel processor that can work with any input that 
 * makes sense for such an application. 
*/
template <typename DataType>
class ChannelProcessor {
private:
    using TemplateVector = std::vector<DataType>;
    using V = VectorUtils<DataType>;

    TemplateVector X;
    DataType m;
    DataType c;
    
    /**
     * Set the channel values and the parameters from file. 
    */
    inline void initializeFromFile(const std::string& channelsFile, const std::string& paramsFile) {
        Parser<DataType>::parseChannels(channelsFile, X);
        auto params = Parser<DataType>::parseParameters(paramsFile);
        if (params.find('m') == params.end() || params.find('c') == params.end()) {
            std::cerr << "Required parameters missing in source " + paramsFile + ".\n";
            exit(0);
        }
        m = params['m'];
        c = params['c'];
    }

public:
    ChannelProcessor(const std::string& channelsFile, const std::string& paramsFile) {
        initializeFromFile(channelsFile, paramsFile);
    }
    
    /**
     * Clear the channel values and restore them from file.
     * 
     * The idea is that, our values may change over the course of time, and we need a versatile way 
     * to fetch the updates and re-calculate the necessary metrics.
    */
    inline void fetchData(const std::string& channelsFile, const std::string& paramsFile) {
        X.clear();
        initializeFromFile(channelsFile, paramsFile);
    }

    /**
     * Function1 as described in the assignment: Y = mX + c
    */
    inline TemplateVector function1(const bool& simd = false) {
        if (simd) {
            return std::move(V::LinearTransformationSIMD(X, m, c));
        } else {
            return std::move(V::LinearTransformation(X, m, c));
        }
    }

    /**
     * Function2 as described in the assignment: b = mean(A + Y)
    */
    inline double function2(const bool& simd = false) {
        TemplateVector A = function3();
        TemplateVector Y = function1(simd);

        if (simd) {
            TemplateVector Q = V::AddSIMD(A, Y);
            return std::move(V::MeanSIMD(Q));
        } else {
            TemplateVector Q = V::Add(A, Y);
            return std::move(V::Mean(Q));
        }
    }

    /**
     * Function3 as described in the assignment: A = 1 / X
    */
    inline TemplateVector function3() {
        TemplateVector A = V::Reciprocal(X);
        return std::move(A);
    }

    /**
     * Function1 as described in the assignment: C = X + b
    */
    inline TemplateVector function4(const bool& simd) {
        double b = function2(simd);
        if (simd) {
            return std::move(V::AddSIMD(X, b));
        } else {
            return std::move(V::Add(X, b));
        }
    }

};

#endif