/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "referencelinestooldata.h"
#include "imageplane.h"

namespace udg {

ReferenceLinesToolData::ReferenceLinesToolData(QObject *parent)
 : ToolData(parent), m_referenceImagePlane(0)
{
}

ReferenceLinesToolData::~ReferenceLinesToolData()
{
}

QString ReferenceLinesToolData::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

ImagePlane *ReferenceLinesToolData::getImagePlane() const
{
    return m_referenceImagePlane;
}

void ReferenceLinesToolData::setFrameOfReferenceUID( const QString &frameOfReference )
{
    m_frameOfReferenceUID = frameOfReference;
}

void ReferenceLinesToolData::setImagePlane( ImagePlane *imagePlane )
{
    m_referenceImagePlane = imagePlane;
    emit changed();
}

}
