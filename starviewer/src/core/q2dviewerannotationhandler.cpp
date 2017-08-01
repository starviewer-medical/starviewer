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

#include "q2dviewerannotationhandler.h"

#include "q2dviewer.h"
#include "mathtools.h"
#include "applicationstylehelper.h"
#include "mammographyimagehelper.h"
#include "image.h"
#include "study.h"
#include "patient.h"
#include "volume.h"
#include "volumehelper.h"
#include "logging.h"

#include <QRegularExpression>

#include <vtkCornerAnnotation.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>

namespace udg {

Q2DViewerAnnotationHandler::Q2DViewerAnnotationHandler(Q2DViewer *viewer)
{
    m_2DViewer = viewer;

    createAnnotations();
    addActors();
}

Q2DViewerAnnotationHandler::~Q2DViewerAnnotationHandler()
{
    for (int i = 0; i < 4; ++i)
    {
        m_patientOrientationTextActor[i]->Delete();
    }
    m_cornerAnnotations->Delete();
}

void Q2DViewerAnnotationHandler::enableAnnotations(AnnotationFlags annotations)
{
    m_enabledAnnotations |= annotations;
    updateAnnotations();
}

void Q2DViewerAnnotationHandler::disableAnnotations(AnnotationFlags annotations)
{
    m_enabledAnnotations &= ~annotations;
    updateAnnotations();
}

void Q2DViewerAnnotationHandler::updateAnnotations(AnnotationFlags annotations)
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    if (annotations.testFlag(MainInformationAnnotation))
    {
        updateMainInformationAnnotation();
    }

    if (annotations.testFlag(AdditionalInformationAnnotation))
    {
        updateAdditionalInformationAnnotation();
    }

    if (annotations.testFlag(VoiLutAnnotation))
    {
        updateVoiLutAnnotation();
    }

    if (annotations.testFlag(SliceAnnotation))
    {
        updateSliceAnnotation();
    }

    if (annotations.testFlag(PatientOrientationAnnotation))
    {
        updatePatientOrientationAnnotation();
    }
}

void Q2DViewerAnnotationHandler::updateMainInformationAnnotation()
{
    Q_ASSERT(m_2DViewer->hasInput());

    QString annotation;

    if (m_enabledAnnotations.testFlag(MainInformationAnnotation))
    {
        Image *image = m_2DViewer->getCurrentDisplayedImage();
        Series *series = m_2DViewer->getMainInput()->getSeries();
        Study *study = series->getParentStudy();
        Patient *patient = study->getParentPatient();

        QString institutionName = series->getInstitutionName();
        QString patientName = patient->getFullName();
        QString age = study->getPatientAge();
        QString sex = patient->getSex();
        QString patientId = patient->getID();
        QString accessionNumber = study->getAccessionNumber();

        if (!accessionNumber.isEmpty())
        {
            accessionNumber = QObject::tr("Acc: %1").arg(accessionNumber);
        }

        QString studyDate = study->getDate().toString(Qt::ISODate);
        QString seriesTime = series->getTimeAsString();

        if (seriesTime.isEmpty())
        {
            seriesTime = "--:--";
        }

        QString imageTime;

        if (image)
        {
            imageTime = image->getFormattedImageTime();

            if (imageTime.isEmpty())
            {
                imageTime = "--:--";
            }
        }

        annotation = QString("%1\n"
                             "%2\n"
                             "%3 %4 %5\n"
                             "%6\n"
                             "%7\n"
                             "%8\n"
                             "%9")
                .arg(institutionName)
                .arg(patientName)
                .arg(age).arg(sex).arg(patientId)
                .arg(accessionNumber)
                .arg(studyDate)
                .arg(seriesTime)
                .arg(imageTime)
                .trimmed();
    }

    setCornerAnnotation(MainInformationAnnotation, annotation);
}

void Q2DViewerAnnotationHandler::updateAdditionalInformationAnnotation()
{
    Q_ASSERT(m_2DViewer->hasInput());

    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    QString annotation;

    if (mammographyImageHelper.isStandardMammographyImage(image))
    {
        annotation = getMammographyAdditionalInformation();
    }
    else
    {
        annotation = getStandardAdditionalInformation();
    }

    setCornerAnnotation(AdditionalInformationAnnotation, annotation);
}

