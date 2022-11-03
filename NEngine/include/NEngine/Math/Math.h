#pragma once
#include <cstdint>
#include <cstring>
#ifdef __cplusplus
#include <sstream>
#endif

namespace NEngine {
namespace Math {
struct Vec2D
{
    Vec2D()
        : X{0},
          Y{0}
    {
    }

    Vec2D(float x, float y)
        : X{x},
          Y{y}
    {
    }
#ifdef __cplusplus
    std::string ToString() const;

    Vec2D(const Vec2D &rhs) = default;
    Vec2D(Vec2D &&rhs) noexcept = default;
    Vec2D &operator=(const Vec2D &rhs) = default;
    Vec2D &operator=(Vec2D &&rhs) noexcept = default;
#endif
    float X;
    float Y;
};

struct Vec3D
{
    Vec3D()
        : X{0},
          Y{0},
          Z{0}
    {
    }

    Vec3D(float x, float y, float z)
        : X{x},
          Y{y},
          Z{z}
    {
    }
#ifdef __cplusplus
    std::string ToString() const;
    Vec3D Cross(const Vec3D &rhs) const;
    float Dot(const Vec3D &rhs) const;
    float Length() const;
    void Normalize();
    bool operator==(const Vec3D &rhs) const;
    bool operator!=(const Vec3D &rhs) const;

