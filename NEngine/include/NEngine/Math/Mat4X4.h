#pragma once

#include <string>
#include "Vec4D.h"

namespace NEngine::Math {
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
    std::string ToString() const;
    bool operator==(const Mat4X4 &rhs) const;
};
}  // namespace NEngine::Math