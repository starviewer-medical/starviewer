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

#include "mammographyimagehelper.h"

#include "image.h"
#include "study.h"
#include "coresettings.h"
#include "logging.h"

#include <QStringList>

namespace udg {

MammographyImageHelper::MammographyImageHelper()
{
}

MammographyImageHelper::~MammographyImageHelper()
{
}

const PatientOrientation MammographyImageHelper::getImageOrientationPresentation(Image *image)
{
    QString projection = getMammographyProjectionLabel(image);
    QString laterality = image->getImageLaterality();
    // S'han de seguir les recomanacions IHE de presentació d'imatges de Mammografia
    // IHE Techincal Framework Vol. 2 revision 8.0, apartat 4.16.4.2.2.1.1.2 Image Orientation and Justification
    PatientOrientation desiredOrientation;
    if (projection == "CC" || projection == "XCC" || projection == "XCCL" || projection == "XCCM" || projection == "FB")
    {
        if (laterality == PatientOrientation::LeftLabel)
        {
            desiredOrientation.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::RightLabel);
        }
        else if (laterality == PatientOrientation::RightLabel)
        {
            desiredOrientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::LeftLabel);
        }
    }
    else if (projection == "MLO" || projection == "ML" || projection == "LM" || projection == "LMO" || projection == "SIO")
    {
        if (laterality == PatientOrientation::LeftLabel)
        {
            desiredOrientation.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::FeetLabel);
        }
        else if (laterality == PatientOrientation::RightLabel)
        {
            desiredOrientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::FeetLabel);
        }
    }
    else
    {
        DEBUG_LOG("Unknown projection found: " + projection);
    }

    return desiredOrientation;
}

const QStringList MammographyImageHelper::getMammographyAutoOrientationExceptions()
{
    Settings settings;
    QStringList mammographyAutoOrientationExceptions = settings.getValue(CoreSettings::MammographyAutoOrientationExceptions).toStringList();

    return mammographyAutoOrientationExceptions;
}

bool MammographyImageHelper::isStandardMammographyImage(Image *image)
{
    if (!image)
    {
        return false;
    }

    Series *imageSeries = image->getParentSeries();
    if (!imageSeries)
    {
        return false;
    }

    if (imageSeries->getModality() != "MG")
    {
        return false;
    }

    Study *imageStudy = imageSeries->getParentStudy();
    if (!imageStudy)
    {
        return true;
    }

    QString studyDescription = imageStudy->getDescription();
    // We check if this image has some exception that excludes it from standard mammography
    bool hasException = false;

    QListIterator<QString> iterator(getMammographyAutoOrientationExceptions());
    while (!hasException && iterator.hasNext())
    {
        hasException = studyDescription.contains(iterator.next(), Qt::CaseInsensitive);
    }

    return !hasException;
}

const QString MammographyImageHelper::getMammographyProjectionLabel(Image *image)
{
    QString projectionLabel;

    if (!image)
    {
        return projectionLabel;
    }

    QString codeMeaning = image->getViewCodeMeaning();
    // PS 3.16 - 2008, Page 408, Context ID 4014, View for mammography
    // TODO Tenir-ho carregat en arxius, maps, etc..
    // TODO Fer servir millor els codis [Code Value (0008,0100)] en compte dels "code meanings" podria resultar més segur
    if (codeMeaning == "medio-lateral")
    {
        projectionLabel = "ML";
    }
    else if (codeMeaning == "medio-lateral oblique")
    {
        projectionLabel = "MLO";
    }
    else if (codeMeaning == "latero-medial")
    {
        projectionLabel = "LM";
    }
    else if (codeMeaning == "latero-medial oblique")
    {
        projectionLabel = "LMO";
    }
    else if (codeMeaning == "cranio-caudal")
    {
        projectionLabel = "CC";
    }
    else if (codeMeaning == "caudo-cranial (from below)")
    {
        projectionLabel = "FB";
    }
    else if (codeMeaning == "superolateral to inferomedial oblique")
    {
        projectionLabel = "SIO";
    }
    else if (codeMeaning == "exaggerated cranio-caudal")
    {
        projectionLabel = "XCC";
    }
    else if (codeMeaning == "cranio-caudal exaggerated laterally")
    {
        projectionLabel = "XCCL";
    }
    else if (codeMeaning == "cranio-caudal exaggerated medially")
    {
        projectionLabel = "XCCM";
    }

    return projectionLabel;
}

}
