/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVTKTHRESHOLDFILTER_H
#define UDGVTKTHRESHOLDFILTER_H

#include "filter.h"

class vtkImageData;
class vtkImageThreshold;

namespace udg {

// Fordward declarations
class Volume;

class VtkThresholdFilter : public Filter{

public:
	VtkThresholdFilter();
	~VtkThresholdFilter();

	void thresholdByUpper (float thresh);
	void thresholdByLower (float thresh);
	void thresholdBetween (float lower, float upper);
    void setInValue (double val);
    void setOutValue (double val);
    void update();
    void setInput(Volume * input);
    //Volume * getOutput();


protected:
	vtkImageThreshold * imageThreshold;
};

}
#endif