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


#include "matrix4.h"


namespace udg {


const Matrix4 Matrix4::Identity(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);

void Matrix4::fromHeadPitchRoll(float headDegrees, float pitchDegrees, float rollDegrees)
{
    // Constructs a rotation matrix based on a Euler Transform.
    // We use the popular NASA standard airplane convention of
    // heading-pitch-roll (i.e., RzRxRy).

    headDegrees = MathTools::degreesToRadians(headDegrees);
    pitchDegrees = MathTools::degreesToRadians(pitchDegrees);
    rollDegrees = MathTools::degreesToRadians(rollDegrees);

    float cosH = cosf(headDegrees);
    float cosP = cosf(pitchDegrees);
    float cosR = cosf(rollDegrees);
    float sinH = sinf(headDegrees);
    float sinP = sinf(pitchDegrees);
    float sinR = sinf(rollDegrees);

    mtx[0][0] = cosR * cosH - sinR * sinP * sinH;
    mtx[0][1] = sinR * cosH + cosR * sinP * sinH;
    mtx[0][2] = -cosP * sinH;
    mtx[0][3] = 0.0f;

    mtx[1][0] = -sinR * cosP;
    mtx[1][1] = cosR * cosP;
    mtx[1][2] = sinP;
    mtx[1][3] = 0.0f;

    mtx[2][0] = cosR * sinH + sinR * sinP * cosH;
    mtx[2][1] = sinR * sinH - cosR * sinP * cosH;
    mtx[2][2] = cosP * cosH;
    mtx[2][3] = 0.0f;

    mtx[3][0] = 0.0f;
    mtx[3][1] = 0.0f;
    mtx[3][2] = 0.0f;
    mtx[3][3] = 1.0f;
}

Matrix4 Matrix4::inverse() const
{
    // This method of computing the inverse of a 4x4 matrix is based
    // on a similar function found in Paul Nettle's matrix template
    // class (http://www.fluidstudios.com).
    //
    // If the inverse doesn't exist for this matrix, then the identity
    // matrix will be returned.

    Matrix4 tmp;
    float d = determinant();

    if (MathTools::closeEnough(d, 0.0f))
    {
        tmp.identity();
    }
    else
    {
        d = 1.0f / d;

        tmp.mtx[0][0] = d * (mtx[1][1] * (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3]) + mtx[2][1] * (mtx[3][2] * mtx[1][3] - mtx[1][2] * mtx[3][3]) + mtx[3][1] * (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3]));
        tmp.mtx[1][0] = d * (mtx[1][2] * (mtx[2][0] * mtx[3][3] - mtx[3][0] * mtx[2][3]) + mtx[2][2] * (mtx[3][0] * mtx[1][3] - mtx[1][0] * mtx[3][3]) + mtx[3][2] * (mtx[1][0] * mtx[2][3] - mtx[2][0] * mtx[1][3]));
        tmp.mtx[2][0] = d * (mtx[1][3] * (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1]) + mtx[2][3] * (mtx[3][0] * mtx[1][1] - mtx[1][0] * mtx[3][1]) + mtx[3][3] * (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]));
        tmp.mtx[3][0] = d * (mtx[1][0] * (mtx[3][1] * mtx[2][2] - mtx[2][1] * mtx[3][2]) + mtx[2][0] * (mtx[1][1] * mtx[3][2] - mtx[3][1] * mtx[1][2]) + mtx[3][0] * (mtx[2][1] * mtx[1][2] - mtx[1][1] * mtx[2][2]));

        tmp.mtx[0][1] = d * (mtx[2][1] * (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3]) + mtx[3][1] * (mtx[2][2] * mtx[0][3] - mtx[0][2] * mtx[2][3]) + mtx[0][1] * (mtx[3][2] * mtx[2][3] - mtx[2][2] * mtx[3][3]));
        tmp.mtx[1][1] = d * (mtx[2][2] * (mtx[0][0] * mtx[3][3] - mtx[3][0] * mtx[0][3]) + mtx[3][2] * (mtx[2][0] * mtx[0][3] - mtx[0][0] * mtx[2][3]) + mtx[0][2] * (mtx[3][0] * mtx[2][3] - mtx[2][0] * mtx[3][3]));
        tmp.mtx[2][1] = d * (mtx[2][3] * (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1]) + mtx[3][3] * (mtx[2][0] * mtx[0][1] - mtx[0][0] * mtx[2][1]) + mtx[0][3] * (mtx[3][0] * mtx[2][1] - mtx[2][0] * mtx[3][1]));
        tmp.mtx[3][1] = d * (mtx[2][0] * (mtx[3][1] * mtx[0][2] - mtx[0][1] * mtx[3][2]) + mtx[3][0] * (mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) + mtx[0][0] * (mtx[2][1] * mtx[3][2] - mtx[3][1] * mtx[2][2]));

        tmp.mtx[0][2] = d * (mtx[3][1] * (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]) + mtx[0][1] * (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3]) + mtx[1][1] * (mtx[3][2] * mtx[0][3] - mtx[0][2] * mtx[3][3]));
        tmp.mtx[1][2] = d * (mtx[3][2] * (mtx[0][0] * mtx[1][3] - mtx[1][0] * mtx[0][3]) + mtx[0][2] * (mtx[1][0] * mtx[3][3] - mtx[3][0] * mtx[1][3]) + mtx[1][2] * (mtx[3][0] * mtx[0][3] - mtx[0][0] * mtx[3][3]));
        tmp.mtx[2][2] = d * (mtx[3][3] * (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) + mtx[0][3] * (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1]) + mtx[1][3] * (mtx[3][0] * mtx[0][1] - mtx[0][0] * mtx[3][1]));
        tmp.mtx[3][2] = d * (mtx[3][0] * (mtx[1][1] * mtx[0][2] - mtx[0][1] * mtx[1][2]) + mtx[0][0] * (mtx[3][1] * mtx[1][2] - mtx[1][1] * mtx[3][2]) + mtx[1][0] * (mtx[0][1] * mtx[3][2] - mtx[3][1] * mtx[0][2]));

        tmp.mtx[0][3] = d * (mtx[0][1] * (mtx[2][2] * mtx[1][3] - mtx[1][2] * mtx[2][3]) + mtx[1][1] * (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3]) + mtx[2][1] * (mtx[1][2] * mtx[0][3] - mtx[0][2] * mtx[1][3]));
        tmp.mtx[1][3] = d * (mtx[0][2] * (mtx[2][0] * mtx[1][3] - mtx[1][0] * mtx[2][3]) + mtx[1][2] * (mtx[0][0] * mtx[2][3] - mtx[2][0] * mtx[0][3]) + mtx[2][2] * (mtx[1][0] * mtx[0][3] - mtx[0][0] * mtx[1][3]));
        tmp.mtx[2][3] = d * (mtx[0][3] * (mtx[2][0] * mtx[1][1] - mtx[1][0] * mtx[2][1]) + mtx[1][3] * (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) + mtx[2][3] * (mtx[1][0] * mtx[0][1] - mtx[0][0] * mtx[1][1]));
        tmp.mtx[3][3] = d * (mtx[0][0] * (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) + mtx[1][0] * (mtx[2][1] * mtx[0][2] - mtx[0][1] * mtx[2][2]) + mtx[2][0] * (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]));
    }

    return tmp;
}