void Q2DViewerAnnotationHandler::updateVoiLutAnnotation()
{
    Q_ASSERT(m_2DViewer->hasInput());

    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    QString annotation;

    // #1349: Disable the VOI LUT annotation when displaying a mammography
    if (m_enabledAnnotations.testFlag(VoiLutAnnotation) && !mammographyImageHelper.isStandardMammographyImage(image))
    {
        int dimensions[3];
        m_2DViewer->getMainInput()->getDimensions(dimensions);
        int xIndex = m_2DViewer->getView().getXIndex();
        int yIndex = m_2DViewer->getView().getYIndex();
        QString imageSize = QObject::tr("%1 x %2").arg(dimensions[xIndex]).arg(dimensions[yIndex]);
        QString voiLut = getVoiLutString();

        annotation = QString("%1\n"
                             "%2")
                .arg(imageSize)
                .arg(voiLut)
                .trimmed();
    }

    setCornerAnnotation(VoiLutAnnotation, annotation);
}

void Q2DViewerAnnotationHandler::updateSliceAnnotation()
{
    Q_ASSERT(m_2DViewer->hasInput());

    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    QString annotation;

    if (m_enabledAnnotations.testFlag(SliceAnnotation) && !mammographyImageHelper.isStandardMammographyImage(image))
    {
        QString locationInfo = getSliceLocationString();

        // Setup the slice annotation
        QString sliceInfo = QObject::tr("Slice: %1").arg(m_2DViewer->getCurrentSlice() + 1);
        sliceInfo += QString("/%1").arg(m_2DViewer->getNumberOfSlices());
        
        QString phaseInfo;

        // If we have phases
        if (m_2DViewer->hasPhases())
        {
            phaseInfo = QObject::tr(" Phase: %1/%2").arg(m_2DViewer->getCurrentPhase() + 1).arg(m_2DViewer->getNumberOfPhases());
        }

        QString thicknessInfo;
        
        // Add slice thickness only if it is > 0.0mm
        if (m_2DViewer->getCurrentSliceThickness() > 0.0)
        {
            thicknessInfo = QObject::tr(" Thickness: %1 mm").arg(m_2DViewer->getCurrentSliceThickness(), 0, 'f', 2);
        }

        annotation = QString("%1\n"
                             "%2%3%4")
                .arg(locationInfo)
                .arg(sliceInfo).arg(phaseInfo).arg(thicknessInfo)
                .trimmed();
    }

    setCornerAnnotation(SliceAnnotation, annotation);
}

void Q2DViewerAnnotationHandler::updatePatientOrientationAnnotation()
{
    // Get the current image orientation
    PatientOrientation currentPatientOrientation = m_2DViewer->getCurrentDisplayedImagePatientOrientation();

    QString patientOrientationText[4];
    patientOrientationText[LeftOrientationLabelIndex] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getRowDirectionLabel());
    patientOrientationText[BottomOrientationLabelIndex] = currentPatientOrientation.getColumnDirectionLabel();
    patientOrientationText[RightOrientationLabelIndex] = currentPatientOrientation.getRowDirectionLabel();
    patientOrientationText[TopOrientationLabelIndex] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getColumnDirectionLabel());

    // #1349: Hide the posterior label in mammographies so it doesn't cover the image
    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();

    if (mammographyImageHelper.isStandardMammographyImage(image))
    {
        patientOrientationText[LeftOrientationLabelIndex].remove(PatientOrientation::PosteriorLabel);
        patientOrientationText[RightOrientationLabelIndex].remove(PatientOrientation::PosteriorLabel);
    }

    bool enabled = m_enabledAnnotations.testFlag(PatientOrientationAnnotation);

    for (int i = 0; i < 4; ++i)
    {
        if (enabled && !patientOrientationText[i].isEmpty())
        {
            m_patientOrientationTextActor[i]->SetInput(patientOrientationText[i].toUtf8().constData());
            m_patientOrientationTextActor[i]->VisibilityOn();
        }
        else
        {
            m_patientOrientationTextActor[i]->VisibilityOff();
        }
    }
}


QString Q2DViewerAnnotationHandler::getStandardAdditionalInformation() const
{
    if (m_enabledAnnotations.testFlag(AdditionalInformationAnnotation))
    {
        QString lateralityString;
        QChar laterality = m_2DViewer->getCurrentDisplayedImageLaterality();

        if (!laterality.isNull() && !laterality.isSpace())
        {
            lateralityString = QString("Lat: %1").arg(laterality);
        }

        QString seriesLabel = getSeriesDescriptiveLabel(m_2DViewer->getMainInput()->getSeries());
        QString fusionBalance;

        if (m_2DViewer->getNumberOfInputs() == 2)
        {
            int balance = m_2DViewer->getFusionBalance();
            const QString &modality0 = m_2DViewer->getInput(0)->getModality();
            const QString &modality1 = m_2DViewer->getInput(1)->getModality();
            fusionBalance = QObject::tr("Fusion: ") + QString("%1% %2 + %3% %4").arg(100 - balance).arg(modality0).arg(balance).arg(modality1);
            seriesLabel += " +\n" + getSeriesDescriptiveLabel(m_2DViewer->getInput(1)->getSeries());
        }
        
        Study *study = m_2DViewer->getMainInput()->getStudy();
        QString physicianName = study->getReferringPhysiciansName();
        if (!physicianName.isEmpty())
        {
            physicianName = QString(QObject::tr("Physician: %1")).arg(physicianName);
        }

        return QString("%1\n"
                       "%2\n"
                       "%3\n"
                       "%4")
                .arg(lateralityString)
                .arg(fusionBalance)
                .arg(seriesLabel)
                .arg(physicianName)
                .trimmed();
    }
    else
    {
        return QString();
    }
}

