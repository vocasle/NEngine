#include "NEngine/Math/MathTest.h"

#include "NEngine/Math/Math.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine::Math {

void
TestVec3D()
{
    using namespace NEngine::Utils;

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
        auto e = 3.0f;
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
}

void
TestMat3X3()
{
    using namespace NEngine::Utils;

    {
        auto mat = Mat3X3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        UTILS_ASSERT(mat(0, 0) == 1 && mat(0, 1) == 2 && mat(0, 2) == 3 &&
                         mat(1, 0) == 4 && mat(1, 1) == 5 && mat(1, 2) == 6 &&
                         mat(2, 0) == 7 && mat(2, 1) == 8 && mat(2, 2) == 9,
                     "operator()(size_t, size_t) test failed");
        UtilsDebugPrint("operator()(size_t, size_t) test passed\n");
    }

    {
        auto m1 = Mat3X3();
        auto m2 = Mat3X3();
        m2(0, 0) = m2(1, 1) = m2(2, 2) = 1;
        UTILS_ASSERT(m2 == m1, "Mat3X3 is not identity by default");
        UtilsDebugPrint("Mat3X3 is identity by default\n");
    }

    {
        auto v0 = Vec3D(1, 2, 3);
        auto v1 = Vec3D(2, 4, 6);
        auto v2 = Vec3D(4, 8, 12);
        auto m1 = Mat3X3(1, 2, 3, 2, 4, 6, 4, 8, 12);
        UTILS_ASSERT(v0 == m1[0] && v1 == m1[1] && v2 == m1[2],
                     "operator[](size_t) test failed");
        UtilsDebugPrint("operator[](size_t) test passed\n");
    }

    {
        auto m1 = Mat3X3(10);
        auto m2 = Mat3X3(1);
        auto res = m2 * 10;
        UTILS_ASSERT(res == m1, "operator*(Mat3X3, float) test failed");
        UtilsDebugPrint("operator*(Mat3X3, float) test passed\n");
    }

    {
        auto m1 = Mat3X3(5);
        auto m2 = Mat3X3(10);
        auto m3 = m1 + m1;
        UTILS_ASSERT(m3 == m2, "operator+(Mat3X3, Mat3X3) test failed");
        UtilsDebugPrint("operator+(Mat3X3, Mat3X3) test passed\n");
    }

    {
        auto m1 = Mat3X3(10);
        auto m2 = Mat3X3(5);
        auto res = m1 - m2;
        UTILS_ASSERT(res == m2, "operator-(Mat3X3, Mat3X3) test failed");
        UtilsDebugPrint("operator-(Mat3X3, Mat3X3) test passed\n");
    }

    {
        auto m = Mat3X3::RotZ(ToRadians(90));
        auto v = Vec3D(1, 0, 0);
        auto e = Vec3D(0, 1, 0);
        auto r = m * v;

        UTILS_ASSERT(NearlyEqual(r, e), "Mat3X3::RotZ test failed");
        UtilsDebugPrint("Mat3X3::RotZ test passed\n");
    }

    {
        auto res0 = false;
        auto res1 = false;

        {
            auto v0 = Vec3D(1, 1, 1);
            auto exp_v0 = Vec3D(5, 5, 5);
            auto m0 = Mat3X3() * 5;
            auto res_v0 = m0 * v0;
            res0 = exp_v0 == res_v0;
        }

        {
            auto v = Vec3D(1, 0, 0);
            auto exp = Vec3D(0, 1, 0);
            auto m = Mat3X3::RotZ(ToRadians(90));
            auto res = m * v;
            res1 = res == exp;
        }

        UTILS_ASSERT(res0 && res1, "operator*(Vec3D, Mat3X3) test failed");
        UtilsDebugPrint("operator*(Vec3D, Mat3X3) test passed\n");
    }
}

void
MathTest()
{
    TestVec3D();
}
}  // namespace NEngine::Math