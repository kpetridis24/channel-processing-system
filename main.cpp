#include "Parser.hpp"
#include <iomanip>
#include <cmath>
#include "ChannelProcessor.hpp"

using FloatVector = std::vector<float>;
using V = VectorUtils<float>;

int main() {
    ChannelProcessor<float> channelProcessor(SystemConstants::ChannelsFile, SystemConstants::ParametersFile);
    
    // Compute Y
    FloatVector Y1 = channelProcessor.function1(false);
    FloatVector Y2 = channelProcessor.function1(true);
    size_t size = Y1.size();

    // Compute b
    float b1 = channelProcessor.function2(false);
    float b2 = channelProcessor.function2(true);

    // Compute A
    FloatVector A = channelProcessor.function3();

    // Compute C
    FloatVector C1 = channelProcessor.function4(false);
    FloatVector C2 = channelProcessor.function4(true);

    // Verify that SIMD and non-SIMD produce the exact same results.
    if (std::fabs(b1 - b2) >= SystemConstants::EqualityThreshold 
        || !V::VerifySameVectors(Y1, Y2) 
        || !V::VerifySameVectors(C1, C2)) {
        std::cerr << "ERROR: SIMD didn't produce the same result.\n";
        exit(0);
    }

    std::cout<<"Computed b = " << b1 << "\n";

    return 0;
}