void Matrix4::orient(const Vector3 &from, const Vector3 &to)
{
    // Creates an orientation matrix that will rotate the vector 'from'
    // into the vector 'to'. For this method to work correctly, vector
    // 'from' and vector 'to' must both be unit length vectors.
    //
    // The algorithm used is from:
    //   Tomas Moller and John F. Hughes, "Efficiently building a matrix
    //   to rotate one vector to another," Journal of Graphics Tools,
    //   4(4):1-4, 1999.

    float e = Vector3::dot(from, to);

    if (MathTools::closeEnough(e, 1.0f))
    {
        // Special case where 'from' is equal to 'to'. In other words,
        // the angle between vector 'from' and vector 'to' is zero
        // degrees. In this case just load the identity matrix.

        identity();
    }
    else if (MathTools::closeEnough(e, -1.0f))
    {
        // Special case where 'from' is directly opposite to 'to'. In
        // other words, the angle between vector 'from' and vector 'to'
        // is 180 degrees. In this case, the following matrix is used:
        //
        // Let:
        //   F = from
        //   S = vector perpendicular to F
        //   U = S X F
        //
        // We want to rotate from (F, U, S) to (-F, U, -S)
        //
        // | -FxFx+UxUx-SxSx  -FxFy+UxUy-SxSy  -FxFz+UxUz-SxSz  0 |
        // | -FxFy+UxUy-SxSy  -FyFy+UyUy-SySy  -FyFz+UyUz-SySz  0 |
        // | -FxFz+UxUz-SxSz  -FyFz+UyUz-SySz  -FzFz+UzUz-SzSz  0 |
        // |       0                 0                0         1 |

        Vector3 side(0.0f, from.z, -from.y);

        if (MathTools::closeEnough(Vector3::dot(side, side), 0.0f))
            side.set(-from.z, 0.0f, from.x);

        side.normalize();

        Vector3 up = Vector3::cross(side, from);
        up.normalize();

        mtx[0][0] = -(from.x * from.x) + (up.x * up.x) - (side.x * side.x);
        mtx[0][1] = -(from.x * from.y) + (up.x * up.y) - (side.x * side.y);
        mtx[0][2] = -(from.x * from.z) + (up.x * up.z) - (side.x * side.z);
        mtx[0][3] = 0.0f;
        mtx[1][0] = -(from.x * from.y) + (up.x * up.y) - (side.x * side.y);
        mtx[1][1] = -(from.y * from.y) + (up.y * up.y) - (side.y * side.y);
        mtx[1][2] = -(from.y * from.z) + (up.y * up.z) - (side.y * side.z);
        mtx[1][3] = 0.0f;
        mtx[2][0] = -(from.x * from.z) + (up.x * up.z) - (side.x * side.z);
        mtx[2][1] = -(from.y * from.z) + (up.y * up.z) - (side.y * side.z);
        mtx[2][2] = -(from.z * from.z) + (up.z * up.z) - (side.z * side.z);
        mtx[2][3] = 0.0f;
        mtx[3][0] = 0.0f;
        mtx[3][1] = 0.0f;
        mtx[3][2] = 0.0f;
        mtx[3][3] = 1.0f;
    }
    else
    {
        // This is the most common case. Creates the rotation matrix:
        //
        //               | E + HVx^2   HVxVy + Vz  HVxVz - Vy  0 |
        // R(from, to) = | HVxVy - Vz  E + HVy^2   HVxVz + Vx  0 |
        //               | HVxVz + Vy  HVyVz - Vx  E + HVz^2   0 |
        //               |     0           0           0       1 |
        //
        // where,
        //   V = from.cross(to)
        //   E = from.dot(to)
        //   H = (1 - E) / V.dot(V)

        Vector3 v = Vector3::cross(from, to);
        v.normalize();

        float h = (1.0f - e) / Vector3::dot(v, v);

        mtx[0][0] = e + h * v.x * v.x;
        mtx[0][1] = h * v.x * v.y + v.z;
        mtx[0][2] = h * v.x * v.z - v.y;
        mtx[0][3] = 0.0f;

        mtx[1][0] = h * v.x * v.y - v.z;
        mtx[1][1] = e + h * v.y * v.y;
        mtx[1][2] = h * v.x * v.z + v.x;
        mtx[1][3] = 0.0f;

        mtx[2][0] = h * v.x * v.z + v.y;
        mtx[2][1] = h * v.y * v.z - v.x;
        mtx[2][2] = e + h * v.z * v.z;
        mtx[2][3] = 0.0f;

        mtx[3][0] = 0.0f;
        mtx[3][1] = 0.0f;
        mtx[3][2] = 0.0f;
        mtx[3][3] = 1.0f;
    }
}

