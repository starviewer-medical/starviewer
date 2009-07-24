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


#ifndef UDGMATRIX4_H
#define UDGMATRIX4_H


#include "mathtools.h"
#include "vector3.h"


namespace udg {


// A homogeneous row-major 4x4 matrix class.
//
// Matrices are concatenated in a left to right order.
// Multiplies vectors to the left of the matrix.

class Matrix4
{
    friend Vector3 operator*(const Vector3 &lhs, const Matrix4 &rhs);
    friend Matrix4 operator*(float scalar, const Matrix4 &rhs);

public:
    static const Matrix4 Identity;

    Matrix4() {}
    Matrix4(float m11, float m12, float m13, float m14,
            float m21, float m22, float m23, float m24,
            float m31, float m32, float m33, float m34,
            float m41, float m42, float m43, float m44);
    ~Matrix4() {}

    float *operator[](int row);
    const float *operator[](int row) const;

    bool operator==(const Matrix4 &rhs) const;
    bool operator!=(const Matrix4 &rhs) const;

    Matrix4 &operator+=(const Matrix4 &rhs);
    Matrix4 &operator-=(const Matrix4 &rhs);
    Matrix4 &operator*=(const Matrix4 &rhs);
    Matrix4 &operator*=(float scalar);
    Matrix4 &operator/=(float scalar);

    Matrix4 operator+(const Matrix4 &rhs) const;
    Matrix4 operator-(const Matrix4 &rhs) const;
    Matrix4 operator*(const Matrix4 &rhs) const;
    Matrix4 operator*(float scalar) const;
    Matrix4 operator/(float scalar) const;

    float determinant() const;
    void fromAxes(const Vector3 &x, const Vector3 &y, const Vector3 &z);
    void fromAxesTransposed(const Vector3 &x, const Vector3 &y, const Vector3 &z);
    void fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees);
    void identity();
    Matrix4 inverse() const;
    void orient(const Vector3 &from, const Vector3 &to);
    void rotate(const Vector3 &axis, float degrees);
    void scale(float sx, float sy, float sz);
    void toAxes(Vector3 &x, Vector3 &y, Vector3 &z) const;
    void toAxesTransposed(Vector3 &x, Vector3 &y, Vector3 &z) const;
    void toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const;
    void translate(float tx, float ty, float tz);
    Matrix4 transpose() const;

private:
    float mtx[4][4];
};

inline Vector3 operator*(const Vector3 &lhs, const Matrix4 &rhs)
{
    return Vector3((lhs.x * rhs.mtx[0][0]) + (lhs.y * rhs.mtx[1][0]) + (lhs.z * rhs.mtx[2][0]),
        (lhs.x * rhs.mtx[0][1]) + (lhs.y * rhs.mtx[1][1]) + (lhs.z * rhs.mtx[2][1]),
        (lhs.x * rhs.mtx[0][2]) + (lhs.y * rhs.mtx[1][2]) + (lhs.z * rhs.mtx[2][2]));
}

inline Matrix4 operator*(float scalar, const Matrix4 &rhs)
{
    return rhs * scalar;
}

inline Matrix4::Matrix4(float m11, float m12, float m13, float m14,
                      float m21, float m22, float m23, float m24,
                      float m31, float m32, float m33, float m34,
                      float m41, float m42, float m43, float m44)
{
    mtx[0][0] = m11, mtx[0][1] = m12, mtx[0][2] = m13, mtx[0][3] = m14;
    mtx[1][0] = m21, mtx[1][1] = m22, mtx[1][2] = m23, mtx[1][3] = m24;
    mtx[2][0] = m31, mtx[2][1] = m32, mtx[2][2] = m33, mtx[2][3] = m34;
    mtx[3][0] = m41, mtx[3][1] = m42, mtx[3][2] = m43, mtx[3][3] = m44;
}

inline float *Matrix4::operator[](int row)
{
    return mtx[row];
}

inline const float *Matrix4::operator[](int row) const
{
    return mtx[row];
}

