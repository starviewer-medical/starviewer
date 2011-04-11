//-----------------------------------------------------------------------------
// Copyright (c) 2005-2007 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------


#include "quaternion.h"


namespace udg {


const Quaternion Quaternion::Identity(1.0f, 0.0f, 0.0f, 0.0f);

Quaternion Quaternion::slerp(const Quaternion &a, const Quaternion &b, float t)
{
    // Smoothly interpolates from quaternion 'a' to quaternion 'b' using
    // spherical linear interpolation.
    //
    // Both quaternions must be unit length and represent absolute rotations.
    // In particular quaternion 'b' must not be relative to quaternion 'a'.
    // If 'b' is relative to 'a' make 'b' an absolute rotation by: b = a * b.
    //
    // The interpolation parameter 't' is in the range [0,1]. When t = 0 the
    // resulting quaternion will be 'a'. When t = 1 the resulting quaternion
    // will be 'b'.
    //
    // The algorithm used is adapted from Allan and Mark Watt's "Advanced
    // Animation and Rendering Techniques" (ACM Press 1992).

    Quaternion result;
    float omega = 0.0f;
    float cosom = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    float sinom = 0.0f;
    float scale0 = 0.0f;
    float scale1 = 0.0f;

    if ((1.0f + cosom) > MathTools::Epsilon)
    {
        // 'a' and 'b' quaternions are not opposite each other.

        if ((1.0f - cosom) > MathTools::Epsilon)
        {
            // Standard case - slerp.
            omega = acosf(cosom);
            sinom = sinf(omega);
            scale0 = sinf((1.0f - t) * omega) / sinom;
            scale1 = sinf(t * omega) / sinom;
        }
        else
        {
            // 'a' and 'b' quaternions are very close so lerp instead.
            scale0 = 1.0f - t;
            scale1 = t;
        }

        result.x = scale0 * a.x + scale1 * b.x;
        result.y = scale0 * a.y + scale1 * b.y;
        result.z = scale0 * a.z + scale1 * b.z;
        result.w = scale0 * a.w + scale1 * b.w;
    }
    else
    {
        // 'a' and 'b' quaternions are opposite each other.

        result.x = -b.y;
        result.y = b.x;
        result.z = -b.w;
        result.w = b.z;

        scale0 = sinf((1.0f - t) - MathTools::PiNumberDivBy2Long);
        scale1 = sinf(t * MathTools::PiNumberDivBy2Long);

        result.x = scale0 * a.x + scale1 * result.x;
        result.y = scale0 * a.y + scale1 * result.y;
        result.z = scale0 * a.z + scale1 * result.z;
        result.w = scale0 * a.w + scale1 * result.w;
    }

    return result;
}

void Quaternion::fromMatrix(const Matrix3 &m)
{
    // Creates a quaternion from a rotation matrix.
    // The algorithm used is from Allan and Mark Watt's "Advanced
    // Animation and Rendering Techniques" (ACM Press 1992).

    float s = 0.0f;
    float q[4] = {0.0f};
    float trace = m[0][0] + m[1][1] + m[2][2];

    if (trace > 0.0f)
    {
        s = sqrtf(trace + 1.0f);
        q[3] = s * 0.5f;
        s = 0.5f / s;
        q[0] = (m[1][2] - m[2][1]) * s;
        q[1] = (m[2][0] - m[0][2]) * s;
        q[2] = (m[0][1] - m[1][0]) * s;
    }
    else
    {
        int nxt[3] = {1, 2, 0};
        int i = 0, j = 0, k = 0;

        if (m[1][1] > m[0][0])
            i = 1;

        if (m[2][2] > m[i][i])
            i = 2;

        j = nxt[i];
        k = nxt[j];
        s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

        q[i] = s * 0.5f;
        s = 0.5f / s;
        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;
    }

    x = q[0], y = q[1], z = q[2], w = q[3];
}

void Quaternion::fromMatrix(const Matrix4 &m)
{
    // Creates a quaternion from a rotation matrix.
    // The algorithm used is from Allan and Mark Watt's "Advanced
    // Animation and Rendering Techniques" (ACM Press 1992).

    float s = 0.0f;
    float q[4] = {0.0f};
    float trace = m[0][0] + m[1][1] + m[2][2];

    if (trace > 0.0f)
    {
        s = sqrtf(trace + 1.0f);
        q[3] = s * 0.5f;
        s = 0.5f / s;
        q[0] = (m[1][2] - m[2][1]) * s;
        q[1] = (m[2][0] - m[0][2]) * s;
        q[2] = (m[0][1] - m[1][0]) * s;
    }
    else
    {
        int nxt[3] = {1, 2, 0};
        int i = 0, j = 0, k = 0;

        if (m[1][1] > m[0][0])
            i = 1;

        if (m[2][2] > m[i][i])
            i = 2;

        j = nxt[i];
        k = nxt[j];
        s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

        q[i] = s * 0.5f;
        s = 0.5f / s;
        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;
    }

    x = q[0], y = q[1], z = q[2], w = q[3];
}

void Quaternion::toAxisAngle(Vector3 &axis, float &degrees) const
{
    // Converts this quaternion to an axis and an angle.

    float sinHalfThetaSq = 1.0f - w * w;

    // Guard against numerical imprecision and identity quaternions.
    if (sinHalfThetaSq <= 0.0f)
    {
        axis.x = 1.0f, axis.y = axis.z = 0.0f;
        degrees = 0.0f;
    }
    else
    {
        float invSinHalfTheta = 1.0f / sqrtf(sinHalfThetaSq);

        axis.x = x * invSinHalfTheta;
        axis.y = y * invSinHalfTheta;
        axis.z = z * invSinHalfTheta;
        degrees = MathTools::radiansToDegrees(2.0f * acosf(w));
    }
}

Matrix3 Quaternion::toMatrix3() const
{
    // Converts this quaternion to a rotation matrix.
    //
    //  | 1 - 2(y^2 + z^2)  2(xy + wz)          2(xz - wy)       |
    //  | 2(xy - wz)        1 - 2(x^2 + z^2)    2(yz + wx)       |
    //  | 2(xz + wy)        2(yz - wx)          1 - 2(x^2 + y^2) |

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    Matrix3 m;

    m[0][0] = 1.0f - (yy + zz);
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;

    m[1][0] = xy - wz;
    m[1][1] = 1.0f - (xx + zz);
    m[1][2] = yz + wx;

    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[2][2] = 1.0f - (xx + yy);

    return m;
}

Matrix4 Quaternion::toMatrix4() const
{
    // Converts this quaternion to a rotation matrix.
    //
    //  | 1 - 2(y^2 + z^2)  2(xy + wz)          2(xz - wy)          0  |
    //  | 2(xy - wz)        1 - 2(x^2 + z^2)    2(yz + wx)          0  |
    //  | 2(xz + wy)        2(yz - wx)          1 - 2(x^2 + y^2)    0  |
    //  | 0                 0                   0                   1  |

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    Matrix4 m;

    m[0][0] = 1.0f - (yy + zz);
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;
    m[0][3] = 0.0f;

    m[1][0] = xy - wz;
    m[1][1] = 1.0f - (xx + zz);
    m[1][2] = yz + wx;
    m[1][3] = 0.0f;

    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[2][2] = 1.0f - (xx + yy);
    m[2][3] = 0.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;

    return m;
}


}