void Matrix4::rotate(const Vector3 &axis, float degrees)
{
    // Creates a rotation matrix about the specified axis.
    // The axis must be a unit vector. The angle must be in degrees.
    //
    // Let u = axis of rotation = (x, y, z)
    //
    //             | x^2(1 - c) + c  xy(1 - c) + zs  xz(1 - c) - ys   0 |
    // Ru(angle) = | yx(1 - c) - zs  y^2(1 - c) + c  yz(1 - c) + xs   0 |
    //             | zx(1 - c) - ys  zy(1 - c) - xs  z^2(1 - c) + c   0 |
    //             |      0              0                0           1 |
    //
    // where,
    //  c = cos(angle)
    //  s = sin(angle)

    degrees = MathTools::degreesToRadians(degrees);

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float c = cosf(degrees);
    float s = sinf(degrees);

    mtx[0][0] = (x * x) * (1.0f - c) + c;
    mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
    mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
    mtx[0][3] = 0.0f;

    mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
    mtx[1][1] = (y * y) * (1.0f - c) + c;
    mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
    mtx[1][3] = 0.0f;

    mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
    mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
    mtx[2][2] = (z * z) * (1.0f - c) + c;
    mtx[2][3] = 0.0f;

    mtx[3][0] = 0.0f;
    mtx[3][1] = 0.0f;
    mtx[3][2] = 0.0f;
    mtx[3][3] = 1.0f;
}

