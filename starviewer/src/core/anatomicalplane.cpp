#include "anatomicalplane.h"

#include <QObject>
#include <QStringList>

#include "patientorientation.h"
#include "dicomvaluerepresentationconverter.h"

namespace udg {

const QString AnatomicalPlane::getLabel(AnatomicalPlaneType orientation)
{
    QString label;

    switch (orientation)
    {
        case Axial:
            label = QObject::tr("AXIAL");
            break;

        case Sagittal:
            label = QObject::tr("SAGITTAL");
            break;

        case Coronal:
            label = QObject::tr("CORONAL");
            break;

        case Oblique:
            label = QObject::tr("OBLIQUE");
            break;

        case NotAvailable:
            label = QObject::tr("N/A");
            break;
    }

    return label;
}

const QString AnatomicalPlane::getProjectionLabelFromPlaneOrientation(const QString &orientation)
{
    QString label;

    QStringList axisList = orientation.split(DICOMValueRepresentationConverter::ValuesSeparator);
    // Comprovem si tenim les annotacions esperades
    if (axisList.count() >= 2)
    {
        QString rowAxis = axisList.at(0).trimmed();
        QString columnAxis = axisList.at(1).trimmed();

        if (!rowAxis.isEmpty() && !columnAxis.isEmpty())
        {
            if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
                (columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Axial);
            }
            else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
                (rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Axial);
            }
            else if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
                (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Coronal);
            }
            else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
                (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Coronal);
            }
            else if ((rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
                (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Sagittal);
            }
            else if ((columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
                (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Sagittal);
            }
            else
            {
                label = AnatomicalPlane::getLabel(AnatomicalPlane::Oblique);
            }
        }
        else
        {
            label = AnatomicalPlane::getLabel(AnatomicalPlane::NotAvailable);
        }
    }

    return label;
}

} // End namespace udg
