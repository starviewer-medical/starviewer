#include "obscurancethread.h"

#include <QColor>
#include <QLinkedList>
#include <QPair>
#include <QStack>

#include <vtkDirectionEncoder.h>
#include <vtkEncodedGradientEstimator.h>

#include "logging.h"
#include "mathtools.h"
#include "obscurance.h"

namespace udg {

ObscuranceThread::ObscuranceThread(int id, int numberOfThreads, const TransferFunction &transferFunction, QObject *parent)
    : QThread(parent),
      m_id(id), m_numberOfThreads(numberOfThreads),
      m_transferFunction(transferFunction),
      m_obscurance(0), m_saliency(0)
{
}

ObscuranceThread::~ObscuranceThread()
{
}

void ObscuranceThread::setGradientEstimator(vtkEncodedGradientEstimator *gradientEstimator)
{
    m_directionEncoder = gradientEstimator->GetDirectionEncoder();
    m_encodedNormals = gradientEstimator->GetEncodedNormals();
}

void ObscuranceThread::setData(const ushort *data, int dataSize, const int dimensions[3], const int increments[3])
{
    m_data = data;
    m_dataSize = dataSize;
    m_dimensions = dimensions;
    m_increments = increments;
}

void ObscuranceThread::setObscuranceParameters(double obscuranceMaximumDistance, Function obscuranceFunction, Variant obscuranceVariant, Obscurance *obscurance)
{
    m_obscuranceMaximumDistance = obscuranceMaximumDistance;
    m_obscuranceFunction = obscuranceFunction;
    m_obscuranceVariant = obscuranceVariant;
    m_obscurance = obscurance;
}

void ObscuranceThread::setSaliency(const double * saliency, double fxSaliencyA, double fxSaliencyB, double fxSaliencyLow, double fxSaliencyHigh)
{
    m_saliency = saliency;
    m_fxSaliencyA = fxSaliencyA;
    m_fxSaliencyB = fxSaliencyB;
    m_fxSaliencyLow = fxSaliencyLow;
    m_fxSaliencyHigh = fxSaliencyHigh;
}

void ObscuranceThread::setPerDirectionParameters(const Vector3 &direction, const Vector3 &forward, const int xyz[3], const int sXYZ[3],
                                                 const QVector<Vector3> &lineStarts, qptrdiff startDelta)
{
    m_direction = direction;
    m_forward = forward;
    m_xyz = xyz;
    m_sXYZ = sXYZ;
    m_lineStarts = lineStarts;
    m_startDelta = startDelta;
}

void ObscuranceThread::run()
{
    DEBUG_LOG(QString("%1: run()").arg(m_id));

    switch (m_obscuranceVariant)
    {
        case ObscuranceMainThread::Density:
            runDensity();
            break;
        case ObscuranceMainThread::DensitySmooth:
            runDensitySmooth();
            break;
        case ObscuranceMainThread::Opacity:
            runOpacity();
            break;
        case ObscuranceMainThread::OpacitySmooth:
            runOpacitySmooth();
            break;
        case ObscuranceMainThread::OpacitySaliency:
            runOpacitySaliency();
            break;
        case ObscuranceMainThread::OpacitySmoothSaliency:
            runOpacitySmoothSaliency();
            break;
        case ObscuranceMainThread::OpacityColorBleeding:
            runOpacityColorBleeding();
            break;
        case ObscuranceMainThread::OpacitySmoothColorBleeding:
            runOpacitySmoothColorBleeding();
            break;
    }
}

// Optimitzat
void ObscuranceThread::runDensity()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<ushort, Vector3> > unresolvedVoxels;
    // Amb això assegurem que tenim la capacitat necessària en el cas pitjor
    unresolvedVoxels.reserve(dimX);

    const ushort *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());
