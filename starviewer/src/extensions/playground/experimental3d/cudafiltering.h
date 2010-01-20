#ifndef CUDAFILTERING_H
#define CUDAFILTERING_H


#include <QVector>


class vtkImageData;


#ifdef __cplusplus
extern "C" {
#endif


QVector<float> cfGaussianDifference( vtkImageData *image, int radius );
QVector<float> cfBoxMeanDifference( vtkImageData *image, int radius );

QVector<float> cfGaussianChebychev( vtkImageData *image, int radius );  // variant de Chebychev
QVector<float> cfBoxMeanChebychev( vtkImageData *image, int radius );   // Chebychev autèntica
QVector<float> cfGaussian( vtkImageData *image, int radius );   // estimació de P(Z >= z) amb una gaussiana en lloc de Chebychev


#ifdef __cplusplus
}
#endif


#endif // CUDAFILTERING_H
