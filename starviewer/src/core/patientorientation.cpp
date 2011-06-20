#include "patientorientation.h"

#include <QRegExp>
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

void PatientOrientation::makePatientOrientationFromImageOrientationPatient(const ImageOrientation &imageOrientation)
{
    QString patientOrientationString;
    patientOrientationString = this->mapDirectionCosinesToOrientationString(imageOrientation.getRowVector());
    patientOrientationString += DICOMValueRepresentationConverter::ValuesSeparator;
    patientOrientationString += this->mapDirectionCosinesToOrientationString(imageOrientation.getColumnVector());
    patientOrientationString += DICOMValueRepresentationConverter::ValuesSeparator;
    patientOrientationString += this->mapDirectionCosinesToOrientationString(imageOrientation.getNormalVector());

    if (!setDICOMFormattedPatientOrientation(patientOrientationString))
    {
        DEBUG_LOG("makePatientOrientationFromImageOrientationPatient() ha generat una cadena d'orientació de pacient invàlida");
    }
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

QString PatientOrientation::mapDirectionCosinesToOrientationString(const QVector3D &vector)
{
    char orientation[4];
    char *optr = orientation;
    *optr = '\0';

    char orientationX = vector.x() < 0 ? 'R' : 'L';
    char orientationY = vector.y() < 0 ? 'A' : 'P';
    char orientationZ = vector.z() < 0 ? 'F' : 'H';

    double absX = fabs(vector.x());
    double absY = fabs(vector.y());
    double absZ = fabs(vector.z());

    for (int i = 0; i < 3; ++i)
    {
        if (absX > .0001 && absX > absY && absX > absZ)
        {
            *optr++= orientationX;
            absX = 0;
        }
        else if (absY > .0001 && absY > absX && absY > absZ)
        {
            *optr++= orientationY;
            absY = 0;
        }
        else if (absZ > .0001 && absZ > absX && absZ > absY)
        {
            *optr++= orientationZ;
            absZ = 0;
        }
        else
        {
            break;
        }
        *optr = '\0';
    }
    return QString(orientation);
}

} // End namespace udg
