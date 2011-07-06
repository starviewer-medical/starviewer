#include "patientorientation.h"

#include <QRegExp>
#include <QStringList>
#include <cmath>

#include "dicomvaluerepresentationconverter.h"
#include "imageorientation.h"
#include "logging.h"

namespace udg {

const QString PatientOrientation::LeftLabel("L");
const QString PatientOrientation::RightLabel("R");
const QString PatientOrientation::PosteriorLabel("P");
const QString PatientOrientation::AnteriorLabel("A");
const QString PatientOrientation::HeadLabel("H");
const QString PatientOrientation::FeetLabel("F");

bool PatientOrientation::setDICOMFormattedPatientOrientation(const QString &patientOrientation)
{
    if (validateDICOMFormattedPatientOrientationString(patientOrientation))
    {
        m_patientOrientationString = patientOrientation;
        return true;
    }
    else
    {
        m_patientOrientationString = "";
        return false;
    }
}

QString PatientOrientation::getDICOMFormattedPatientOrientation() const
{
    return m_patientOrientationString;
}

void PatientOrientation::setPatientOrientationFromImageOrientation(const ImageOrientation &imageOrientation)
{
    QString patientOrientationString;
    patientOrientationString = this->getOrientationLabelFromDirectionVector(imageOrientation.getRowVector());
    patientOrientationString += DICOMValueRepresentationConverter::ValuesSeparator;
    patientOrientationString += this->getOrientationLabelFromDirectionVector(imageOrientation.getColumnVector());
    patientOrientationString += DICOMValueRepresentationConverter::ValuesSeparator;
    patientOrientationString += this->getOrientationLabelFromDirectionVector(imageOrientation.getNormalVector());

    if (!setDICOMFormattedPatientOrientation(patientOrientationString))
    {
        DEBUG_LOG("makePatientOrientationFromImageOrientationPatient() ha generat una cadena d'orientació de pacient invàlida");
    }
}

QString PatientOrientation::getRowDirectionLabel() const
{
    return getNthDirectionLabel(0);
}

QString PatientOrientation::getColumnDirectionLabel() const
{
    return getNthDirectionLabel(1);
}

QString PatientOrientation::getNormalDirectionLabel() const
{
    return getNthDirectionLabel(2);
}

QString PatientOrientation::getNthDirectionLabel(int i) const
{
    QString label;
    
    if (!m_patientOrientationString.isEmpty())
    {
        QStringList labelList = m_patientOrientationString.split(DICOMValueRepresentationConverter::ValuesSeparator);
        if (labelList.size() >= i + 1 && i >= 0)
        {
            label = labelList.at(i);
        }
    }

    return label;
}

QString PatientOrientation::getOppositeOrientationLabel(const QString &label)
{
    int i = 0;
    QString oppositeLabel;
    while (i < label.size())
    {
        if (QString(label.at(i)) == PatientOrientation::LeftLabel)
        {
            oppositeLabel += PatientOrientation::RightLabel;
        }
        else if (QString(label.at(i)) == PatientOrientation::RightLabel)
        {
            oppositeLabel += PatientOrientation::LeftLabel;
        }
        else if (QString(label.at(i)) == PatientOrientation::AnteriorLabel)
        {
            oppositeLabel += PatientOrientation::PosteriorLabel;
        }
        else if (QString(label.at(i)) == PatientOrientation::PosteriorLabel)
        {
            oppositeLabel += PatientOrientation::AnteriorLabel;
        }
        else if (QString(label.at(i)) == PatientOrientation::HeadLabel)
        {
            oppositeLabel += PatientOrientation::FeetLabel;
        }
        else if (QString(label.at(i)) == PatientOrientation::FeetLabel)
        {
            oppositeLabel += PatientOrientation::HeadLabel;
        }
        else
        {
            oppositeLabel += "?";
        }
        i++;
    }
    return oppositeLabel;
}

bool PatientOrientation::validateDICOMFormattedPatientOrientationString(const QString &string)
{
    // Construim l'expressió regular que ens comprova que la cadena està en el format correcte: Cadena buida, o amb 2 o 3 elements.
    
    // Etiquetes vàlides: RLAPHF
    QString validOrientationLabels = RightLabel + LeftLabel + AnteriorLabel + PosteriorLabel + HeadLabel + FeetLabel;
    
    // Expressió per les etiquetes [RLAPHF]+
    QString validLabelsExpression = "[" + validOrientationLabels + "]+";
    
    // Expressió pels separadors 
    // Hem de posar dos cops seguits DICOMValueRepresentationConverter::ValuesSeparator, ja que tal com diu la documentació de QRegExp:
    //
    // Note: The C++ compiler transforms backslashes in strings. To include a \ in a regexp, enter it twice, i.e. \\. 
    // To match the backslash character itself, enter it four times, i.e. \\\\.
    //
    // Llavors cal posar \\\\ perquè l'expressió sigui correcta
    QString separatorExpression = DICOMValueRepresentationConverter::ValuesSeparator + DICOMValueRepresentationConverter::ValuesSeparator;
    
    // L'expressió final: ([RLAPHF]+\\\\[RLAPHF]+(\\\\[RLAPHF]+)?)*
    // És a dir, 2 o 3 ítems amb etiquetes vàlides separats per \\.
    QString regExpString = "(" + validLabelsExpression + separatorExpression + validLabelsExpression +
        "(" + separatorExpression + validLabelsExpression + ")?)*";

    QRegExp validStringExpression(regExpString);
    return validStringExpression.exactMatch(string);
}

QString PatientOrientation::getOrientationLabelFromDirectionVector(const QVector3D &vector)
{
    QString orientation;

    QString orientationX = vector.x() < 0 ? RightLabel : LeftLabel;
    QString orientationY = vector.y() < 0 ? AnteriorLabel : PosteriorLabel;
    QString orientationZ = vector.z() < 0 ? FeetLabel : HeadLabel;

    double absX = fabs(vector.x());
    double absY = fabs(vector.y());
    double absZ = fabs(vector.z());

    for (int i = 0; i < 3; ++i)
    {
        if (absX > .0001 && absX > absY && absX > absZ)
        {
            orientation += orientationX;
            absX = 0;
        }
        else if (absY > .0001 && absY > absX && absY > absZ)
        {
            orientation += orientationY;
            absY = 0;
        }
        else if (absZ > .0001 && absZ > absX && absZ > absY)
        {
            orientation += orientationZ;
            absZ = 0;
        }
        else
        {
            break;
        }
    }
    
    return orientation;
}

} // End namespace udg