inline bool Matrix4::operator==(const Matrix4 &rhs) const
{
    return MathTools::closeEnough(mtx[0][0], rhs.mtx[0][0])
        && MathTools::closeEnough(mtx[0][1], rhs.mtx[0][1])
        && MathTools::closeEnough(mtx[0][2], rhs.mtx[0][2])
        && MathTools::closeEnough(mtx[0][3], rhs.mtx[0][3])
        && MathTools::closeEnough(mtx[1][0], rhs.mtx[1][0])
        && MathTools::closeEnough(mtx[1][1], rhs.mtx[1][1])
        && MathTools::closeEnough(mtx[1][2], rhs.mtx[1][2])
        && MathTools::closeEnough(mtx[1][3], rhs.mtx[1][3])
        && MathTools::closeEnough(mtx[2][0], rhs.mtx[2][0])
        && MathTools::closeEnough(mtx[2][1], rhs.mtx[2][1])
        && MathTools::closeEnough(mtx[2][2], rhs.mtx[2][2])
        && MathTools::closeEnough(mtx[2][3], rhs.mtx[2][3])
        && MathTools::closeEnough(mtx[3][0], rhs.mtx[3][0])
        && MathTools::closeEnough(mtx[3][1], rhs.mtx[3][1])
        && MathTools::closeEnough(mtx[3][2], rhs.mtx[3][2])
        && MathTools::closeEnough(mtx[3][3], rhs.mtx[3][3]);
}

inline bool Matrix4::operator!=(const Matrix4 &rhs) const
{
    return !(*this == rhs);
}

inline Matrix4 &Matrix4::operator+=(const Matrix4 &rhs)
{
    mtx[0][0] += rhs.mtx[0][0], mtx[0][1] += rhs.mtx[0][1], mtx[0][2] += rhs.mtx[0][2], mtx[0][3] += rhs.mtx[0][3];
    mtx[1][0] += rhs.mtx[1][0], mtx[1][1] += rhs.mtx[1][1], mtx[1][2] += rhs.mtx[1][2], mtx[1][3] += rhs.mtx[1][3];
    mtx[2][0] += rhs.mtx[2][0], mtx[2][1] += rhs.mtx[2][1], mtx[2][2] += rhs.mtx[2][2], mtx[2][3] += rhs.mtx[2][3];
    mtx[3][0] += rhs.mtx[3][0], mtx[3][1] += rhs.mtx[3][1], mtx[3][2] += rhs.mtx[3][2], mtx[3][3] += rhs.mtx[3][3];
    return *this;
}

inline Matrix4 &Matrix4::operator-=(const Matrix4 &rhs)
{
    mtx[0][0] -= rhs.mtx[0][0], mtx[0][1] -= rhs.mtx[0][1], mtx[0][2] -= rhs.mtx[0][2], mtx[0][3] -= rhs.mtx[0][3];
    mtx[1][0] -= rhs.mtx[1][0], mtx[1][1] -= rhs.mtx[1][1], mtx[1][2] -= rhs.mtx[1][2], mtx[1][3] -= rhs.mtx[1][3];
    mtx[2][0] -= rhs.mtx[2][0], mtx[2][1] -= rhs.mtx[2][1], mtx[2][2] -= rhs.mtx[2][2], mtx[2][3] -= rhs.mtx[2][3];
    mtx[3][0] -= rhs.mtx[3][0], mtx[3][1] -= rhs.mtx[3][1], mtx[3][2] -= rhs.mtx[3][2], mtx[3][3] -= rhs.mtx[3][3];
    return *this;
}

