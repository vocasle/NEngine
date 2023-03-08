#include "NEngine/Math/MathTest.h"

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

#ifdef _DEBUG
#define UTILS_ASSERT(expr, msg) assert((expr) && (msg))
#else
#define UTILS_ASSERT(...)
#endif

namespace NEngine::Math {

void
TestVec3D()
{
#if NENGINE_VERBOSE_MATH_TEST
    UtilsDebugPrint("\n==============================\n");
    UtilsDebugPrint("Running tests for vec3\n");
    UtilsDebugPrint("==============================\n");
#endif

    {
        auto v = vec3();

        UTILS_ASSERT(v.X == 0 && v.Y == 0 && v.Z == 0, "vec3() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("vec3() test passed\n");
#endif
    }

    {
        auto v = vec3();

        UTILS_ASSERT(v.X == 0 && v.Y == 0 && v.Z == 0, "vec3() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("vec3() test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);

        UTILS_ASSERT(v.X == 1 && v.Y == 2 && v.Z == 3,
                     "vec3(float, float, float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("vec3(float, float, float) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto vCopy = v;

        UTILS_ASSERT(NearlyEqual(v, vCopy), "vec3(const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("vec3(const vec3 &) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto vCopy = vec3(vec3(1, 2, 3));

        UTILS_ASSERT(NearlyEqual(v, vCopy), "vec3(vec3 &&) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("vec3(vec3 &&) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto v2 = vec3(3, 2, 1);
        v = v2;

        UTILS_ASSERT(NearlyEqual(v2, v), "operator=(const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator=(const vec3 &) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto v2 = vec3(3, 2, 1);
        v = vec3(3, 2, 1);

        UTILS_ASSERT(NearlyEqual(v2, v), "operator=(vec3 &&) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator=(vec3 &&) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto v2 = vec3(1, 2, 3);

        UTILS_ASSERT(v == v2, "operator==(const vec3 &rhs) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator==(const vec3 &rhs) test passed\n");
#endif
    }

    {
        auto v = vec3(1.00001f, 2, 3);
        auto v2 = vec3(1.0001f, 2, 3);

        UTILS_ASSERT(v != v2, "operator!=(const vec3 &rhs) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator!=(const vec3 &rhs) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 3);
        auto str = std::string("{ 1, 2, 3 }");

        UTILS_ASSERT(v.ToString() == str, "ToString() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("ToString() test passed\n");
#endif
    }

    {
        auto i = vec3(1, 0, 0);
        auto j = vec3(0, 1, 0);
        auto e = vec3(0, 0, 1);
        auto k = vec3::Cross(i, j);

        UTILS_ASSERT(
            NearlyEqual(e, k) && NearlyEqual(vec3::Cross(j, i), k * (-1)),
            "Cross(const vec3 &, const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Cross(const vec3 &, const vec3 &) test passed\n");
#endif
    }

    {
        auto v1 = vec3(1, 2, 3);
        auto v2 = vec3(2, 2, 1);
        auto e = 9.0f;
        auto res = vec3::Dot(v1, v2);

        UTILS_ASSERT(NearlyEqual(e, res),
                     "Dot(const vec3 &, const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Dot(const vec3 &, const vec3 &) test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 2);
        auto e = 3.0f;
        auto r = v.Length();

        UTILS_ASSERT(NearlyEqual(e, r), "Length() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Length() test passed\n");
#endif
    }

    {
        auto v = vec3(1, 2, 2);
        auto len = v.Length();
        auto e = vec3(1 / len, 2 / len, 2 / len);
        auto r = v.Normalize();

        UTILS_ASSERT(NearlyEqual(e, r), "Normalize() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Normalize() test passed\n");
#endif
    }

    {
        auto v1 = vec3(3, 2, 5);
        auto v2 = vec3(1, 4, 6);
        auto e = vec3(4, 6, 11);
        auto r = v1 + v2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator+(const vec3 &, const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator+(const vec3 &, const vec3 &) test passed\n");
#endif
    }

    {
        auto v1 = vec3(3, 2, 5);
        auto v2 = vec3(1, 4, 6);
        auto e = vec3(2, -2, -1);
        auto r = v1 - v2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator-(const vec3 &, const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator-(const vec3 &, const vec3 &) test passed\n");
#endif
    }

    {
        auto v1 = vec3(3, 2, 5);
        auto e = vec3(6, 4, 10);
        auto r = v1 * 2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator*(const vec3 &, const float &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator*(const vec3 &, const float &) test passed\n");
#endif
    }

    {
        auto v1 = vec3(3, 2, 5);
        auto e = vec3(1.5f, 1, 2.5f);
        auto r = v1 / 2;

        UTILS_ASSERT(NearlyEqual(e, r),
                     "operator/(const vec3 &, const float &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator/(const vec3 &, const float &) test passed\n");
#endif
    }
}

void
TestMat3X3()
{
#if NENGINE_VERBOSE_MATH_TEST
    UtilsDebugPrint("\n==============================\n");
    UtilsDebugPrint("Running tests for mat3\n");
    UtilsDebugPrint("==============================\n");
#endif

    {
        auto mat = mat3();

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

        UTILS_ASSERT(isIdentity, "mat3() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("mat3() test passed\n");
#endif
    }

    {
        auto mat = mat3(5);

        auto isPassed = true;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                isPassed = isPassed && mat(i, j) == 5;
            }
        }

        UTILS_ASSERT(isPassed, "mat3(float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("mat3(float) test passed\n");
#endif
    }

    {
        auto mat = mat3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        UTILS_ASSERT(mat(0, 0) == 1 && mat(1, 0) == 4 && mat(2, 0) == 7 &&
                         mat(0, 1) == 2 && mat(1, 1) == 5 && mat(2, 1) == 8 &&
                         mat(0, 2) == 3 && mat(1, 2) == 6 && mat(2, 2) == 9,
                     "mat3(float, float, float,\n"
                     "\tfloat, float, float,\n"
                     "\tfloat, float, float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint(
            "mat3(float, float, float,\n"
            "\tfloat, float, float,\n"
            "\tfloat, float, float) test passed\n");
#endif
    }

    {
        auto mat = mat3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        UTILS_ASSERT(mat(0, 0) == 1 && mat(1, 0) == 4 && mat(2, 0) == 7 &&
                         mat(0, 1) == 2 && mat(1, 1) == 5 && mat(2, 1) == 8 &&
                         mat(0, 2) == 3 && mat(1, 2) == 6 && mat(2, 2) == 9,
                     "operator()(size_t, size_t) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator()(size_t, size_t) test passed\n");
#endif
    }

    {
        auto mat = mat3(1, 2, 3, 4, 5, 6, 7, 8, 9);

        auto v0 = vec3(1, 2, 3);
        auto v1 = vec3(4, 5, 6);
        auto v2 = vec3(7, 8, 9);

        UTILS_ASSERT(v0 == mat[0] && v1 == mat[1] && v2 == mat[2],
                     "operator[](size_t) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("operator[](size_t) test passed\n");
#endif
    }

    {
        auto m0 = mat3(5);
        auto m1 = mat3(5);
        auto e = mat3(10);
        auto r = m0 + m1;
        UTILS_ASSERT(r == e, "Add(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Add(const mat3 &, const mat3 &) test passed\n");
#endif
    }

    {
        auto m0 = mat3(5);
        auto m1 = mat3(5);
        auto e = mat3(0);
        auto r = m0 - m1;
        UTILS_ASSERT(r == e,
                     "Subtract(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Subtract(const mat3 &, const mat3 &) test passed\n");
#endif
    }

    {
        auto scale = mat3() * 4;
        auto rotZ = mat3::RotZ(ToRadians(90));
        auto e = mat3(0, 4, 0, -4, 0, 0, 0, 0, 4);
        auto r = mat3::Mult(scale, rotZ);
        UTILS_ASSERT(r == e, "Mult(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Mult(const mat3 &, const mat3 &) test passed\n");
#endif
    }

    {
        auto m = mat3() * 4;
        auto e = vec3(8, 8, 8);
        auto r = mat3::Mult(m, vec3(2, 2, 2));

        UTILS_ASSERT(r == e, "Mult(const mat3 &, const vec3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Mult(const mat3 &, const vec3 &) test passed\n");
#endif
    }

    {
        auto m = mat3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = 56.0f;
        auto r = m.Determinant();

        UTILS_ASSERT(r == e, "Determinant() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UtilsDebugPrint("Determinant() test passed\n");
#endif
    }

    {
        auto m = mat3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = mat3(4, 2, 8, 4, 1, 6, 7, 8, 9);
        auto r = m.Transpose();

        UTILS_ASSERT(e == r, "Transpose() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("Transpose() test passed");
#endif
    }

    {
        auto m = mat3(4, 4, 7, 2, 1, 8, 8, 6, 9);
        auto e = mat3(-0.696429f,
                      0.821429f,
                      0.071429f,
                      0.107143f,
                      -0.357143f,
                      0.142857f,
                      0.446429f,
                      -0.321429f,
                      -0.071429f);
        auto ei = mat3();
        auto r = m.Inverse();
        auto ri = r * m;

        UTILS_ASSERT(ri == ei, "Inverse() test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("Inverse() test passed");
#endif
    }

    {
        auto m = mat3::RotX(ToRadians(90));
        auto v = vec3(0, 1, 0);
        auto e = vec3(0, 0, 1);
        auto r = m * v;
        UTILS_ASSERT(e == r, "RotX(float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("RotX(float) test passed");
#endif
    }

    {
        auto m = mat3::RotY(ToRadians(90));
        auto v = vec3(0, 0, 1);
        auto e = vec3(1, 0, 0);
        auto r = m * v;
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("m=%s", m.ToString().c_str());
        UTILS_PRINTLN("e=%s, r=%s", e.ToString().c_str(), r.ToString().c_str());
#endif
        UTILS_ASSERT(e == r, "RotY(float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("RotY(float) test passed");
#endif
    }

    {
        auto m = mat3::RotZ(ToRadians(90));
        auto v = vec3(1, 0, 0);
        auto e = vec3(0, 1, 0);
        auto r = m * v;
        UTILS_ASSERT(e == r, "RotZ(float) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("RotZ(float) test passed");
#endif
    }

    {
        auto m1 = mat3(5);
        auto m2 = mat3(5);
        auto e = mat3(10);
        auto r = m1 + m2;
        UTILS_ASSERT(e == r,
                     "operator+(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("operator+(const mat3 &, const mat3 &) test passed");
#endif
    }

    {
        const auto m1 = mat3(10);
        const auto m2 = mat3(50);
        const auto e = mat3(-40);
        const auto r = m1 - m2;
        UTILS_ASSERT(e == r,
                     "operator-(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("operator-(const mat3 &, const mat3 &) test passed");
#endif
    }

    {
        const auto m1 = mat3::RotY(ToRadians(45));
        const auto m2 = mat3::RotY(ToRadians(45));
        const auto e = vec3(1, 0, 0);
        const auto r = mat3::Mult(m1 * m2, vec3(0, 0, 1));
        UTILS_ASSERT(e == r,
                     "operator*(const mat3 &, const mat3 &) test failed");
#if NENGINE_VERBOSE_MATH_TEST
        UTILS_PRINTLN("operator*(const mat3 &, const mat3 &) test passed");
#endif
    }
}

void
MathTest()
{
#if NENGINE_USE_DIRECTXMATH
    static_assert(sizeof vec2 == sizeof XMFLOAT2,
                  "Sizes of vec2 and XMFLOAT2 does not match!");
    static_assert(sizeof vec3 == sizeof XMFLOAT3,
                  "Sizes of vec3 and XMFLOAT3 does not match!");
    static_assert(sizeof vec4 == sizeof XMFLOAT4,
                  "Sizes of vec4 and XMFLOAT4 does not match!");
    static_assert(sizeof mat3 == sizeof XMFLOAT3X3,
                  "Sizes of mat3 and XMFLOAT3X3 does not match!");
    static_assert(sizeof mat4 == sizeof XMFLOAT4X4,
                  "Sizes of mat4 and XMFLOAT4X4 does not match!");
#endif
    TestVec3D();
    TestMat3X3();
}
}  // namespace NEngine::Math
