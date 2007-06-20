/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qslideronestep.h"

#include <QMessageBox>
#include <QWheelEvent>


namespace udg {

QSliderOneStep::QSliderOneStep( QWidget *parent )
 : QSlider( parent )
{

}


QSliderOneStep::~QSliderOneStep()
{
}


void QSliderOneStep::wheelEvent ( QWheelEvent * event ){
	if (event->delta() > 0){
		this->setValue(this->value()+1);
	}
	else if (event->delta() < 0){
		this->setValue(this->value()-1);
	}
}

}
