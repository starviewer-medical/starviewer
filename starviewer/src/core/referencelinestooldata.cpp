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
 : ToolData(parent)
{
}

ReferenceLinesToolData::~ReferenceLinesToolData()
{
}

QString ReferenceLinesToolData::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

QList<ImagePlane *> ReferenceLinesToolData::getPlanesToProject() const
{
    return m_planesToProject;
}

void ReferenceLinesToolData::setFrameOfReferenceUID( const QString &frameOfReference )
{
    m_frameOfReferenceUID = frameOfReference;
}

void ReferenceLinesToolData::setPlanesToProject( QList<ImagePlane *> planes )
{
    m_planesToProject.clear();
    m_planesToProject = planes;
    emit changed();
}

void ReferenceLinesToolData::setPlanesToProject( ImagePlane *plane )
{
    m_planesToProject.clear();
    if( plane )
        m_planesToProject << plane;
    emit changed();
}

}
