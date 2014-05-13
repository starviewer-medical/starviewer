/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "referencelinestooldata.h"
#include "imageplane.h"

namespace udg {

ReferenceLinesToolData::ReferenceLinesToolData(QObject *parent)
 : ToolData(parent)
{
}

ReferenceLinesToolData::~ReferenceLinesToolData()
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
}

QString ReferenceLinesToolData::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

QList<ImagePlane*> ReferenceLinesToolData::getPlanesToProject() const
{
    return m_planesToProject;
}

void ReferenceLinesToolData::setFrameOfReferenceUID(const QString &frameOfReference)
{
    m_frameOfReferenceUID = frameOfReference;
}

void ReferenceLinesToolData::setPlanesToProject(QList<ImagePlane*> planes)
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
    m_planesToProject.clear();
    m_planesToProject = planes;
    emit changed();
}

void ReferenceLinesToolData::setPlanesToProject(ImagePlane *plane)
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
    m_planesToProject.clear();
    if (plane)
    {
        m_planesToProject << plane;
    }
    emit changed();
}

}