inline Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    Matrix4 tmp;

    // Row 1.
    tmp.mtx[0][0] = (mtx[0][0] * rhs.mtx[0][0]) + (mtx[0][1] * rhs.mtx[1][0]) + (mtx[0][2] * rhs.mtx[2][0]) + (mtx[0][3] * rhs.mtx[3][0]);
    tmp.mtx[0][1] = (mtx[0][0] * rhs.mtx[0][1]) + (mtx[0][1] * rhs.mtx[1][1]) + (mtx[0][2] * rhs.mtx[2][1]) + (mtx[0][3] * rhs.mtx[3][1]);
    tmp.mtx[0][2] = (mtx[0][0] * rhs.mtx[0][2]) + (mtx[0][1] * rhs.mtx[1][2]) + (mtx[0][2] * rhs.mtx[2][2]) + (mtx[0][3] * rhs.mtx[3][2]);
    tmp.mtx[0][3] = (mtx[0][0] * rhs.mtx[0][3]) + (mtx[0][1] * rhs.mtx[1][3]) + (mtx[0][2] * rhs.mtx[2][3]) + (mtx[0][3] * rhs.mtx[3][3]);

    // Row 2.
    tmp.mtx[1][0] = (mtx[1][0] * rhs.mtx[0][0]) + (mtx[1][1] * rhs.mtx[1][0]) + (mtx[1][2] * rhs.mtx[2][0]) + (mtx[1][3] * rhs.mtx[3][0]);
    tmp.mtx[1][1] = (mtx[1][0] * rhs.mtx[0][1]) + (mtx[1][1] * rhs.mtx[1][1]) + (mtx[1][2] * rhs.mtx[2][1]) + (mtx[1][3] * rhs.mtx[3][1]);
    tmp.mtx[1][2] = (mtx[1][0] * rhs.mtx[0][2]) + (mtx[1][1] * rhs.mtx[1][2]) + (mtx[1][2] * rhs.mtx[2][2]) + (mtx[1][3] * rhs.mtx[3][2]);
    tmp.mtx[1][3] = (mtx[1][0] * rhs.mtx[0][3]) + (mtx[1][1] * rhs.mtx[1][3]) + (mtx[1][2] * rhs.mtx[2][3]) + (mtx[1][3] * rhs.mtx[3][3]);

    // Row 3.
    tmp.mtx[2][0] = (mtx[2][0] * rhs.mtx[0][0]) + (mtx[2][1] * rhs.mtx[1][0]) + (mtx[2][2] * rhs.mtx[2][0]) + (mtx[2][3] * rhs.mtx[3][0]);
    tmp.mtx[2][1] = (mtx[2][0] * rhs.mtx[0][1]) + (mtx[2][1] * rhs.mtx[1][1]) + (mtx[2][2] * rhs.mtx[2][1]) + (mtx[2][3] * rhs.mtx[3][1]);
    tmp.mtx[2][2] = (mtx[2][0] * rhs.mtx[0][2]) + (mtx[2][1] * rhs.mtx[1][2]) + (mtx[2][2] * rhs.mtx[2][2]) + (mtx[2][3] * rhs.mtx[3][2]);
    tmp.mtx[2][3] = (mtx[2][0] * rhs.mtx[0][3]) + (mtx[2][1] * rhs.mtx[1][3]) + (mtx[2][2] * rhs.mtx[2][3]) + (mtx[2][3] * rhs.mtx[3][3]);

    // Row 4.
    tmp.mtx[3][0] = (mtx[3][0] * rhs.mtx[0][0]) + (mtx[3][1] * rhs.mtx[1][0]) + (mtx[3][2] * rhs.mtx[2][0]) + (mtx[3][3] * rhs.mtx[3][0]);
    tmp.mtx[3][1] = (mtx[3][0] * rhs.mtx[0][1]) + (mtx[3][1] * rhs.mtx[1][1]) + (mtx[3][2] * rhs.mtx[2][1]) + (mtx[3][3] * rhs.mtx[3][1]);
    tmp.mtx[3][2] = (mtx[3][0] * rhs.mtx[0][2]) + (mtx[3][1] * rhs.mtx[1][2]) + (mtx[3][2] * rhs.mtx[2][2]) + (mtx[3][3] * rhs.mtx[3][2]);
    tmp.mtx[3][3] = (mtx[3][0] * rhs.mtx[0][3]) + (mtx[3][1] * rhs.mtx[1][3]) + (mtx[3][2] * rhs.mtx[2][3]) + (mtx[3][3] * rhs.mtx[3][3]);

    *this = tmp;
    return *this;
}

inline Matrix4 &Matrix4::operator*=(float scalar)
{
    mtx[0][0] *= scalar, mtx[0][1] *= scalar, mtx[0][2] *= scalar, mtx[0][3] *= scalar;
    mtx[1][0] *= scalar, mtx[1][1] *= scalar, mtx[1][2] *= scalar, mtx[1][3] *= scalar;
    mtx[2][0] *= scalar, mtx[2][1] *= scalar, mtx[2][2] *= scalar, mtx[2][3] *= scalar;
    mtx[3][0] *= scalar, mtx[3][1] *= scalar, mtx[3][2] *= scalar, mtx[3][3] *= scalar;
    return *this;
}

inline Matrix4 &Matrix4::operator/=(float scalar)
{
    mtx[0][0] /= scalar, mtx[0][1] /= scalar, mtx[0][2] /= scalar, mtx[0][3] /= scalar;
    mtx[1][0] /= scalar, mtx[1][1] /= scalar, mtx[1][2] /= scalar, mtx[1][3] /= scalar;
    mtx[2][0] /= scalar, mtx[2][1] /= scalar, mtx[2][2] /= scalar, mtx[2][3] /= scalar;
    mtx[3][0] /= scalar, mtx[3][1] /= scalar, mtx[3][2] /= scalar, mtx[3][3] /= scalar;
    return *this;
}