QString Q2DViewerAnnotationHandler::getMammographyAdditionalInformation() const
{
    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();

    return QString("%1 %2").arg(image->getImageLaterality()).arg(mammographyImageHelper.getMammographyProjectionLabel(image));
}

QString Q2DViewerAnnotationHandler::getSeriesDescriptiveLabel(Series *series) const
{
    if (!series)
    {
        return QString();
    }

    // If protocol and description are equal, protocol will be set, otherwise they will be merged
    QString protocolName = series->getProtocolName();
    QString description = series->getDescription();

    QString label = protocolName;

    if (description != protocolName)
    {
        label += "\n" + description;
    }

    return label;
}

QString Q2DViewerAnnotationHandler::getVoiLutString() const
{
    VoiLut voiLut = m_2DViewer->getCurrentVoiLut();
    QString lutPart;

    if (voiLut.isLut())
    {
        lutPart = voiLut.getOriginalLutExplanation() + " ";
    }

    WindowLevel windowLevel = voiLut.getWindowLevel();
    QString windowLevelPart = QObject::tr("WW: %1 WL: %2").arg(MathTools::roundToNearestInteger(windowLevel.getWidth()))
                                                          .arg(MathTools::roundToNearestInteger(windowLevel.getCenter()));

    QString thresholdPart;

    if (VolumeHelper::isPrimaryPET(m_2DViewer->getMainInput()) || VolumeHelper::isPrimaryNM(m_2DViewer->getMainInput()))
    {
        double range[2];
        m_2DViewer->getMainInput()->getScalarRange(range);

        double percent = 0.0;
        // Avoid division by zero
        if (range[1] != 0.0)
        {
            percent = (windowLevel.getWidth() / range[1]) * 100;
        }

        thresholdPart = "\n" + QObject::tr("Threshold: %1%").arg(percent, 0, 'f', 2);
    }

    return lutPart + windowLevelPart + thresholdPart;
}

QString Q2DViewerAnnotationHandler::getSliceLocationString() const
{
    QString sliceLocation;
    
    // Slice location will be present only when we are on the original acquisition plane
    if (m_2DViewer->getView() == OrthogonalPlane::XYPlane)
    {
        Image *image = m_2DViewer->getCurrentDisplayedImage();
        if (image)
        {
            QString location = image->getSliceLocation();
            if (!location.isEmpty())
            {
                double loc = location.toDouble();
                if (m_2DViewer->isThickSlabActive())
                {
                    double halfThickness = m_2DViewer->getSlabThickness() / 2.0;
                    sliceLocation = QObject::tr("Loc: %1-%2").arg(loc - halfThickness, 0, 'f', 2)
                                                             .arg(loc + halfThickness, 0, 'f', 2);
                }
                else
                {
                    sliceLocation = QObject::tr("Loc: %1").arg(loc, 0, 'f', 2);
                }
            }
        }
    }

    return sliceLocation;
}

Q2DViewerAnnotationHandler::CornerAnnotationIndexType Q2DViewerAnnotationHandler::getCornerForAnnotationType(AnnotationFlag annotation) const
{
    MammographyImageHelper mammographyImageHelper;
    Image *image = m_2DViewer->getCurrentDisplayedImage();

    // #1349: If displaying a mammography and the posterior side is at the right, then swap annotation sides so that patient information doesn't cover the image
    if (mammographyImageHelper.isStandardMammographyImage(image) &&
            m_2DViewer->getCurrentDisplayedImagePatientOrientation().getRowDirectionLabel() == PatientOrientation::PosteriorLabel)
    {
        switch (annotation)
        {
            case MainInformationAnnotation: return UpperLeftCornerIndex;
            case AdditionalInformationAnnotation: return LowerLeftCornerIndex;
            case VoiLutAnnotation: return UpperRightCornerIndex;
            case SliceAnnotation: return LowerRightCornerIndex;
            default: break;
        }
    }
    else
    {
        switch (annotation)
        {
            case MainInformationAnnotation: return UpperRightCornerIndex;
            case AdditionalInformationAnnotation: return LowerRightCornerIndex;
            case VoiLutAnnotation: return UpperLeftCornerIndex;
            case SliceAnnotation: return LowerLeftCornerIndex;
            default: break;
        }
    }

    // Default case that should not be reached
    DEBUG_LOG(QString("Asked for the corner of an unexpected annotation flag: %1. Returning default value.").arg(annotation));
    WARN_LOG(QString("Asked for the corner of an unexpected annotation flag: %1. Returning default value.").arg(annotation));
    return LowerLeftCornerIndex;
}

