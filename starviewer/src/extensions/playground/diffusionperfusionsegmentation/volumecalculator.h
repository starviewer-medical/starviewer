/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMECALCULATOR_H
#define UDGVOLUMECALCULATOR_H

#include "volume.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VolumeCalculator{
public:
    VolumeCalculator();

    ~VolumeCalculator();

    void setInput( Volume * mask );

    void setInsideValue( int value );

    double getVolume();

    int getVoxels();

private:

    void calculateInsideValue();

    Volume * m_mask;
    int m_insideValue;
    bool m_insideValueSet;

};

}

#endif