inline Matrix4 Matrix4::operator+(const Matrix4 &rhs) const
{
    Matrix4 tmp(*this);
    tmp += rhs;
    return tmp;
}

inline Matrix4 Matrix4::operator-(const Matrix4 &rhs) const
{
    Matrix4 tmp(*this);
    tmp -= rhs;
    return tmp;
}

inline Matrix4 Matrix4::operator*(const Matrix4 &rhs) const
{
    Matrix4 tmp(*this);
    tmp *= rhs;
    return tmp;
}

inline Matrix4 Matrix4::operator*(float scalar) const
{
    Matrix4 tmp(*this);
    tmp *= scalar;
    return tmp;
}

inline Matrix4 Matrix4::operator/(float scalar) const
{
    Matrix4 tmp(*this);
    tmp /= scalar;
    return tmp;
}

inline float Matrix4::determinant() const
{
    return (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1])
        * (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3])
        - (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1])
        * (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3])
        + (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1])
        * (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3])
        + (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1])
        * (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3])
        - (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1])
        * (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3])
        + (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1])
        * (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]);
}

inline void Matrix4::fromAxes(const Vector3 &x, const Vector3 &y, const Vector3 &z)
{
    mtx[0][0] = x.x,  mtx[0][1] = x.y,  mtx[0][2] = x.z,  mtx[0][3] = 0.0f;
    mtx[1][0] = y.x,  mtx[1][1] = y.y,  mtx[1][2] = y.z,  mtx[1][3] = 0.0f;
    mtx[2][0] = z.x,  mtx[2][1] = z.y,  mtx[2][2] = z.z,  mtx[2][3] = 0.0f;
    mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

inline void Matrix4::fromAxesTransposed(const Vector3 &x, const Vector3 &y, const Vector3 &z)
{
    mtx[0][0] = x.x,  mtx[0][1] = y.x,  mtx[0][2] = z.x,  mtx[0][3] = 0.0f;
    mtx[1][0] = x.y,  mtx[1][1] = y.y,  mtx[1][2] = z.y,  mtx[1][3] = 0.0f;
    mtx[2][0] = x.z,  mtx[2][1] = y.z,  mtx[2][2] = z.z,  mtx[2][3] = 0.0f;
    mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

inline void Matrix4::identity()
{
    mtx[0][0] = 1.0f, mtx[0][1] = 0.0f, mtx[0][2] = 0.0f, mtx[0][3] = 0.0f;
    mtx[1][0] = 0.0f, mtx[1][1] = 1.0f, mtx[1][2] = 0.0f, mtx[1][3] = 0.0f;
    mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = 1.0f, mtx[2][3] = 0.0f;
    mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

inline void Matrix4::toAxes(Vector3 &x, Vector3 &y, Vector3 &z) const
{
    x.set(mtx[0][0], mtx[0][1], mtx[0][2]);
    y.set(mtx[1][0], mtx[1][1], mtx[1][2]);
    z.set(mtx[2][0], mtx[2][1], mtx[2][2]);
}

inline void Matrix4::toAxesTransposed(Vector3 &x, Vector3 &y, Vector3 &z) const
{
    x.set(mtx[0][0], mtx[1][0], mtx[2][0]);
    y.set(mtx[0][1], mtx[1][1], mtx[2][1]);
    z.set(mtx[0][2], mtx[1][2], mtx[2][2]);
}

inline Matrix4 Matrix4::transpose() const
{
    Matrix4 tmp;

    tmp[0][0] = mtx[0][0], tmp[0][1] = mtx[1][0], tmp[0][2] = mtx[2][0], tmp[0][3] = mtx[3][0];
    tmp[1][0] = mtx[0][1], tmp[1][1] = mtx[1][1], tmp[1][2] = mtx[2][1], tmp[1][3] = mtx[3][1];
    tmp[2][0] = mtx[0][2], tmp[2][1] = mtx[1][2], tmp[2][2] = mtx[2][2], tmp[2][3] = mtx[3][2];
    tmp[3][0] = mtx[0][3], tmp[3][1] = mtx[1][3], tmp[3][2] = mtx[2][3], tmp[3][3] = mtx[3][3];

    return tmp;
}


}


#endif
