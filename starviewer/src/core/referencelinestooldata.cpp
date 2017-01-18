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

#include <QSharedPointer>

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

const QList<QSharedPointer<ImagePlane>>& ReferenceLinesToolData::getPlanesToProject() const
{
    return m_planesToProject;
}

void ReferenceLinesToolData::setFrameOfReferenceUID(const QString &frameOfReference)
{
    m_frameOfReferenceUID = frameOfReference;
}

void ReferenceLinesToolData::setPlanesToProject(QList<QSharedPointer<ImagePlane>> planes)
{
    m_planesToProject = std::move(planes);
    emit changed();
}

void ReferenceLinesToolData::setPlanesToProject(QSharedPointer<ImagePlane> plane)
{
    m_planesToProject.clear();
    if (plane)
    {
        m_planesToProject << std::move(plane);
    }
    emit changed();
}

}
