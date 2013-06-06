#ifndef UDGGENERICDISTANCETOOL_H
#define UDGGENERICDISTANCETOOL_H

#include "measurementtool.h"

#include <QPointer>

namespace udg {

class DrawerLine;

/**
    Abstract class to be superclass of distance-kind tools
 */
class GenericDistanceTool : public MeasurementTool {
Q_OBJECT
public:
    GenericDistanceTool(QViewer *viewer, QObject *parent = 0);
    ~GenericDistanceTool();

    void handleEvent(long unsigned eventID);

protected:
    /// These methods should be implemented by its subclasses giving the desired behaviour for each event
    virtual void handleLeftButtonPress() = 0;
    virtual void handleMouseMove() = 0;
    virtual void handleKeyPress() = 0;

protected:
    /// DrawerLine that holds the drawn distance
    QPointer<DrawerLine> m_distanceLine;
};

} // End namespace udg

#endif
