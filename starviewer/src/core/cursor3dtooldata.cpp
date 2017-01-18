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

#include "cursor3dtooldata.h"
#include "imageplane.h"

namespace udg {

Cursor3DToolData::Cursor3DToolData(QObject *parent)
 : ToolData(parent)
{
    m_originPointPosition = new double[3];
}

Cursor3DToolData::~Cursor3DToolData()
{
    delete[] m_originPointPosition;
}

QString Cursor3DToolData::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

QSharedPointer<ImagePlane> Cursor3DToolData::getImagePlane() const
{
    return m_referenceImagePlane;
}

double* Cursor3DToolData::getOriginPointPosition() const
{
    return m_originPointPosition;
}

void Cursor3DToolData::setFrameOfReferenceUID(const QString &frameOfReference)
{
    m_frameOfReferenceUID = frameOfReference;
}

void Cursor3DToolData::setImagePlane(QSharedPointer<ImagePlane> imagePlane)
{
    m_referenceImagePlane = std::move(imagePlane);
    emit changed();
}

void Cursor3DToolData::setOriginPointPosition(double position[3])
{
    for (int i = 0; i < 3; i++)
    {
        m_originPointPosition[i] = position[i];
    }
    emit changed();
}

QString Cursor3DToolData::getInstanceUID() const
{
    return m_instanceUID;
}

void Cursor3DToolData::setInstanceUID(const QString &instanceUID)
{
    m_instanceUID = instanceUID;
}

void Cursor3DToolData::hideCursors()
{
    emit turnOffCursor();
}

}
