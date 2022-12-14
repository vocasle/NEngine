#include "NEngine/Math/MathTest.h"

#include "NEngine/Math/Math.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine::Math {

void
TestVec3D()
{
    using namespace NEngine::Utils;
    UtilsDebugPrint("\n==============================\n");
    UtilsDebugPrint("Running tests for Vec3D\n");
    UtilsDebugPrint("==============================\n");

    {
        auto v = Vec3D();

        UTILS_ASSERT(v.X == 0 && v.Y == 0 && v.Z == 0, "Vec3D() test failed");
        UtilsDebugPrint("Vec3D() test passed\n");
    }

    {
        auto v = Vec3D();

        UTILS_ASSERT(v.X == 0 && v.Y == 0 && v.Z == 0, "Vec3D() test failed");
        UtilsDebugPrint("Vec3D() test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);

        UTILS_ASSERT(v.X == 1 && v.Y == 2 && v.Z == 3,
                     "Vec3D(float, float, float) test failed");
        UtilsDebugPrint("Vec3D(float, float, float) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto vCopy = v;

        UTILS_ASSERT(NearlyEqual(v, vCopy), "Vec3D(const Vec3D &) test failed");
        UtilsDebugPrint("Vec3D(const Vec3D &) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto vCopy = Vec3D(Vec3D(1, 2, 3));

        UTILS_ASSERT(NearlyEqual(v, vCopy), "Vec3D(Vec3D &&) test failed");
        UtilsDebugPrint("Vec3D(Vec3D &&) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto v2 = Vec3D(3, 2, 1);
        v = v2;

        UTILS_ASSERT(NearlyEqual(v2, v),
                     "operator=(const Vec3D &) test failed");
        UtilsDebugPrint("operator=(const Vec3D &) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto v2 = Vec3D(3, 2, 1);
        v = Vec3D(3, 2, 1);

        UTILS_ASSERT(NearlyEqual(v2, v), "operator=(Vec3D &&) test failed");
        UtilsDebugPrint("operator=(Vec3D &&) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto v2 = Vec3D(1, 2, 3);

        UTILS_ASSERT(v == v2, "operator==(const Vec3D &rhs) test failed");
        UtilsDebugPrint("operator==(const Vec3D &rhs) test passed\n");
    }

    {
        auto v = Vec3D(1.00001f, 2, 3);
        auto v2 = Vec3D(1.0001f, 2, 3);

        UTILS_ASSERT(v != v2, "operator!=(const Vec3D &rhs) test failed");
        UtilsDebugPrint("operator!=(const Vec3D &rhs) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 3);
        auto str = std::string("{ 1, 2, 3 }");

        UTILS_ASSERT(v.ToString() == str, "ToString() test failed");
        UtilsDebugPrint("ToString() test passed\n");
    }

    {
        auto i = Vec3D(1, 0, 0);
        auto j = Vec3D(0, 1, 0);
        auto e = Vec3D(0, 0, 1);
        auto k = Vec3D::Cross(i, j);

        UTILS_ASSERT(
            NearlyEqual(e, k) && NearlyEqual(Vec3D::Cross(j, i), k * (-1)),
            "Cross(const Vec3D &, const Vec3D &) test failed");
        UtilsDebugPrint("Cross(const Vec3D &, const Vec3D &) test passed\n");
    }

    {
        auto v1 = Vec3D(1, 2, 3);
        auto v2 = Vec3D(2, 2, 1);
        auto e = 9.0f;
        auto res = Vec3D::Dot(v1, v2);

        UTILS_ASSERT(NearlyEqual(e, res),
                     "Dot(const Vec3D &, const Vec3D &) test failed");
        UtilsDebugPrint("Dot(const Vec3D &, const Vec3D &) test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 2);
        auto e = 3.0f;
        auto r = v.Length();

        UTILS_ASSERT(NearlyEqual(e, r), "Length() test failed");
        UtilsDebugPrint("Length() test passed\n");
    }

    {
        auto v = Vec3D(1, 2, 2);
        auto len = v.Length();
        auto e = Vec3D(1 / len, 2 / len, 2 / len);
        auto r = v.Normalize();

        UTILS_ASSERT(NearlyEqual(e, r), "Normalize() test failed");
        UtilsDebugPrint("Normalize() test passed\n");
    }

    {
        auto v1 = Vec3D(3, 2, 5);
        auto v2 = Vec3D(1, 4, 6);
        auto e = Vec3D(4, 6, 11);
        auto r = v1 + v2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator+(const Vec3D &, const Vec3D &) test failed");
        UtilsDebugPrint(
            "operator+(const Vec3D &, const Vec3D &) test passed\n");
    }

    {
        auto v1 = Vec3D(3, 2, 5);
        auto v2 = Vec3D(1, 4, 6);
        auto e = Vec3D(2, -2, -1);
        auto r = v1 - v2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator-(const Vec3D &, const Vec3D &) test failed");
        UtilsDebugPrint(
            "operator-(const Vec3D &, const Vec3D &) test passed\n");
    }

    {
        auto v1 = Vec3D(3, 2, 5);
        auto e = Vec3D(6, 4, 10);
        auto r = v1 * 2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator*(const Vec3D &, const float &) test failed");
        UtilsDebugPrint(
            "operator*(const Vec3D &, const float &) test passed\n");
    }

    {
        auto v1 = Vec3D(3, 2, 5);
        auto e = Vec3D(1.5f, 1, 2.5f);
        auto r = v1 / 2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator/(const Vec3D &, const float &) test failed");
        UtilsDebugPrint(
            "operator/(const Vec3D &, const float &) test passed\n");
    }
}

void
TestMat3X3()
{
    using namespace NEngine::Utils;

    UtilsDebugPrint("\n==============================\n");
    UtilsDebugPrint("Running tests for Mat3X3\n");
    UtilsDebugPrint("==============================\n");

    {
        auto mat = Mat3X3();

        auto isIdentity = true;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                if (i != j) {
                    isIdentity = isIdentity && mat(i, j) == 0;
                }
                else {
                    isIdentity = isIdentity && mat(i, j) == 1;
                }
            }
        }

        UTILS_ASSERT(isIdentity, "Mat3X3() test failed");
        UtilsDebugPrint("Mat3X3() test passed\n");
    }

    {
        auto mat = Mat3X3(5);

        auto isPassed = true;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                isPassed = isPassed && mat(i, j) == 5;
            }
        }

        UTILS_ASSERT(isPassed, "Mat3X3(float) test failed");
        UtilsDebugPrint("Mat3X3(float) test passed\n");
    }

    {
        auto mat = Mat3X3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        UTILS_ASSERT(mat(0, 0) == 1 && mat(1, 0) == 2 && mat(2, 0) == 3 &&
                         mat(0, 1) == 4 && mat(1, 1) == 5 && mat(2, 1) == 6 &&
                         mat(0, 2) == 7 && mat(1, 2) == 8 && mat(2, 2) == 9,
                     "Mat3X3(float, float, float,\n"
                     "\tfloat, float, float,\n"
                     "\tfloat, float, float) test failed");
        UtilsDebugPrint(
            "Mat3X3(float, float, float,\n"
            "\tfloat, float, float,\n"
            "\tfloat, float, float) test passed\n");
    }

    {
        auto mat = Mat3X3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        UTILS_ASSERT(mat(0, 0) == 1 && mat(1, 0) == 2 && mat(2, 0) == 3 &&
                         mat(0, 1) == 4 && mat(1, 1) == 5 && mat(2, 1) == 6 &&
                         mat(0, 2) == 7 && mat(1, 2) == 8 && mat(2, 2) == 9,
                     "operator()(size_t, size_t) test failed");
        UtilsDebugPrint("operator()(size_t, size_t) test passed\n");
    }

    {
        auto mat = Mat3X3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        auto v0 = Vec3D(1, 2, 3);
        auto v1 = Vec3D(4, 5, 6);
        auto v2 = Vec3D(7, 8, 9);

        UTILS_ASSERT(v0 == mat[0] && v1 == mat[1] && v2 == mat[2],
                     "operator[](size_t) test failed");
        UtilsDebugPrint("operator[](size_t) test passed\n");
    }

    {
        auto m0 = Mat3X3(5);
        auto m1 = Mat3X3(5);
        auto e = Mat3X3(10);
        auto r = m0 + m1;
        UTILS_ASSERT(r == e, "Add(const Mat3X3 &, const Mat3X3 &) test failed");
        UtilsDebugPrint("Add(const Mat3X3 &, const Mat3X3 &) test passed\n");
    }

    {
        auto m0 = Mat3X3(5);
        auto m1 = Mat3X3(5);
        auto e = Mat3X3(0);
        auto r = m0 - m1;
        UTILS_ASSERT(r == e,
                     "Subtract(const Mat3X3 &, const Mat3X3 &) test failed");
        UtilsDebugPrint(
            "Subtract(const Mat3X3 &, const Mat3X3 &) test passed\n");
    }

    {
        auto scale = Mat3X3() * 4;
        auto rotZ = Mat3X3::RotZ(ToRadians(90));
        auto e = Mat3X3(0, 4, 0, -4, 0, 0, 0, 0, 4);
        auto r = Mat3X3::Mult(scale, rotZ);
        UTILS_ASSERT(r == e,
                     "Mult(const Mat3X3 &, const Mat3X3 &) test failed");
        UtilsDebugPrint("Mult(const Mat3X3 &, const Mat3X3 &) test passed\n");
    }

    {
        auto m = Mat3X3() * 4;
        auto e = Vec3D(8, 8, 8);
        auto r = Mat3X3::Mult(m, Vec3D(2, 2, 2));

        UTILS_ASSERT(r == e, "Mult(const Mat3X3 &, const Vec3D &) test failed");
        UtilsDebugPrint("Mult(const Mat3X3 &, const Vec3D &) test passed\n");
    }

    {
        auto m = Mat3X3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = 56.0f;
        auto r = m.Determinant();

        UTILS_ASSERT(r == e, "Determinant() test failed");
        UtilsDebugPrint("Determinant() test passed\n");
    }

    {
        auto m = Mat3X3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = Mat3X3(4, 2, 8, 4, 1, 6, 7, 8, 9);
        auto r = m.Transpose();
        UTILS_ASSERT(e == r, "Transpose() test failed");
        UTILS_PRINTLN("Transpose() test passed");
    }

    {
        auto m = Mat3X3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = Mat3X3(-0.696429f,
                        0.821429f,
                        0.071429f,
                        0.107143f,
                        -0.357143f,
                        0.142857f,
                        0.446429f,
                        -0.321429f,
                        -0.071429f);
        auto ei = Mat3X3();
        auto r = m.Inverse();
        auto ri = r * m;

        UTILS_ASSERT(ri == ei, "Inverse() test failed");
        UTILS_PRINTLN("Inverse() test passed");
    }

    {
        auto m = Mat3X3::RotX(ToRadians(90));
        auto v = Vec3D(0, 1, 0);
        auto e = Vec3D(0, 0, 1);
        auto r = m * v;
        UTILS_ASSERT(e == r, "RotX(float) test failed");
        UTILS_PRINTLN("RotX(float) test passed");
    }

    {
        auto m = Mat3X3::RotY(ToRadians(90));
        auto v = Vec3D(0, 0, 1);
        auto e = Vec3D(1, 0, 0);
        auto r = m * v;
        UTILS_PRINTLN("m=%s", m.ToString().c_str());
        UTILS_PRINTLN("e=%s, r=%s", e.ToString().c_str(),
                r.ToString().c_str());
        UTILS_ASSERT(e == r, "RotY(float) test failed");
        UTILS_PRINTLN("RotY(float) test passed");
    }

    {
        auto m = Mat3X3::RotZ(ToRadians(90));
        auto v = Vec3D(1, 0, 0);
        auto e = Vec3D(0, 1, 0);
        auto r = m * v;
        UTILS_ASSERT(e == r, "RotZ(float) test failed");
        UTILS_PRINTLN("RotZ(float) test passed");
    }

    {
        auto m1 = Mat3X3(5);
        auto m2 = Mat3X3(5);
        auto e = Mat3X3(10);
        auto r = m1 + m2;
        UTILS_ASSERT(e == r,
                     "operator+(const Mat3X3 &, const Mat3X3 &) test failed");
        UTILS_PRINTLN("operator+(const Mat3X3 &, const Mat3X3 &) test passed");
    }

    {
        const auto m1 = Mat3X3(10);
        const auto m2 = Mat3X3(50);
        const auto e = Mat3X3(-40);
        const auto r = m1 - m2;
        UTILS_ASSERT(e == r, "operator-(const Mat3X3 &, const Mat3X3 &) test failed");

        UTILS_PRINTLN("operator-(const Mat3X3 &, const Mat3X3 &) test passed");
    } 

    {
        const auto m1 = Mat3X3::RotY(ToRadians(45));
        const auto m2 = Mat3X3::RotY(ToRadians(45));
        const auto e = Vec3D(1, 0, 0);
        const auto r = Mat3X3::Mult(m1 * m2, Vec3D(0, 0, 1));
        UTILS_ASSERT(e == r, "operator*(const Mat3X3 &, const Mat3X3 &) test failed");

        UTILS_PRINTLN("operator*(const Mat3X3 &, const Mat3X3 &) test passed");

    }
}

void
MathTest()
{
    TestVec3D();
    TestMat3X3();
}
}  // namespace NEngine::Math
