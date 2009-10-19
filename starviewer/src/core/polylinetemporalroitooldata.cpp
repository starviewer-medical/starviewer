/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylinetemporalroitooldata.h"
#include "logging.h"


namespace udg {

PolylineTemporalROIToolData::PolylineTemporalROIToolData(QObject *parent)
 : ToolData(parent)
{
	m_temporalImage = 0;
	m_temporalImageHasBeenDefined = false;
}


PolylineTemporalROIToolData::~PolylineTemporalROIToolData()
{
}

void PolylineTemporalROIToolData::setMeanVector( QVector<double> m )
{
	m_mean=m;
	emit dataChanged();
}

}
