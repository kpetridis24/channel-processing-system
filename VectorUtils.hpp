#ifndef VECTORUTILS_HPP
#define VECTORUTILS_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <immintrin.h>
#include <numeric>
#include "SystemConstants.hpp"

/**
 * Useful mathematical operations for our channel processing application.
 * 
 * The functionalities exposed are:
 * 
 *  1. Linear Transformation:   aX + b
 *  2. Vector-scalar addition:  X + a
 *  3. Vector-vector addition:  X + Y
 *  4. Mean of vector:          sum(X) / X.size
 *  5. Reciprocal of vector:    1 / X
 * 
 * All the above functions, have 2 implementations. One is the regular, and one optimized, 
 * employing vectorized operations to boost the performance of our system, in case that 
 * SIMD is supported by the hardware.
*/
template <typename DataType>
class VectorUtils {
    using TemplateVector = std::vector<DataType>;

public:
    inline static TemplateVector LinearTransformation(TemplateVector& V, DataType a, DataType b) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }

        size_t size = V.size();
        TemplateVector result(size);

        for (size_t i = 0; i < size; i++) {
            result[i] = a * V[i] + b;
        }

        return std::move(result);
    }

    inline static DataType Mean(TemplateVector& V) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }
        DataType sum = std::accumulate(V.begin(), V.end(), DataType(0));
        return std::move(static_cast<double>(sum) / V.size());
    }

    inline static TemplateVector Add(TemplateVector& V, TemplateVector& U) {
        if (V.size() != U.size()) {
            throw std::runtime_error("Vectors must have the same dimensions.");
        }

        size_t size = V.size();
        TemplateVector result(size);

        for (size_t i = 0; i < size; i++) {
            result[i] = V[i] + U[i];
        }

        return std::move(result);
    }

    inline static TemplateVector Add(TemplateVector& V, DataType b) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }

        size_t size = V.size();
        TemplateVector result(size);

        for (size_t i = 0; i < size; i++) {
            result[i] = V[i] + b;
        }

        return std::move(result);
    }

    inline static TemplateVector Reciprocal(TemplateVector& V) {
        DataType threshold = static_cast<DataType>(SystemConstants::ZeroThreshold);
        bool foundZero = std::any_of(V.begin(), V.end(), [threshold](const auto& v) {
            return v <= threshold; 
        });

        if (foundZero) {
            throw std::runtime_error("Error: Division by 0.");
        }

        std::vector<DataType> result(V.size());
        std::transform(V.begin(), V.end(), result.begin(), [](auto v) {
            return 1.0 / v;
        });

        return std::move(result);
    }

    inline static TemplateVector LinearTransformationSIMD(const TemplateVector V, DataType a, DataType b) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }
        const size_t size = V.size();
        const size_t alignedSize = size - size % 8;
        std::vector<DataType> result(size);
        
        if constexpr (std::is_same<DataType, float>::value) {
            __m256 aVector = _mm256_set1_ps(a);
            __m256 bVector = _mm256_set1_ps(b);
            for (size_t i = 0; i < alignedSize; i += 8 / sizeof(DataType)) {
                __m256 v = _mm256_loadu_ps(&V[i]);
                __m256 transformed = _mm256_add_ps(_mm256_mul_ps(aVector, v), bVector);
                _mm256_storeu_ps(&result[i], transformed);
            }
        } else if constexpr (std::is_same<DataType, double>::value) {
            __m256d aVector = _mm256_set1_pd(a);
            __m256d bVector = _mm256_set1_pd(b);
            for (size_t i = 0; i < alignedSize; i += 4 / sizeof(DataType)) {
                __m256d v = _mm256_loadu_pd(&V[i]);
                __m256d transformed = _mm256_add_pd(_mm256_mul_pd(aVector, v), bVector);
                _mm256_storeu_pd(&result[i], transformed);
            }
        } else {
            for (size_t i = 0; i < size; i++) {
                result[i] = a * V[i] + b;
            }
            return std::move(result);
        }

        for (size_t i = alignedSize; i < size; i++) {
            result[i] = a * V[i] + b;
        }

        return std::move(result);
    }

    inline static DataType MeanSIMD(TemplateVector V) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }

        size_t size = V.size();
        size_t simdSize = sizeof(__m128) / sizeof(DataType);
        size_t numIterations = size / simdSize;

        DataType sum = DataType(0);

        for (int i = 0; i < numIterations; i++) {
            const DataType* chunk = &V[i * simdSize];

            if constexpr (std::is_same<DataType, float>::value) {
                __m128 data = _mm_loadu_ps(chunk);
                __m128 hSum = _mm_hadd_ps(data, data);
                hSum = _mm_hadd_ps(hSum, hSum);
                float floatSum;
                _mm_store_ss(&floatSum, hSum);
                sum += floatSum;

            } else if constexpr (std::is_same<DataType, double>::value) {
                __m128d data = _mm_loadu_pd(chunk);
                __m128d hSum = _mm_hadd_pd(data, data);
                double doubleSum;
                _mm_store_sd(&doubleSum, hSum);
                sum += doubleSum;

            } else if constexpr (std::is_same<DataType, int>::value) {
                __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(chunk));
                __m128i hSum = _mm_hadd_epi32(data, data);
                hSum = _mm_hadd_epi32(hSum, hSum);
                int intSum;
                _mm_store_si128(reinterpret_cast<__m128i*>(&intSum), hSum);
                sum += intSum;
            }
        }

        // Handle remaining elements (not multiple of simdSize)
        for (size_t i = numIterations * simdSize; i < size; i++) {
            sum += V[i];
        }

        return sum / static_cast<DataType>(size);

    }

    inline static TemplateVector AddSIMD(TemplateVector V, TemplateVector U) {
        if (V.size() != U.size()) {
            throw std::runtime_error("Vectors must have the same dimensions.");
        }

        const size_t size = V.size();
        const size_t alignedSize = size - size % 8;

        TemplateVector result(size);

        if constexpr (std::is_same<DataType, double>::value) {
            for (size_t i = 0; i < alignedSize; i += 4 / sizeof(DataType)) {
                __m256d a = _mm256_loadu_pd(&V[i]);
                __m256d b = _mm256_loadu_pd(&U[i]);
                __m256d sum = _mm256_add_pd(a, b);
                _mm256_storeu_pd(&result[i], sum);
            }
        } else if constexpr (std::is_same<DataType, float>::value) {
            for (size_t i = 0; i < alignedSize; i += 8 / sizeof(DataType)) {
                __m256 a = _mm256_loadu_ps(&V[i]);
                __m256 b = _mm256_loadu_ps(&U[i]);
                __m256 sum = _mm256_add_ps(a, b);
                _mm256_storeu_ps(&result[i], sum);
            }
        } else {
            for (size_t i = 0; i < size; i++) {
                result[i] = V[i] + U[i];
            }
            return std::move(result);
        }

        for (size_t i = alignedSize; i < size; i++) {
            result[i] = V[i] + U[i];
        }

        return std::move(result);
    }

    inline static TemplateVector AddSIMD(TemplateVector V, DataType b) {
        if (V.empty()) {
            throw std::runtime_error("Vector is empty.");
        }

        const size_t size = V.size();
        const size_t alignedSize = size - size % 8;

        TemplateVector result(size);

        if constexpr (std::is_same<DataType, double>::value) {
            __m256d scalarVector = _mm256_set1_pd(b);
            for (size_t i = 0; i < alignedSize; i += 4 / sizeof(DataType)) {
                __m256d v = _mm256_loadu_pd(&V[i]);
                __m256d sum = _mm256_add_pd(v, scalarVector);
                _mm256_storeu_pd(&result[i], sum);
            }
        } else if constexpr (std::is_same<DataType, float>::value) {
            __m256 scalarVector = _mm256_set1_ps(b);
            for (size_t i = 0; i < alignedSize; i += 8 / sizeof(DataType)) {
                __m256 v = _mm256_loadu_ps(&V[i]);
                __m256 sum = _mm256_add_ps(v, scalarVector);
                _mm256_storeu_ps(&result[i], sum);
            }
        } else {
            for (size_t i = 0; i < size; i++) {
                result[i] = V[i] + b;
            }
            return std::move(result);
        }

        for (size_t i = alignedSize; i < size; i++) {
            result[i] = V[i] + b;
        }

        return std::move(result);
    }

    static bool VerifySameVectors(TemplateVector& V1, TemplateVector& V2) {
        return std::equal(V1.begin(), V1.end(), V2.begin(), V2.end(), [](auto& a, auto& b) {
            return std::fabs(a - b) <= SystemConstants::EqualityThreshold;
        });    
    }

};

#endif