void Q2DViewerAnnotationHandler::setCornerAnnotation(AnnotationFlag annotation, QString text)
{
    // If the given annotation is not one of the expected values, then do nothing
    if (annotation != MainInformationAnnotation && annotation != AdditionalInformationAnnotation
            && annotation != VoiLutAnnotation && annotation != SliceAnnotation)
    {
        return;
    }

    // Clean whitespace at both sides of the string
    text = text.trimmed();

    if (text.isEmpty())
    {
        // Use a space instead of an empty string to avoid graphical problems with vtkCornerAnnotation
        text = " ";
    }
    else
    {
        // Remove empty lines
        text = text.replace(QRegularExpression("\n+"), "\n");
    }

    m_cornerAnnotations->SetText(getCornerForAnnotationType(annotation), text.toUtf8().constData());
}

void Q2DViewerAnnotationHandler::createAnnotations()
{
    m_cornerAnnotations = vtkCornerAnnotation::New();
    m_cornerAnnotations->SetMaximumFontSize(ApplicationStyleHelper(true).getCornerAnnotationFontSize());
    m_cornerAnnotations->GetTextProperty()->SetFontFamilyToArial();
    m_cornerAnnotations->GetTextProperty()->ShadowOn();

    createOrientationAnnotations();
}

void Q2DViewerAnnotationHandler::createOrientationAnnotations()
{
    for (int i = 0; i < 4; i++)
    {
        m_patientOrientationTextActor[i] = vtkTextActor::New();
        m_patientOrientationTextActor[i]->SetTextScaleModeToNone();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontSize(ApplicationStyleHelper(true).getOrientationAnnotationFontSize());
        m_patientOrientationTextActor[i]->GetTextProperty()->BoldOn();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontFamilyToArial();
        m_patientOrientationTextActor[i]->GetTextProperty()->ShadowOn();

        m_patientOrientationTextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        m_patientOrientationTextActor[i]->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }
    // Place each actor on its corresponding place. 0-3, counter-clockwise direction, starting at 0 = left of the viewer
    m_patientOrientationTextActor[LeftOrientationLabelIndex]->GetTextProperty()->SetJustificationToLeft();
    m_patientOrientationTextActor[LeftOrientationLabelIndex]->SetPosition(0.01, 0.5);

    m_patientOrientationTextActor[BottomOrientationLabelIndex]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[BottomOrientationLabelIndex]->SetPosition(0.5, 0.01);

    m_patientOrientationTextActor[RightOrientationLabelIndex]->GetTextProperty()->SetJustificationToRight();
    m_patientOrientationTextActor[RightOrientationLabelIndex]->SetPosition(0.99, 0.5);

    m_patientOrientationTextActor[TopOrientationLabelIndex]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[TopOrientationLabelIndex]->GetTextProperty()->SetVerticalJustificationToTop();
    m_patientOrientationTextActor[TopOrientationLabelIndex]->SetPosition(0.5, 0.99);
}

void Q2DViewerAnnotationHandler::addActors()
{
    Q_ASSERT(m_cornerAnnotations);
    Q_ASSERT(m_patientOrientationTextActor[0]);
    Q_ASSERT(m_patientOrientationTextActor[1]);
    Q_ASSERT(m_patientOrientationTextActor[2]);
    Q_ASSERT(m_patientOrientationTextActor[3]);

    vtkRenderer *renderer = m_2DViewer->getRenderer();
    Q_ASSERT(renderer);
    
    renderer->AddViewProp(m_cornerAnnotations);
    renderer->AddViewProp(m_patientOrientationTextActor[0]);
    renderer->AddViewProp(m_patientOrientationTextActor[1]);
    renderer->AddViewProp(m_patientOrientationTextActor[2]);
    renderer->AddViewProp(m_patientOrientationTextActor[3]);
}

} // End namespace udg
