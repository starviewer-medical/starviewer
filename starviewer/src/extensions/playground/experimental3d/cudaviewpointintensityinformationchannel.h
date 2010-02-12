#ifndef CUDAVIEWPOINTINTENSITYINFORMATIONCHANNEL_H
#define CUDAVIEWPOINTINTENSITYINFORMATIONCHANNEL_H


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


void cviicSetupRayCast( vtkImageData *image, const TransferFunction &transferFunction, int renderSize, int displaySize, QColor backgroundColor, bool display );
QVector<float> cviicRayCastAndGetHistogram( Vector3 viewpoint, Matrix4 viewMatrix );
void cviicCleanupRayCast();

void cviicSetupIntensityProbabilities();
void cviicAccumulateIntensityProbabilities( float viewProbability, float totalViewedVolume );
QVector<float> cviicGetIntensityProbabilities();
void cviicCleanupIntensityProbabilities();

void cviicSetupImi(/* bool vomi, bool colorVomi */);
void cviicAccumulateImi( float viewProbability, float totalViewedVolume );
//void cvicAccumulateColorVomi( float viewProbability, const Vector3Float &viewColor, float totalViewedVolume );
QVector<float> cviicGetImi();
//QVector<Vector3Float> cvicGetColorVomi();
void cviicCleanupImi();


#ifdef __cplusplus
}
#endif


#endif // CUDAVIEWPOINTINTENSITYINFORMATIONCHANNEL_H