void Matrix4::scale(float sx, float sy, float sz)
{
    // Creates a scaling matrix.
    //
    //                 | sx   0    0    0 |
    // S(sx, sy, sz) = | 0    sy   0    0 |
    //                 | 0    0    sz   0 |
    //                 | 0    0    0    1 |

    mtx[0][0] = sx,   mtx[0][1] = 0.0f, mtx[0][2] = 0.0f, mtx[0][3] = 0.0f;
    mtx[1][0] = 0.0f, mtx[1][1] = sy,   mtx[1][2] = 0.0f, mtx[1][3] = 0.0f;
    mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = sz,   mtx[2][3] = 0.0f;
    mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

void Matrix4::toHeadPitchRoll(float &headDegrees, float &pitchDegrees, float &rollDegrees) const
{
    // Extracts the Euler angles from a rotation matrix. The returned
    // angles are in degrees. This method might suffer from numerical
    // imprecision for ill defined rotation matrices.
    //
    // This function only works for rotation matrices constructed using
    // the popular NASA standard airplane convention of heading-pitch-roll
    // (i.e., RzRxRy).
    //
    // The algorithm used is from:
    //  David Eberly, "Euler Angle Formulas", Geometric Tools web site,
    //  http://www.geometrictools.com/Documentation/EulerAngles.pdf.

    float thetaX = asinf(mtx[1][2]);
    float thetaY = 0.0f;
    float thetaZ = 0.0f;

    if (thetaX < MathTools::PiNumberDivBy2Long)
    {
        if (thetaX > -MathTools::PiNumberDivBy2Long)
        {
            thetaZ = atan2f(-mtx[1][0], mtx[1][1]);
            thetaY = atan2f(-mtx[0][2], mtx[2][2]);
        }
        else
        {
            // Not a unique solution.
            thetaZ = -atan2f(mtx[2][0], mtx[0][0]);
            thetaY = 0.0f;
        }
    }
    else
    {
        // Not a unique solution.
        thetaZ = atan2f(mtx[2][0], mtx[0][0]);
        thetaY = 0.0f;
    }

    headDegrees = MathTools::radiansToDegrees(thetaY);
    pitchDegrees = MathTools::radiansToDegrees(thetaX);
    rollDegrees = MathTools::radiansToDegrees(thetaZ);
}

void Matrix4::translate(float tx, float ty, float tz)
{
    // Creates a translation matrix.
    //
    //                 | 1    0    0    0 |
    // T(tx, ty, tz) = | 0    1    0    0 |
    //                 | 0    0    1    0 |
    //                 | tx   ty   tz   1 |

    mtx[0][0] = 1.0f, mtx[0][1] = 0.0f, mtx[0][2] = 0.0f, mtx[0][3] = 0.0f;
    mtx[1][0] = 0.0f, mtx[1][1] = 1.0f, mtx[1][2] = 0.0f, mtx[1][3] = 0.0f;
    mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = 1.0f, mtx[2][3] = 0.0f;
    mtx[3][0] = tx,   mtx[3][1] = ty,   mtx[3][2] = tz,   mtx[3][3] = 1.0f;
}


}