//         if (unresolvedVoxels.capacity() < dimX) std::cout << "perdem capacitat!!!!" << std::endl; // hi ha pèrdua de capacitat = COST!!!
                                                                                                    // provar alternatives al pop

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            ushort value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value)
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
                double cos = uNormal * m_direction;

                if (cos < 0.0)
                {
                    if (m_obscurance->isDoublePrecision())
                    {
                        m_obscurance->addObscurance(uIndex, -cos * obscurance((rv - ru).length()));
                    }
                }
            }

            unresolvedVoxels.push(qMakePair(value, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

void ObscuranceThread::runDensitySmooth()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<ushort, Vector3> > unresolvedVoxels;
    QLinkedList<QPair<ushort, Vector3> > postponedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());
        Q_ASSERT(postponedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            unsigned short value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];

            QLinkedList<QPair<ushort, Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList<QPair<ushort, Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while (itPostponedVoxels != itPostponedVoxelsEnd)
            {
                if (itPostponedVoxels->first <= value)
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                    Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                    double distance = (rv - ru).length();

                    if (distance <= 3.0)
                    {
                        // Tangent plane at u
                        // Normal en espai local (transformat)
                        Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // Distance from v to tangent plane at u
                        double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);

                        // Not blocking -> advance to the next
                        if (D <= 1.5)
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // Blocking
                    double cos = uNormal * m_direction;
                    if (cos < 0.0)
                    {
                        m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                    }

                    itPostponedVoxels = postponedVoxels.erase(itPostponedVoxels);
                }
                else
                {
                    ++itPostponedVoxels;
                }
            }

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value)
            {
                QPair<ushort, Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                double distance = (rv - ru).length();

                if (distance <= 3.0)
                {
                    // Tangent plane at u
                    // Normal en espai local (transformat)
                    Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // Distance from v to tangent plane at u
                    double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);
                    // Add u to postponed list
                    if (D <= 1.5)
                    {
                        postponedVoxels.append(uPair);
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if (cos < 0.0)
                {
                    m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                }
            }

            unresolvedVoxels.push(qMakePair(value, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!postponedVoxels.isEmpty())
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

void ObscuranceThread::runOpacity()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<double, Vector3> > unresolvedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            unsigned short value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity(value);

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
                double cos = uNormal * m_direction;

                if (cos < 0.0)
                {
                    double distance = (rv - ru).length();
                    m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

void ObscuranceThread::runOpacitySmooth()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<double, Vector3> > unresolvedVoxels;
    QLinkedList<QPair<double, Vector3> > postponedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());
        Q_ASSERT(postponedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            unsigned short value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity(value);

            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while (itPostponedVoxels != itPostponedVoxelsEnd)
            {
                if (itPostponedVoxels->first <= opacity)
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                    Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                    double distance = (rv - ru).length();

                    if (distance <= 3.0)
                    {
                        // Tangent plane at u
                        // Normal en espai local (transformat)
                        Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // Distance from v to tangent plane at u
                        double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);
                        // Not blocking -> advance to the next
                        if (D <= 1.5)
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // Blocking
                    double cos = uNormal * m_direction;
                    if (cos < 0.0)
                    {
                        m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                    }

                    itPostponedVoxels = postponedVoxels.erase(itPostponedVoxels);
                }
                else
                {
                    ++itPostponedVoxels;
                }
            }

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                QPair<double, Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                double distance = (rv - ru).length();

                if (distance <= 3.0)
                {
                    // Tangent plane at u
                    // Normal en espai local (transformat)
                    Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // Distance from v to tangent plane at u
                    double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);
                    // Add u to postponed list
                    if (D <= 1.5)
                    {
                        postponedVoxels.append(uPair);
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if (cos < 0.0)
                {
                    m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!postponedVoxels.isEmpty())
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

// = runOpacity() (de moment)
void ObscuranceThread::runOpacitySaliency()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<double, Vector3> > unresolvedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            unsigned short value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity(value);

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
                double cos = uNormal * m_direction;

                if (cos < 0.0)
                {
                    double distance = (rv - ru).length();
                    m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

void ObscuranceThread::runOpacitySmoothSaliency()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];
    double minFxSaliency = 1.0 - m_fxSaliencyA, maxFxSaliency = 1.0 + m_fxSaliencyB;

    QStack<QPair<double, Vector3> > unresolvedVoxels;
    QLinkedList<QPair<double, Vector3> > postponedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());
        Q_ASSERT(postponedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            // Index de v (sense el delta)
            int vIndex = v.x * incX + v.y * incY + v.z * incZ;
            unsigned short value = dataPtr[vIndex];
            double opacity = m_transferFunction.getOpacity(value);
            double fxSaliency = 1.0 + m_saliency[m_startDelta + vIndex] * (m_fxSaliencyA + m_fxSaliencyB) - m_fxSaliencyA;
            if (fxSaliency < m_fxSaliencyLow)
            {
                fxSaliency = minFxSaliency;
            }
            else if (fxSaliency > m_fxSaliencyHigh)
            {
                fxSaliency = maxFxSaliency;
            }
            opacity *= fxSaliency;

            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while (itPostponedVoxels != itPostponedVoxelsEnd)
            {
                if (itPostponedVoxels->first <= opacity)
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                    Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                    double distance = (rv - ru).length();

                    if (distance <= 3.0)
                    {
                        // Tangent plane at u
                        // Normal en espai local (transformat)
                        Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // Distance from v to tangent plane at u
                        double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);

                        // Not blocking -> advance to the next
                        if (D <= 1.5)
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // Blocking
                    double cos = uNormal * m_direction;
                    if (cos < 0.0)
                    {
                        m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                    }

                    itPostponedVoxels = postponedVoxels.erase(itPostponedVoxels);
                }
                else
                {
                    ++itPostponedVoxels;
                }
            }

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                QPair<double, Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                double distance = (rv - ru).length();

                if (distance <= 3.0)
                {
                    // Tangent plane at u
                    // Normal en espai local (transformat)
                    Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // Distance from v to tangent plane at u
                    double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);

                    // Add u to postponed list
                    if (D <= 1.5)
                    {
                        postponedVoxels.append(uPair);
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if (cos < 0.0)
                {
                    m_obscurance->addObscurance(uIndex, -cos * obscurance(distance));
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!postponedVoxels.isEmpty())
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addObscurance(uIndex, -cos);
            }
        }
    }
}

