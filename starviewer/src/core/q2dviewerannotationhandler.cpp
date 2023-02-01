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

#include "applicationstylehelper.h"
#include "image.h"
#include "mammographyimagehelper.h"
#include "patient.h"
#include "q2dviewer.h"
#include "q2dviewerannotationssettingshelper.h"
#include "series.h"
#include "study.h"
#include "volume.h"
#include "volumehelper.h"

#include <QRegularExpression>

#include <vtkCornerAnnotation.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

namespace udg {

namespace {

// Several methods to obtain values for the recognized keys in the annotation templates.
// getDicom* methods return directly data from the corresponding DICOM tag with no further processing; corresponding keys start with an uppercase letter.
// getVariable* methods return a value that comes from a DICOM tag but is processed, or a value that depends on the viewer status;
// corresponding keys start with a lowercase letter.

QString getDicomPatientID(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getPatient()->getID();
}

QString getDicomPatientName(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getPatient()->getFullName();
}

QString getDicomPatientSex(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getPatient()->getSex();
}

QString getVariablePatientAge(const Q2DViewer *viewer)
{
    Study *study = viewer->getMainInput()->getStudy();
    QString age = study->getPatientAge();
    QString calculatedAge = study->getCalculatedPatientAge();

    if (!calculatedAge.isEmpty() && age != calculatedAge)
    {
        age = QString(QObject::tr("WARNING: age mismatch\n%1 (stored) / %2 (calculated)\n").arg(age, calculatedAge));
    }

    return age;
}

QString getDicomAccessionNumber(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getStudy()->getAccessionNumber();
}

QString getDicomInstitutionName(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getSeries()->getInstitutionName();
}

QString getDicomReferringPhysicianName(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getStudy()->getReferringPhysiciansName();
}

QString getVariableStudyDate(const Q2DViewer *viewer)
{
    return viewer->getMainInput()->getStudy()->getDate().toString(Qt::ISODate);
}

QString getVariableSeriesTime(const Q2DViewer *viewer)
{
    QString seriesTime = viewer->getMainInput()->getSeries()->getTimeAsString();

    if (seriesTime.isEmpty())
    {
        seriesTime = "--:--";
    }

    return seriesTime;
}

QString getVariableImageTime(const Q2DViewer *viewer)
{
    QString imageTime;
    Image *image = viewer->getCurrentDisplayedImage();

    if (image)
    {
        imageTime = image->getFormattedImageTime();

        if (imageTime.isEmpty())
        {
            imageTime = "--:--";
        }
    }

    return imageTime;
}

QString getVariableSeriesLabel(const Q2DViewer *viewer)
{
    auto getSeriesDescriptiveLabel = [](Series *series) -> QString {
        if (!series)
        {
            return {};
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
    };

    QString seriesLabel = getSeriesDescriptiveLabel(viewer->getMainInput()->getSeries());

    if (viewer->getNumberOfInputs() == 2)
    {
        seriesLabel += "\n+\n" + getSeriesDescriptiveLabel(viewer->getInput(1)->getSeries());

        if (seriesLabel.trimmed() == "+")
        {
            seriesLabel = "";   // don't show a single '+' if both labels are empty
        }
    }

    return seriesLabel;
}

QString getDicomImageLaterality(const Q2DViewer *viewer)
{
    Image *image = viewer->getCurrentDisplayedImage();
    return image ? image->getImageLaterality() : QString();
}

QString getVariableLaterality(const Q2DViewer *viewer)
{
    QChar laterality = viewer->getCurrentDisplayedImageLaterality();

    if (!laterality.isNull() && !laterality.isSpace())
    {
        return QString(laterality);
    }
    else
    {
        return {};
    }
}

QString getVariableMGProjection(const Q2DViewer *viewer)
{
    Image *image = viewer->getCurrentDisplayedImage();
    MammographyImageHelper mammographyImageHelper;
    return mammographyImageHelper.getMammographyProjectionLabel(image);
}

QString getVariableFusionBalance(const Q2DViewer *viewer)
{
    QString fusionBalance;

    if (viewer->getNumberOfInputs() == 2)
    {
        int balance = viewer->getFusionBalance();
        const QString &modality0 = viewer->getInput(0)->getModality();
        const QString &modality1 = viewer->getInput(1)->getModality();
        fusionBalance = QString("%1% %2 + %3% %4").arg(100 - balance).arg(modality0).arg(balance).arg(modality1);
    }

    return fusionBalance;
}

QString getVariableImageWidth(const Q2DViewer *viewer)
{
    int dimensions[3];
    viewer->getMainInput()->getDimensions(dimensions);
    return QString::number(dimensions[viewer->getView().getXIndex()]);
}

QString getVariableImageHeight(const Q2DViewer *viewer)
{
    int dimensions[3];
    viewer->getMainInput()->getDimensions(dimensions);
    return QString::number(dimensions[viewer->getView().getYIndex()]);
}

QString getVariableLutName(const Q2DViewer *viewer)
{
    VoiLut voiLut = viewer->getCurrentVoiLut();
    return voiLut.isLut() ? voiLut.getOriginalLutExplanation() : QString();
}

// Converts the given window width or center value to string with a number of decimals depending on its magnitude.
QString printWindowLevel(double x)
{
    if (std::abs(x) >= 100)     // for |x| >= 100 -> 0 decimals
    {
        return QString::number(MathTools::roundToNearestInteger(x));
    }
    else if (std::abs(x) >= 10) // for 10 <= |x| < 100 -> at most 2 decimals
    {
        return QString::number(x, 'g', 4);
    }
    else if (std::abs(x) >= 1)  // for 1 <= |x| < 10 -> at most 2 decimals
    {
        return QString::number(x, 'g', 3);
    }
    else                        // for |x| < 1 -> 5 decimals
    {
        return QString::number(x, 'f', 5);
    }
}

QString getVariableWindowWidth(const Q2DViewer *viewer)
{
    return printWindowLevel(viewer->getCurrentVoiLut().getWindowLevel().getWidth());
}

QString getVariableWindowCenter(const Q2DViewer *viewer)
{
    return printWindowLevel(viewer->getCurrentVoiLut().getWindowLevel().getCenter());
}

QString getVariableThreshold(const Q2DViewer *viewer)
{
    Volume *volume = viewer->getMainInput();

    if (VolumeHelper::isPrimaryPET(volume) || VolumeHelper::isPrimaryNM(volume))
    {
        double range[2];
        volume->getScalarRange(range);
        double percent = 0.0;

        if (range[1] != 0.0)    // avoid division by zero
        {
            double windowWidth = viewer->getCurrentVoiLut().getWindowLevel().getWidth();
            percent = (windowWidth / range[1]) * 100;
        }

        return QString::number(percent, 'f', 2);
    }
    else
    {
        return {};
    }
}

QString getVariableLocation(const Q2DViewer *viewer)
{
    QString sliceLocation;

    // Slice location will be present only when we are on the original acquisition plane
    if (viewer->getView() == OrthogonalPlane::XYPlane)
    {
        Image *image = viewer->getCurrentDisplayedImage();

        if (image)
        {
            QString location = image->getSliceLocation();

            if (!location.isEmpty())
            {
                double loc = location.toDouble();

                if (viewer->isThickSlabActive())
                {
                    double halfThickness = viewer->getSlabThickness() / 2.0;
                    sliceLocation = QString("%1-%2").arg(loc - halfThickness, 0, 'f', 2).arg(loc + halfThickness, 0, 'f', 2);
                }
                else
                {
                    sliceLocation = QString::number(loc, 'f', 2);   // we ensure 2 decimals independently of the original format
                }
            }
        }
    }

    return sliceLocation;
}

QString getVariableSlice(const Q2DViewer *viewer)
{
    return QString("%1/%2").arg(viewer->getCurrentSlice() + 1).arg(viewer->getNumberOfSlices());
}

QString getVariablePhase(const Q2DViewer *viewer)
{
    return viewer->hasPhases() ? QString("%1/%2").arg(viewer->getCurrentPhase() + 1).arg(viewer->getNumberOfPhases()) : QString();
}

QString getVariableThickness(const Q2DViewer *viewer)
{
    double thickness = viewer->getCurrentSliceThickness();
    return thickness > 0.0 ? QString::number(thickness, 'f', 2) : QString();
}

// Map from variable to getter function.
const QHash<QString, decltype(&getDicomPatientID)> VariableGetters{
    {"PatientID", &getDicomPatientID},
    {"PatientName", &getDicomPatientName},
    {"PatientSex", &getDicomPatientSex},
    {"patientAge", &getVariablePatientAge},
    {"AccessionNumber", &getDicomAccessionNumber},
    {"InstitutionName", &getDicomInstitutionName},
    {"ReferringPhysicianName", &getDicomReferringPhysicianName},
    {"studyDate", &getVariableStudyDate},
    {"seriesTime", &getVariableSeriesTime},
    {"imageTime", &getVariableImageTime},
    {"seriesLabel", &getVariableSeriesLabel},
    {"ImageLaterality", &getDicomImageLaterality},
    {"laterality", &getVariableLaterality},
    {"mgProjection", &getVariableMGProjection},
    {"fusionBalance", &getVariableFusionBalance},
    {"imageWidth", &getVariableImageWidth},
    {"imageHeight", &getVariableImageHeight},
    {"lutName", &getVariableLutName},
    {"windowWidth", &getVariableWindowWidth},
    {"windowCenter", &getVariableWindowCenter},
    {"threshold", &getVariableThreshold},
    {"location", &getVariableLocation},
    {"slice", &getVariableSlice},
    {"phase", &getVariablePhase},
    {"thickness", &getVariableThickness}
};

// Gets the value from the given DICOM tag in "ggggeeee" format, if present, and returns it.
QString getCustomDicomTag(const Q2DViewer *viewer, const QString &dicomTag)
{
    Image *image = viewer->getCurrentDisplayedImage();

    if (!image)
    {
        image = viewer->getMainInput()->getImage(0);    // if we don't have a current image, use the first image

        if (!image)
        {
            return {};
        }
    }

    int group = dicomTag.left(4).toInt(nullptr, 16);
    int element = dicomTag.right(4).toInt(nullptr, 16);
    return image->getDicomTagReader().getValueAttributeAsQString(DICOMTag(group, element));
}

}

const QVector<QPair<QString, QString>>& Q2DViewerAnnotationHandler::getSupportedAnnotations()
{
    static QVector<QPair<QString, QString>> SupportedAnnotations{
        {"PatientID", tr("Patient ID, directly extracted from the DICOM tag.")},
        {"PatientName", tr("Patient’s name, directly extracted from the DICOM tag.")},
        {"PatientSex", tr("Patient’s sex, directly extracted from the DICOM tag.")},
        {"patientAge", tr("Patient’s age, either from the DICOM tag or calculated. If both differ, a warning is shown.")},
        {"AccessionNumber", tr("Accession number, directly extracted from the DICOM tag.")},
        {"InstitutionName", tr("Institution name, directly extracted from the DICOM tag.")},
        {"ReferringPhysicianName", tr("Referring physician’s name, directly extracted from the DICOM tag.")},
        {"studyDate", tr("Study date, in ISO 8601 format.")},
        {"seriesTime", tr("Series time, in “hh:mm:ss” format. If not available, “--:--” is shown.")},
        {"imageTime", tr("Image time, in “hh:mm:ss” format. If not available, “--:--” is shown. In reconstructions nothing is shown.")},
        {"seriesLabel", tr("Descriptive label for the series, including the protocol name and the series description if this is different than the protocol "
                           "name. When viewing a fusion of two series, descriptive labels for both series are shown separated by a “+”.")},
        {"ImageLaterality", tr("Image laterality, directly extracted from the DICOM tag.")},
        {"laterality", tr("Image laterality or, if not found, series laterality, directly extracted from the DICOM tag.")},
        {"mgProjection", tr("Mammography projection label using standardised values.")},
        {"fusionBalance", tr("Fusion balance, showing the contribution of each image. Only shown if there is a fusion.")},
        {"imageWidth", tr("Image width in pixels with the current projection.")},
        {"imageHeight", tr("Image height in pixels with the current projection.")},
        {"lutName", tr("Name of the applied VOI LUT, if any, usually extracted from the DICOM tag “LUT Explanation”. Does not change with the VOI LUT tool.")},
        {"windowWidth", tr("Current window width.")},
        {"windowCenter", tr("Current window center.")},
        {"threshold", tr("Current threshold in PET and SPECT images. Not shown for other image types.")},
        {"location", tr("Slice location with 2 decimals. When thick slab is active, a range is shown. In reconstructions nothing is shown.")},
        {"slice", tr("Current slice and number of slices in “current/total” format.")},
        {"phase", tr("Current phase and number of phases in “current/total” format. If there is only one phase nothing is shown.")},
        {"thickness", tr("Thickness of the current slice or the slab. If thickness is 0, nothing is shown.")}
    };

    return SupportedAnnotations;
}

Q2DViewerAnnotationHandler::Q2DViewerAnnotationHandler(Q2DViewer *viewer)
    : m_2DViewer(viewer), m_annotationsEnabled(true)
{
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

void Q2DViewerAnnotationHandler::enableAnnotations(bool enable)
{
    m_annotationsEnabled = enable;
    updateAnnotations();
}

void Q2DViewerAnnotationHandler::updateAnnotations()
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    for (CornerAnnotationIndexType corner : {LowerLeftCornerIndex, LowerRightCornerIndex, UpperLeftCornerIndex, UpperRightCornerIndex})
    {
        updateCornerAnnotation(corner);
    }

    updatePatientOrientationAnnotation();
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

void Q2DViewerAnnotationHandler::updateCornerAnnotation(CornerAnnotationIndexType corner)
{
    static const QRegularExpression VariableRegularExpression("{%([a-z]+)(?::(.*?))?%}", QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression DicomTagRegularExpression("{%([0-9a-f]{8})(?::(.*?))?%}", QRegularExpression::CaseInsensitiveOption);

    if (m_annotationsEnabled && m_2DViewer->hasInput())
    {
        QString annotation = getAnnotationTemplateForCorner(corner);
        QRegularExpressionMatchIterator it = VariableRegularExpression.globalMatch(annotation);

        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();
            QString matchedString = match.captured(0);
            QString variable = match.captured(1);
            QString format = match.captured(2); // null if not specified

            if (VariableGetters.contains(variable))
            {
                QString value = VariableGetters[variable](m_2DViewer);

                if (!format.isNull() && !value.isEmpty())
                {
                    value = format.replace("$&", value);
                }

                annotation.replace(matchedString, value);
            }
        }

        it = DicomTagRegularExpression.globalMatch(annotation);

        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();
            QString matchedString = match.captured(0);
            QString dicomTag = match.captured(1);
            QString format = match.captured(2); // null if not specified
            QString value = getCustomDicomTag(m_2DViewer, dicomTag);

            if (!format.isNull() && !value.isEmpty())
            {
                value = format.replace("$&", value);
            }

            annotation.replace(matchedString, value);
        }

        setCornerAnnotation(corner, annotation);
    }
    else
    {
        setCornerAnnotation(corner, QString());
    }
}

void Q2DViewerAnnotationHandler::updatePatientOrientationAnnotation()
{
    PatientOrientation currentPatientOrientation = m_2DViewer->getCurrentDisplayedImagePatientOrientation();
    QString patientOrientationText[4];
    patientOrientationText[LeftOrientationLabelIndex] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getRowDirectionLabel());
    patientOrientationText[BottomOrientationLabelIndex] = currentPatientOrientation.getColumnDirectionLabel();
    patientOrientationText[RightOrientationLabelIndex] = currentPatientOrientation.getRowDirectionLabel();
    patientOrientationText[TopOrientationLabelIndex] = PatientOrientation::getOppositeOrientationLabel(currentPatientOrientation.getColumnDirectionLabel());

