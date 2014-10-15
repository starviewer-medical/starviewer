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

#include "modalitysynccriterion.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "volume.h"

#include <QSet>

namespace udg {

ModalitySyncCriterion::ModalitySyncCriterion()
 : SyncCriterion()
{
}

ModalitySyncCriterion::~ModalitySyncCriterion()
{
}

bool ModalitySyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    
    if (!viewer1->hasInput() || !viewer2->hasInput())
    {
        return false;
    }

    Q2DViewer *viewer2D1 = Q2DViewer::castFromQViewer(viewer1);
    Q2DViewer *viewer2D2 = Q2DViewer::castFromQViewer(viewer2);

    if (viewer2D1 && viewer2D2)
    {
        QStringList modalities1 = getModalities(viewer2D1);
        QStringList modalities2 = getModalities(viewer2D2);
        
        QSet<QString> coincidentModalities = modalities1.toSet().intersect(modalities2.toSet());

        return !coincidentModalities.isEmpty() || isFusionCase(modalities1, modalities2);
    }
    else
    {
        return viewer1->getMainInput()->getModality() == viewer2->getMainInput()->getModality();
    }
}

QStringList ModalitySyncCriterion::getModalities(Q2DViewer *viewer) const
{
    Q_ASSERT(viewer);

    QList<Volume*> inputs = viewer->getInputs();
    QStringList modalities;
    foreach (Volume *volume, inputs)
    {
        modalities << volume->getModality();
    }

    return modalities;
}

bool ModalitySyncCriterion::isFusionCase(const QStringList &modalities1, const QStringList &modalities2) const
{
    return (modalities1.contains("CT") && modalities2.contains("PT")) || (modalities1.contains("PT") && modalities2.contains("CT")) ||
            (modalities1.contains("CT") && modalities2.contains("NM")) || (modalities1.contains("NM") && modalities2.contains("CT"));
}

} // End namespace udg
