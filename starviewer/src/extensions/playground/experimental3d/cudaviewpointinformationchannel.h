#ifndef CUDAVIEWPOINTINFORMATIONCHANNEL_H
#define CUDAVIEWPOINTINFORMATIONCHANNEL_H


#include <QVector>

#include "vector3.h"


class vtkImageData;


namespace udg {
class Matrix4;
class TransferFunction;
}


using namespace udg;


#ifdef __cplusplus
extern "C" {
#endif


void cvicSetupRayCast( vtkImageData *image, const TransferFunction &transferFunction, int renderSize, int displaySize, QColor backgroundColor, bool display );
QVector<float> cvicRayCastAndGetHistogram( Vector3 viewpoint, Matrix4 viewMatrix );
void cvicCleanupRayCast();

void cvicSetupVoxelProbabilities();
void cvicAccumulateVoxelProbabilities( float viewProbability, float totalViewedVolume );
QVector<float> cvicGetVoxelProbabilities();
void cvicCleanupVoxelProbabilities();

void cvicSetupVomi( bool vomi, bool colorVomi );
void cvicAccumulateVomi( float viewProbability, float totalViewedVolume );
void cvicAccumulateColorVomi( float viewProbability, const Vector3Float &viewColor, float totalViewedVolume );
QVector<float> cvicGetVomi();
QVector<Vector3Float> cvicGetColorVomi();
void cvicCleanupVomi();


#ifdef __cplusplus
}
#endif


#endif // CUDAVIEWPOINTINFORMATIONCHANNEL_H
