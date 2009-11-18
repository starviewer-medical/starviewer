#ifndef CUDAFILTERING_H
#define CUDAFILTERING_H


#include <QVector>


class vtkImageData;


#ifdef __cplusplus
extern "C" {
#endif


QVector<float> cfGaussianDifference( vtkImageData *image, float radius );


#ifdef __cplusplus
}
#endif


#endif // CUDAFILTERING_H