/// \todo encara és smooth
void ObscuranceThread::runOpacityColorBleeding()
{
    const Vector3 AMBIENT_COLOR(1.0, 1.0, 1.0);

    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<double, Vector3> > unresolvedVoxels;
    QLinkedList<QPair<double, Vector3> > postponedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };
        Q_ASSERT(unresolvedVoxels.isEmpty());
        Q_ASSERT(postponedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            unsigned short value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity(value);
            QColor vColor = m_transferFunction.getColor(value);
            Vector3 vColorVector(vColor.redF(), vColor.greenF(), vColor.blueF());

            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while (itPostponedVoxels != itPostponedVoxelsEnd)
            {
                if (itPostponedVoxels->first <= opacity)
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                    Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                    double distance = (rv - ru).length();

                    if (distance <= 3.0)
                    {
                        // Tangent plane at u
                        // Normal en espai local (transformat)
                        Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // Distance from v to Tangent plane at u
                        double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);
                        // Not blocking -> advance to the next
                        if (D <= 1.5)
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // Blocking
                    double cos = uNormal * m_direction;
                    if (cos < 0.0)
                    {
                        m_obscurance->addColorBleeding(uIndex, -cos * obscurance(distance) * vColorVector);
                    }

                    itPostponedVoxels = postponedVoxels.erase(itPostponedVoxels);
                }
                else
                {
                    ++itPostponedVoxels;
                }
            }

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                QPair<double, Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

                double distance = (rv - ru).length();

                if (distance <= 3.0)
                {
                    // Tangent plane at u
                    // Normal en espai local (transformat)
                    Vector3 uNormalLocal(sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z]);
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // Distance from v to Tangent plane at u
                    double D = qAbs(a * rv.x + b * rv.y + c * rv.z + d);
                    // Add u to postponed list
                    if (D <= 1.5)
                    {
                        postponedVoxels.append(uPair);
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if (cos < 0.0)
                {
                    m_obscurance->addColorBleeding(uIndex, -cos * obscurance(distance) * vColorVector);
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!postponedVoxels.isEmpty())
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addColorBleeding(uIndex, -cos * AMBIENT_COLOR);
            }
        }

        while (!unresolvedVoxels.isEmpty())
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);

            double cos = uNormal * m_direction;
            if (cos < 0.0)
            {
                m_obscurance->addColorBleeding(uIndex, -cos * AMBIENT_COLOR);
            }
        }
    }
}

