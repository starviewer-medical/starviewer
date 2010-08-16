/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "filter.h"

namespace udg {

Filter::Filter()
{
	m_filterName = "";
}

Filter::~Filter()
{
	delete m_outputVolume;
	delete m_inputVolume;
}

void Filter::setInput(Volume * input)
{
	m_inputVolume = input;
}

Volume * Filter::getOutput()
{
	return m_outputVolume;
}

void Filter::setFilterName(QString name)
{
	m_filterName=name;
}

QString Filter::getFilterName()
{
	return m_filterName;

}
}