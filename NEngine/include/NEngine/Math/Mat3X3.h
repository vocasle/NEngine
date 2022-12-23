#pragma once
#include <cstring>

#include "Vec3D.h"

namespace NEngine::Math {

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
    std::string ToString() const;
    float Determinant() const;
};
}  // namespace NEngine::Math