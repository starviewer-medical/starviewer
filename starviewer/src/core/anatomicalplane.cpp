#include "anatomicalplane.h"

#include <QObject>

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
} // End namespace udg
