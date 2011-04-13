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
