/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef MUTUALINFORMATIONPARAMETERS_H
#define MUTUALINFORMATIONPARAMETERS_H

#include "parameters.h"
#include "identifier.h"

namespace udg{

/**
    Sub-classe de Parameters. En aquest cas encapsula els paràmetres d'un mètode de registre
    basat en mesures d'Informació mútua.
    
    Tindrem un mètode set/get per tots i cadascun dels paràmetres del mètode.
    Els mètodes set emetran el signal changed amb l'id de paràmetre que hem creat amb el tipus
    enumerat
*/

class MutualInformationParameters : public Parameters{
 Q_OBJECT
public:   
    /**
        Aquest tipus enumerat ens serveix per crear els identificadors de cada paràmetre.
    */
    enum MutualInformationParametersNames{ FixedImageID, MovingImageID, FixedImageStandardDeviation, MovingImageStandardDeviation, SpatialSamples, FixedImageVariance, MovingImageVariance, LearningRate, NumberOfIterations };
    
    MutualInformationParameters(QObject *parent = 0, const char *name = 0);
    ~MutualInformationParameters();

    void setFixedImageID( udg::Identifier id )
        {    m_fixedImageID = id ; emit changed( FixedImageID ); };
    
    void setMovingImageID( udg::Identifier id )
        {    m_movingImageID = id ; emit changed( MovingImageID ); };
    
    void setFixedImageStandardDeviation( double stdDev )
        {    m_fixedImageStandardDeviation = stdDev; emit changed( FixedImageStandardDeviation );  };
    
    void setMovingImageStandardDeviation( double stdDev )
        {    m_movingImageStandardDeviation = stdDev; emit changed( MovingImageStandardDeviation );  };
    
    void setSpatialSamples( int samples )
        {    m_spatialSamples = samples; emit changed( SpatialSamples ); };
    
    void setFixedImageVariance( double variance )
        {    m_fixedImageVariance = variance; emit changed( FixedImageVariance ); };
    
    void setMovingImageVariance( double variance )
        {    m_movingImageVariance = variance; emit changed( MovingImageVariance ); };
    
    void setLearningRate( double rate )
        {    m_learningRate = rate; emit changed( LearningRate ); };
    
    void setNumberOfIterations( int iterations )
        {    m_numberOfIterations = iterations; emit changed( NumberOfIterations ); };
    
    
    udg::Identifier getFixedImageID( ) const
        {    return m_fixedImageID; };
    
    udg::Identifier getMovingImageID(  ) const
        {    return m_movingImageID; };
    
    double getFixedImageStandardDeviation(  ) const
        {    return m_fixedImageStandardDeviation; };
    
    double getMovingImageStandardDeviation(  ) const
        {    return m_movingImageStandardDeviation; };
    
    int getSpatialSamples(  ) const
        {    return m_spatialSamples; };
    
    double getFixedImageVariance(  ) const
        {    return m_fixedImageVariance; };
    
    double getMovingImageVariance(  ) const
        {    return m_movingImageVariance; };
    
    double getLearningRate(  ) const
        {    return m_learningRate; };
    
    int getNumberOfIterations( ) const
        {    return m_numberOfIterations; };
        
private:
    udg::Identifier m_fixedImageID, m_movingImageID; // l'id del repositori
    
    // mètrica
    double m_fixedImageStandardDeviation, m_movingImageStandardDeviation;
    int m_spatialSamples;
    
    // filtre gaussià
    double m_fixedImageVariance, m_movingImageVariance;
    
    double m_learningRate;
    int m_numberOfIterations;
};

}; // end namespace udg

#endif
