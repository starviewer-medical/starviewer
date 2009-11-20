#ifndef CUDAFILTERING_H
#define CUDAFILTERING_H


#include <QVector>


class vtkImageData;


#ifdef __cplusplus
extern "C" {
#endif


QVector<float> cfGaussianDifference( vtkImageData *image, int radius );
QVector<float> cfBoxMeanDifference( vtkImageData *image, int radius );


#ifdef __cplusplus
}
#endif


#endif // CUDAFILTERING_H