    Q2DViewerAnnotationsSettingsHelper helper;
    Q2DViewerAnnotationsSettings settings = helper.getSettings(m_2DViewer->getMainInput()->getModality());
    bool showOrientation[4];
    showOrientation[TopOrientationLabelIndex] = settings.topOrientation;
    showOrientation[BottomOrientationLabelIndex] = settings.bottomOrientation;
    showOrientation[LeftOrientationLabelIndex] = settings.leftOrientation;
    showOrientation[RightOrientationLabelIndex] = settings.rightOrientation;

    Image *image = m_2DViewer->getCurrentDisplayedImage();
    MammographyImageHelper mammographyImageHelper;

    // #1349: If displaying a mammography and the posterior side is at the right, then swap annotation sides so that patient information doesn't cover the image
    if (settings.invertSidesMG && mammographyImageHelper.isStandardMammographyImage(image) &&
            m_2DViewer->getCurrentDisplayedImagePatientOrientation().getRowDirectionLabel() == PatientOrientation::PosteriorLabel)
    {
        showOrientation[LeftOrientationLabelIndex] = settings.rightOrientation;
        showOrientation[RightOrientationLabelIndex] = settings.leftOrientation;
    }

    for (int i = 0; i < 4; ++i)
    {
        if (m_annotationsEnabled && showOrientation[i] && !patientOrientationText[i].isEmpty())
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

QString Q2DViewerAnnotationHandler::getAnnotationTemplateForCorner(CornerAnnotationIndexType corner) const
{
    Q2DViewerAnnotationsSettingsHelper helper;
    Q2DViewerAnnotationsSettings settings = helper.getSettings(m_2DViewer->getMainInput()->getModality());
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    MammographyImageHelper mammographyImageHelper;

    // #1349: If displaying a mammography and the posterior side is at the right, then swap annotation sides so that patient information doesn't cover the image
    if (settings.invertSidesMG && mammographyImageHelper.isStandardMammographyImage(image) &&
            m_2DViewer->getCurrentDisplayedImagePatientOrientation().getRowDirectionLabel() == PatientOrientation::PosteriorLabel)
    {
        switch (corner)
        {
            case UpperLeftCornerIndex: return settings.topRight;
            case UpperRightCornerIndex: return settings.topLeft;
            case LowerLeftCornerIndex: return settings.bottomRight;
            case LowerRightCornerIndex: return settings.bottomLeft;
        }
    }
    else
    {
        switch (corner)
        {
            case UpperLeftCornerIndex: return settings.topLeft;
            case UpperRightCornerIndex: return settings.topRight;
            case LowerLeftCornerIndex: return settings.bottomLeft;
            case LowerRightCornerIndex: return settings.bottomRight;
        }
    }

    return {};
}

void Q2DViewerAnnotationHandler::setCornerAnnotation(CornerAnnotationIndexType corner, QString text)
{
    text = text.trimmed();                              // remove whitespace at the beginning and the end
    text.replace(QRegularExpression(" +"), " ");        // reduce multiple spaces to one
    text.replace(QRegularExpression("\n | \n"), "\n");  // remove spaces at the beginning and the end of each line
    text.replace(QRegularExpression("\n+"), "\n");      // remove empty lines
    m_cornerAnnotations->SetText(corner, text.toUtf8().constData());
}

} // End namespace udg
