#include "patientorientation.h"

namespace udg {

const QString PatientOrientation::LeftLabel("L");
const QString PatientOrientation::RightLabel("R");
const QString PatientOrientation::PosteriorLabel("P");
const QString PatientOrientation::AnteriorLabel("A");
const QString PatientOrientation::HeadLabel("H");
const QString PatientOrientation::FeetLabel("F");

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

} // End namespace udg
