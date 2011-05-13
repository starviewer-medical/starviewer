#include "orientation.h"

#include <QObject>

namespace udg {

const QString Orientation::LeftLabel("L");
const QString Orientation::RightLabel("R");
const QString Orientation::PosteriorLabel("P");
const QString Orientation::AnteriorLabel("A");
const QString Orientation::HeadLabel("H");
const QString Orientation::FeetLabel("F");

const QString Orientation::getPlaneOrientationLabel(PlaneOrientationType orientation)
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

} // End namespace udg
