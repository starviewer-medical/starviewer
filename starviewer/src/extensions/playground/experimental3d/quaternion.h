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


#ifndef UDGQUATERNION_H
#define UDGQUATERNION_H


#include <cmath>
#include <cstdlib>

#include "mathtools.h"
#include "matrix3.h"
#include "matrix4.h"
#include "vector3.h"


namespace udg {


// This Quaternion class will concatenate quaternions in a left to right order.
// The reason for this is to maintain the same multiplication semantics as the
// Matrix3 and Matrix4 classes.

class Quaternion
{
    friend Quaternion operator*(float lhs, const Quaternion &rhs);

public:
    static const Quaternion Identity;

    float w, x, y, z;

    static Quaternion slerp(const Quaternion &a, const Quaternion &b, float t);

    Quaternion() {}
    Quaternion(float w_, float x_, float y_, float z_);
    Quaternion(float headDegrees, float pitchDegrees, float rollDegrees);
    Quaternion(const Vector3 &axis, float degrees);
    explicit Quaternion(const Matrix3 &m);
    explicit Quaternion(const Matrix4 &m);
    ~Quaternion() {}

    bool operator==(const Quaternion &rhs) const;
    bool operator!=(const Quaternion &rhs) const;

    Quaternion &operator+=(const Quaternion &rhs);
    Quaternion &operator-=(const Quaternion &rhs);
    Quaternion &operator*=(const Quaternion &rhs);
    Quaternion &operator*=(float scalar);
    Quaternion &operator/=(float scalar);

    Quaternion operator+(const Quaternion &rhs) const;
    Quaternion operator-(const Quaternion &rhs) const;
    Quaternion operator*(const Quaternion &rhs) const;
    Quaternion operator*(float scalar) const;
    Quaternion operator/(float scalar) const;

    Quaternion conjugate() const;
    void fromAxisAngle(const Vector3 &axis, float degrees);
    void fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees);
    void fromMatrix(const Matrix3 &m);
    void fromMatrix(const Matrix4 &m);
    void identity();
    Quaternion inverse() const;
    float magnitude() const;
    void normalize();
    void set(float w_, float x_, float y_, float z_);
    void toAxisAngle(Vector3 &axis, float &degrees) const;
    void toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const;
    Matrix3 toMatrix3() const;
    Matrix4 toMatrix4() const;
};

inline Quaternion operator*(float lhs, const Quaternion &rhs)
{
    return rhs * lhs;
}

inline Quaternion::Quaternion(float w_, float x_, float y_, float z_)
    : w(w_), x(x_), y(y_), z(z_) {}

inline Quaternion::Quaternion(float headDegrees, float pitchDegrees, float rollDegrees)
{
    fromHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
}

inline Quaternion::Quaternion(const Vector3 &axis, float degrees)
{
    fromAxisAngle(axis, degrees);
}

inline Quaternion::Quaternion(const Matrix3 &m)
{
    fromMatrix(m);
}

inline Quaternion::Quaternion(const Matrix4 &m)
{
    fromMatrix(m);
}

inline bool Quaternion::operator==(const Quaternion &rhs) const
{
    return MathTools::closeEnough(w, rhs.w) && MathTools::closeEnough(x, rhs.x)
        && MathTools::closeEnough(y, rhs.y) && MathTools::closeEnough(z, rhs.z);
}

inline bool Quaternion::operator!=(const Quaternion &rhs) const
{
    return !(*this == rhs);
}

inline Quaternion &Quaternion::operator+=(const Quaternion &rhs)
{
    w += rhs.w, x += rhs.x, y += rhs.y, z += rhs.z;
    return *this;
}

inline Quaternion &Quaternion::operator-=(const Quaternion &rhs)
{
    w -= rhs.w, x -= rhs.x, y -= rhs.y, z -= rhs.z;
    return *this;
}

inline Quaternion &Quaternion::operator*=(const Quaternion &rhs)
{
    // Multiply so that rotations are applied in a left to right order.
    Quaternion tmp(
        (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z),
        (w * rhs.x) + (x * rhs.w) - (y * rhs.z) + (z * rhs.y),
        (w * rhs.y) + (x * rhs.z) + (y * rhs.w) - (z * rhs.x),
        (w * rhs.z) - (x * rhs.y) + (y * rhs.x) + (z * rhs.w));

    /*
    // Multiply so that rotations are applied in a right to left order.
    Quaternion tmp(
    (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z),
    (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y),
    (w * rhs.y) - (x * rhs.z) + (y * rhs.w) + (z * rhs.x),
    (w * rhs.z) + (x * rhs.y) - (y * rhs.x) + (z * rhs.w));
    */

    *this = tmp;
    return *this;
}

inline Quaternion &Quaternion::operator*=(float scalar)
{
    w *= scalar, x *= scalar, y *= scalar, z *= scalar;
    return *this;
}

inline Quaternion &Quaternion::operator/=(float scalar)
{
    w /= scalar, x /= scalar, y /= scalar, z /= scalar;
    return *this;
}

inline Quaternion Quaternion::operator+(const Quaternion &rhs) const
{
    Quaternion tmp(*this);
    tmp += rhs;
    return tmp;
}

inline Quaternion Quaternion::operator-(const Quaternion &rhs) const
{
    Quaternion tmp(*this);
    tmp -= rhs;
    return tmp;
}

inline Quaternion Quaternion::operator*(const Quaternion &rhs) const
{
    Quaternion tmp(*this);
    tmp *= rhs;
    return tmp;
}

inline Quaternion Quaternion::operator*(float scalar) const
{
    Quaternion tmp(*this);
    tmp *= scalar;
    return tmp;
}

inline Quaternion Quaternion::operator/(float scalar) const
{
    Quaternion tmp(*this);
    tmp /= scalar;
    return tmp;
}

inline Quaternion Quaternion::conjugate() const
{
    Quaternion tmp(w, -x, -y, -z);
    return tmp;
}

inline void Quaternion::fromAxisAngle(const Vector3 &axis, float degrees)
{
    float halfTheta = MathTools::degreesToRadians(degrees) * 0.5f;
    float s = sinf(halfTheta);
    w = cosf(halfTheta), x = axis.x * s, y = axis.y * s, z = axis.z * s;
}

inline void Quaternion::fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees)
{
    Matrix3 m;
    m.fromHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
    fromMatrix(m);
}

inline void Quaternion::identity()
{
    w = 1.0f, x = y = z = 0.0f;
}

inline Quaternion Quaternion::inverse() const
{
    float invMag = 1.0f / magnitude();
    return conjugate() * invMag;
}

inline float Quaternion::magnitude() const
{
    return sqrtf(w * w + x * x + y * y + z * z);
}

inline void Quaternion::normalize()
{
    float invMag = 1.0f / magnitude();
    w *= invMag, x *= invMag, y *= invMag, z *= invMag;
}

inline void Quaternion::set(float w_, float x_, float y_, float z_)
{
    w = w_, x = x_, y = y_, z = z_;
}

inline void Quaternion::toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const
{
    Matrix3 m = toMatrix3();
    m.toHeadPitchRoll(headDegrees, pitchDegrees, rollDegrees);
}


}


#endif
