#ifndef CUDAFILTERING_H
#define CUDAFILTERING_H


#include <QVector>


class vtkImageData;


#ifdef __cplusplus
extern "C" {
#endif


QVector<float> cfGaussianDifference( vtkImageData *image, int radius );
QVector<float> cfBoxMeanDifference( vtkImageData *image, int radius );

// Mètodes on calculem l'ambient occlusion com la probabilitat que un vòxel tingui un valor més baix que els del seu entorn (P(Z >= z))
QVector<float> cfProbabilisticAmbientOcclusionGaussianChebychev( vtkImageData *image, int radius ); // estimació de P(Z >= z) amb Chebychev (variant)
QVector<float> cfProbabilisticAmbientOcclusionBoxMeanChebychev( vtkImageData *image, int radius );  // estimació de P(Z >= z) amb Chebychev (bona)
QVector<float> cfProbabilisticAmbientOcclusionGaussian( vtkImageData *image, int radius );  // estimació de P(Z >= z) amb una gaussiana
QVector<float> cfProbabilisticAmbientOcclusionCube( vtkImageData *image, int radius );  // estimació de P(Z >= z) amb la freqüència de Z >= z en un cub
QVector<float> cfProbabilisticAmbientOcclusionSphere( vtkImageData *image, int radius );    // estimació de P(Z >= z) amb la freqüència de Z >= z en una esfera
QVector<float> cfProbabilisticAmbientOcclusionTangentCube( vtkImageData *image, int radius );   // estimació de P(Z >= z) amb el mètode del cub tangent (László)
QVector<float> cfProbabilisticAmbientOcclusionVarianceTangentCube( vtkImageData *image, int radius );   // estimació de P(Z >= z) amb Chebychev i cub tangent (László)


#ifdef __cplusplus
}
#endif


#endif // CUDAFILTERING_H
