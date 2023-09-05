#include <cassert>
#include <cmath>
#include "../VectorUtils.hpp"

using dvec = std::vector<double>;
using fvec = std::vector<float>;

class VectorUtilsTest {
public:
    VectorUtilsTest() {}

    void testAll() {
        testAddVectors();
        testDifferentVectorSizes();
        testDifferentVectorSizesSIMD();
        testAddVectorScalar();
        testAddVectorsSIMD();
        testAddVectorScalarSIMD();
        testLinearTransformation();
        testLinearTransformationSIMD();
        testMean();
        testMeanSIMD();
        testReciprocal();
    }

    void testAddVectors() {
        dvec v1 = {1.5, 2.1, 8.7, 4.0};
        dvec v2 = {3.1, 4.2, 1.6, 7.3};
        dvec target = {4.6, 6.3, 10.3, 11.3};
        dvec res = VectorUtils<double>::Add(v1, v2);

        assert(VectorUtils<double>::VerifySameVectors(res, target));

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        fvec v4 = {302.15, 406.2, 19.6, 71.39, 920.54, 63.46};
        fvec target2 = {426.02, 449.79, 111.78, 105.6, 976.54, 106.583};
        fvec res2 = VectorUtils<float>::Add(v3, v4);

        assert(VectorUtils<float>::VerifySameVectors(res2, target2));
    }

    void testAddVectorsSIMD() {
        dvec v1 = {1.5, 2.1, 8.7, 4.0};
        dvec v2 = {3.1, 4.2, 1.6, 7.3};
        dvec target = {4.6, 6.3, 10.3, 11.3};
        dvec res = VectorUtils<double>::AddSIMD(v1, v2);

        assert(VectorUtils<double>::VerifySameVectors(res, target));

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        fvec v4 = {302.15, 406.2, 19.6, 71.39, 920.54, 63.46};
        fvec target2 = {426.02, 449.79, 111.78, 105.6, 976.54, 106.583};
        fvec res2 = VectorUtils<float>::AddSIMD(v3, v4);

        assert(VectorUtils<float>::VerifySameVectors(res2, target2));
    }

    void testDifferentVectorSizes() {
        dvec v1 = {1.5, 2.1, 8.7};
        dvec v2 = {3.1, 4.2, 1.6, 7.3};

        try {
            dvec res = VectorUtils<double>::Add(v1, v2);
            assert(res.empty());
        } catch (const std::runtime_error& e) {
            assert(true);
        }

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        fvec v4 = {302.15, 406.2, 19.6};

        try {
            fvec res = VectorUtils<float>::Add(v3, v4);
            assert(res.empty());
        } catch (const std::runtime_error& e) {
            assert(true);
        };
    }

    void testDifferentVectorSizesSIMD() {
        dvec v1 = {1.5, 2.1, 8.7};
        dvec v2 = {3.1, 4.2, 1.6, 7.3};
        dvec target = {4.6, 6.3, 10.3, 11.3};

        try {
            dvec res = VectorUtils<double>::AddSIMD(v1, v2);
            assert(res.empty());
        } catch (const std::runtime_error& e) {
            assert(true);
        }

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        fvec v4 = {302.15, 406.2, 19.6};

        try {
            fvec res = VectorUtils<float>::AddSIMD(v3, v4);
            assert(res.empty());
        } catch (const std::runtime_error& e) {
            assert(true);
        };
    }

    void testAddVectorScalar() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04};
        double b = 12.16;
        dvec target = {26.48, 21.16, 69.05, 33.2};
        dvec res = VectorUtils<double>::Add(v1, b);

        assert(VectorUtils<double>::VerifySameVectors(res, target));

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        float b2 = 45.2;
        fvec target2 = {169.07, 88.79, 137.38, 79.41, 101.2, 88.323};
        fvec res2 = VectorUtils<float>::Add(v3, b2);

        assert(VectorUtils<float>::VerifySameVectors(res2, target2));
    }

    void testAddVectorScalarSIMD() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04};
        double b = 12.16;
        dvec target = {26.48, 21.16, 69.05, 33.2};
        dvec res = VectorUtils<double>::AddSIMD(v1, b);

        assert(VectorUtils<double>::VerifySameVectors(res, target));

        fvec v3 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        float b2 = 45.2;
        fvec target2 = {169.07, 88.79, 137.38, 79.41, 101.2, 88.323};
        fvec res2 = VectorUtils<float>::AddSIMD(v3, b2);

        assert(VectorUtils<float>::VerifySameVectors(res2, target2));
    }

    void testLinearTransformation() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04};
        double a = 0.89;
        double b = 12.16;
        dvec target = {24.9048, 20.17, 62.7921, 30.8856};
        dvec res = VectorUtils<double>::LinearTransformation(v1, a, b);

        assert(VectorUtils<double>::VerifySameVectors(res, target));
    }

    void testLinearTransformationSIMD() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04};
        double a = 0.89;
        double b = 12.16;
        dvec target = {24.9048, 20.17, 62.7921, 30.8856};
        dvec res = VectorUtils<double>::LinearTransformationSIMD(v1, a, b);

        assert(VectorUtils<double>::VerifySameVectors(res, target));
    }

    void testMean() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04, 581.53, 0.2383, 46521.5321, 98.04, 21.52};
        double target = 5258.23449;
        double res = VectorUtils<double>::Mean(v1);
        assert(std::fabs(target - res) <= SystemConstants::EqualityThreshold);
    }

    void testMeanSIMD() {
        dvec v1 = {14.32, 9.0, 56.89, 21.04, 581.53, 0.2383, 46521.5321, 98.04, 21.52};
        double target = 5258.23449;
        double res = VectorUtils<double>::MeanSIMD(v1);
        assert(std::fabs(target - res) <= SystemConstants::EqualityThreshold);
    }

    void testReciprocal() {
        fvec v1 = {123.87, 43.59, 92.18, 34.21, 56.0, 43.123};
        fvec target = {0.008072, 0.022941, 0.010843, 0.0292312, 0.0178571, 0.0231894};
        fvec res = VectorUtils<float>::Reciprocal(v1);

        assert(VectorUtils<float>::VerifySameVectors(res, target));
    }
};

int main() {
    VectorUtilsTest vUtilsTest;
    vUtilsTest.testAll();
}