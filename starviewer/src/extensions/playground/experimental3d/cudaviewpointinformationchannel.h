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

void ce3dSetupVomi();
void ce3dAccumulateVomi( float viewProbability, float totalViewedVolume );
QVector<float> ce3dCleanupVomi();


#ifdef __cplusplus
}
#endif


#endif // CUDAVIEWPOINTINFORMATIONCHANNEL_H