void ObscuranceThread::runOpacitySmoothColorBleeding()
{
    const Vector3 AMBIENT_COLOR(1.0, 1.0, 1.0);

    const int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    const int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    const int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    const int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack<QPair<double, Vector3> > unresolvedVoxels;
    QLinkedList<QPair<double, Vector3> > postponedVoxels;

    const unsigned short *dataPtr = m_data + m_startDelta;
    const int nLineStarts = m_lineStarts.size();

    // u és el tapat, v és el que tapa
    // Iterar per cada línia
    for (int j = m_id; j < nLineStarts; j += m_numberOfThreads)
    {
        Vector3 rv = m_lineStarts.at(j);
        Voxel v = { qRound(rv.x), qRound(rv.y), qRound(rv.z) };

        Q_ASSERT(unresolvedVoxels.isEmpty());
        Q_ASSERT(postponedVoxels.isEmpty());

        // Iterar per la línia
        while (v.x < dimX && v.y < dimY && v.z < dimZ)
        {
            // Tractar el vòxel
            const double value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            const double opacity = m_transferFunction.getOpacity(value);
            const QColor vColor = m_transferFunction.getColor(value);
            const Vector3 vColorVector(vColor.redF(), vColor.greenF(), vColor.blueF());

            QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            const QLinkedList<QPair<double, Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while (itPostponedVoxels != itPostponedVoxelsEnd)
            {
                if (itPostponedVoxels->first <= opacity)
                {
                    const Vector3 ru = itPostponedVoxels->second;
                    const Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                    const int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    const float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                    const double distance = (rv - ru).length();
                    // ru és tapat -> calculem el color bleeding
                    if (smoothBlocking(rv, ru, distance, uGradient))
                    {
                        const Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
                        const double cos = uNormal * m_direction;

                        if (cos < 0.0)
                        {
                            m_obscurance->addColorBleeding(uIndex, -cos * obscurance(distance) * vColorVector);
                        }

                        itPostponedVoxels = postponedVoxels.erase(itPostponedVoxels);
                        // Evitem Avançar l'iterador
                        continue;
                    }
                }

                ++itPostponedVoxels;
            }

            while (!unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity)
            {
                const QPair<double, Vector3> uPair = unresolvedVoxels.pop();
                const Vector3 ru = uPair.second;
                const Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

                const int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                const float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
                const double distance = (rv - ru).length();
                // ru és tapat -> calculem el color bleeding
                if (smoothBlocking(rv, ru, distance, uGradient))
                {
                    const Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
                    const double cos = uNormal * m_direction;

                    if (cos < 0.0)
                    {
                        m_obscurance->addColorBleeding(uIndex, -cos * obscurance(distance) * vColorVector);
                    }
                }
                // ru no és tapat -> el posposem
                else
                {
                    postponedVoxels.append(uPair);
                }
            }

            unresolvedVoxels.push(qMakePair(opacity, rv));

            // Avançar el vòxel
            rv += m_forward;
            v.x = qRound(rv.x); v.y = qRound(rv.y); v.z = qRound(rv.z);
        }

        while (!postponedVoxels.isEmpty())
        {
            const Vector3 ru = postponedVoxels.takeFirst().second;
            const Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            const int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            const float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            const Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
            const double cos = uNormal * m_direction;

            if (cos < 0.0)
            {
                m_obscurance->addColorBleeding(uIndex, -cos * AMBIENT_COLOR);
            }
        }

        while (!unresolvedVoxels.isEmpty())
        {
            const Vector3 ru = unresolvedVoxels.pop().second;
            const Voxel u = { qRound(ru.x), qRound(ru.y), qRound(ru.z) };

            const int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            const float *uGradient = m_directionEncoder->GetDecodedGradient(m_encodedNormals[uIndex]);
            const Vector3 uNormal(uGradient[0], uGradient[1], uGradient[2]);
            const double cos = uNormal * m_direction;

            if (cos < 0.0)
            {
                m_obscurance->addColorBleeding(uIndex, -cos * AMBIENT_COLOR);
            }
        }
    }
}

inline double ObscuranceThread::obscurance(double distance) const
{
    const double EXP_NORM = 1.0 - exp(-1.0);

    if (distance > m_obscuranceMaximumDistance)
    {
        return 1.0;
    }

    switch (m_obscuranceFunction)
    {
        case ObscuranceMainThread::Constant0:
            return 0.0;
        case ObscuranceMainThread::Distance:
            return distance / m_obscuranceMaximumDistance;
        case ObscuranceMainThread::SquareRoot:
            return sqrt(distance / m_obscuranceMaximumDistance);
        case ObscuranceMainThread::Exponential:
            return 1.0 - exp(-distance / m_obscuranceMaximumDistance);
        case ObscuranceMainThread::ExponentialNorm:
            return (1.0 - exp(-distance / m_obscuranceMaximumDistance)) / EXP_NORM;
        case ObscuranceMainThread::CubeRoot:
            return MathTools::cubeRoot(distance / m_obscuranceMaximumDistance);
        default:
            Q_ASSERT(false);
            return 0.0;
    }
}

/// \bug Si posem l'inline surt un resultat incorrecte a vegades (amb gcc (GCC) 4.1.2 20061115 (prerelease) (SUSE Linux)).
///      Es podria provar també de deixar-ho inline i passar els paràmetres per valor, i deixar el més ràpid dels que funcionin.
/*inline*/ bool ObscuranceThread::smoothBlocking(const Vector3 &blocking, const Vector3 &blocked, double distance, const float *blockedGradient) const
{
    if (distance <= 3.0)
    {
        // Tangent plane at blocked
        const Vector3 blockedNormalLocal(m_sXYZ[0] * blockedGradient[m_xyz[0]],
                                          m_sXYZ[1] * blockedGradient[m_xyz[1]],
                                          // Normal en espai local (transformat)
                                          m_sXYZ[2] * blockedGradient[m_xyz[2]]);
        const double a = blockedNormalLocal.x, b = blockedNormalLocal.y, c = blockedNormalLocal.z, d = -blockedNormalLocal * blocked;
        // Distance from blocking to tangent plane at blocked
        if (qAbs(a * blocking.x + b * blocking.y + c * blocking.z + d) <= 1.5)
        {
            return false;
        }
    }

    return true;
}

}