    Vec3D(const Vec3D &rhs) = default;
    Vec3D(Vec3D &&rhs) noexcept = default;
    Vec3D &operator=(const Vec3D &rhs) = default;
    Vec3D &operator=(Vec3D &&rhs) noexcept = default;
#endif
    float X;
    float Y;
    float Z;
};

#ifdef __cplusplus
Vec3D operator+(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator-(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator*(const Vec3D &lhs, const float s);
Vec3D operator*(const float s, const Vec3D &rhs);
#endif

struct Vec4D
{
    Vec4D()
        : X{0},
          Y{0},
          Z{0},
          W{0}
    {
    }

    Vec4D(float x, float y, float z, float w)
        : X{x},
          Y{y},
          Z{z},
          W{w}
    {
    }
#ifdef __cplusplus
    Vec4D(const Vec3D &v, float w)
        : Vec4D(v.X, v.Y, v.Z, w)
    {
    }

    Vec4D(const Vec4D &rhs) = default;
    Vec4D(Vec4D &&rhs) noexcept = default;
    Vec4D &operator=(const Vec4D &rhs) = default;
    Vec4D &operator=(Vec4D &&rhs) noexcept = default;

    std::string ToString() const;
    Vec4D &operator*(const float scalar);
    Vec4D &operator/(const float scalar);
    bool operator==(const Vec4D &rhs) const;
#endif
    float X;
    float Y;
    float Z;
    float W;
};

struct Mat3X3
{
    Mat3X3()
    {
        memset(this, 0, sizeof(Mat3X3));
    }

    union
    {
        struct
        {
            float A00, A01, A02;
            float A10, A11, A12;
            float A20, A21, A22;
        };

        Vec3D V[3];
        float A[3][3];
    };
#ifdef __cplusplus
    std::string ToString() const;
    float Determinant() const;
#endif
};

struct Mat4X4
{
    Mat4X4()
        : Mat4X4(0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f)
    {
    }

    Mat4X4(float a00,
           float a01,
           float a02,
           float a03,
           float a10,
           float a11,
           float a12,
           float a13,
           float a20,
           float a21,
           float a22,
           float a23,
           float a30,
           float a31,
           float a32,
           float a33)
        : A00{a00},
          A01{a01},
          A02{a02},
          A03{a03},
          A10{a10},
          A11{a11},
          A12{a12},
          A13{a13},
          A20{a20},
          A21{a21},
          A22{a22},
          A23{a23},
          A30{a30},
          A31{a31},
          A32{a32},
          A33{a33}
    {
    }

    Mat4X4(const float *data)
        : Mat4X4(data[0],
                 data[1],
                 data[2],
                 data[3],
                 data[4],
                 data[5],
                 data[6],
                 data[7],
                 data[8],
                 data[9],
                 data[10],
                 data[11],
                 data[12],
                 data[13],
                 data[14],
                 data[15])
    {
    }

    union
    {
        struct
        {
            float A00, A01, A02, A03;
            float A10, A11, A12, A13;
            float A20, A21, A22, A23;
            float A30, A31, A32, A33;
        };

        Vec4D V[4];
        float A[4][4];
    };
#ifdef __cplusplus
    std::string ToString() const;
    bool operator==(const Mat4X4 &rhs) const;
#endif
};

// *** 2D vector math ***
Vec2D MathVec2DZero(void);

void MathVec2DModulateByVec2D(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DModulateByScalar(const Vec2D *vec1, float s, Vec2D *out);

void MathVec2DAddition(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DSubtraction(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

float MathVec2DDot(const Vec2D *vec1, const Vec2D *vec2);

void MathVec2DProj(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DPerp(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DNormalize(Vec2D *vec1);

// *** 3D vector math ***
Vec3D MathVec3DZero(void);

Vec3D MathVec3DFromXYZ(float x, float y, float z);

void MathVec3DModulateByVec3D(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

Vec3D MathVec3DModulateByScalar(const Vec3D *vec1, float s);

Vec3D MathVec3DAddition(const Vec3D *vec1, const Vec3D *vec2);

Vec3D MathVec3DSubtraction(const Vec3D *vec1, const Vec3D *vec2);

float MathVec3DDot(const Vec3D *vec1, const Vec3D *vec2);

void MathVec3DProj(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

void MathVec3DPerp(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

Vec3D MathVec3DCross(const Vec3D *vec1, const Vec3D *vec2);

void MathVec3DNormalize(Vec3D *vec1);

void MathVec3DNegate(Vec3D *vec);

void MathVec3DPrint(const Vec3D *vec);

// *** 4D vector math ***
Vec4D MathVec4DZero(void);

Vec4D MathVec4DFromXYZW(float x, float y, float z, float w);

void MathVec4DModulateByVec4D(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out);

void MathVec4DModulateByScalar(const Vec4D *vec1, float s, Vec4D *out);

Vec4D MathVec4DAddition(const Vec4D *vec1, const Vec4D *vec2);

void MathVec4DSubtraction(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out);

float MathVec4DDot(const Vec4D *vec1, const Vec4D *vec2);

void MathVec4DNormalize(Vec4D *vec1);

void MathVec4DPrint(const Vec4D *vec);

Vec4D MathVec4DVectorPermute(Vec4D V1,
                             Vec4D V2,
                             uint32_t PermuteX,
                             uint32_t PermuteY,
                             uint32_t PermuteZ,
                             uint32_t PermuteW);

// *** 3X3 matrix math ***
Mat3X3 MathMat3X3Identity(void);

Mat3X3 MathMat3X3Addition(const Mat3X3 *mat1, const Mat3X3 *mat2);

void MathMat3X3ModulateByScalar(Mat3X3 *mat, float s);

Vec3D MathMat3X3MultByVec3D(const Mat3X3 *mat, const Vec3D *vec);

Mat3X3 MathMat3X3MultByMat3X3(const Mat3X3 *mat1, const Mat3X3 *mat2);

void MathMat3X3Transpose(Mat3X3 *mat);

void MathMat3X3Copy(const Mat3X3 *from, Mat3X3 *to);

// *** 4X4 matrix math ***
Mat4X4 MathMat4X4Identity(void);

void MathMat4X4Copy(const Mat4X4 *from, Mat4X4 *to);

void MathMat4X4ModulateByScalar(Mat4X4 *mat, float s);

Mat4X4 MathMat4X4Addition(const Mat4X4 *mat1, const Mat4X4 *mat2);

Vec4D MathMat4X4MultVec4DByMat4X4(const Vec4D *vec, const Mat4X4 *mat);

Mat4X4 MathMat4X4MultMat4X4ByMat4X4(const Mat4X4 *mat1, const Mat4X4 *mat2);

void MathMat4X4Transpose(Mat4X4 *mat);

Mat4X4 MathMat4X4ScaleFromVec3D(const Vec3D *scale);

Mat4X4 MathMat4X4TranslateFromVec3D(const Vec3D *offset);

Mat4X4 MathMat4X4RotateFromVec3D(const Vec3D *angles);

float MathMat4X4Determinant(const Mat4X4 *mat);

void MathMat4X4Normalize(Mat4X4 *mat);

Mat4X4 MathMat4X4Orthographic(float viewWidth,
                              float viewHeight,
                              float zNear,
                              float zFar);

Mat4X4 MathMat4X4ViewAt(const Vec3D *eyePos,
                        const Vec3D *focusPos,
                        const Vec3D *upDirect);

Mat4X4 MathMat4X4RotateZ(float angle);

Mat4X4 MathMat4X4RotateX(float angle);

Mat4X4 MathMat4X4RotateY(float angle);

Mat4X4 MathMat4X4PerspectiveFov(float fovAngleY,
                                float aspectRatio,
                                float nearZ,
                                float farZ);

Mat4X4 MathMat4X4OrthographicOffCenter(float viewLeft,
                                       float viewRight,
                                       float viewBottom,
                                       float viewTop,
                                       float nearZ,
                                       float farZ);

Mat4X4 MathMat4X4Inverse(const Mat4X4 *mat);

// *** misc math helpers ***
float MathClamp(float min, float max, float v);

float MathToRadians(float degrees);

float MathToDegrees(float radians);

unsigned int MathNearlyEqual(const float lhs, const float rhs);

int32_t MathIsNaN(float n);

float MathRandom(float min, float max);

#ifdef MATH_TEST
void MathTest(void);
#endif

Mat4X4 MathQuaternionToRotationMat(const Vec4D &quat);

#ifdef __cplusplus
Mat4X4 operator*(const Mat4X4 &lhs, const Mat4X4 &rhs);

Vec3D MathVec3DSubtraction(const Vec3D &vec1, const Vec3D &vec2);

Vec4D MathVec4DAddition(const Vec4D &vec1, const Vec4D &vec2);

float MathVec3DLength(const Vec3D &v);

Mat4X4 MathMat4X4Inverse(const Mat4X4 &mat);
#endif
}  // namespace Math

}  // namespace NEngine
