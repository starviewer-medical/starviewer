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

#ifndef UDGSLICINGTOOL_H
#define UDGSLICINGTOOL_H

#include "tool.h"

// Qt
#include <QVector>

namespace udg {

class Q2DViewer;

class SlicingTool : public Tool {
    Q_OBJECT
public:
    enum class SlicingMode {None, Slice, Phase, Volume };

    explicit SlicingTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingTool();
    
    double getMinimum(SlicingMode mode) const;
    double getMaximum(SlicingMode mode) const;
    double getRangeSize(SlicingMode mode) const;
    double getLocation(SlicingMode mode) const;
    double setLocation(SlicingMode mode, double location);
    double incrementLocation(SlicingMode mode, double shift);
    
    unsigned int getNumberOfAxes() const;
    void setNumberOfAxes(unsigned int numberOfAxes);
    
    SlicingMode getMode(unsigned int axis) const;
    void setMode(unsigned int axis, SlicingMode mode);
    
    double getMinimum(unsigned int axis) const;
    double getMaximum(unsigned int axis) const;
    double getRangeSize(unsigned int axis) const;
    double getLocation(unsigned int axis) const;
    double setLocation(unsigned int axis, double location);
    double incrementLocation(unsigned int axis, double shift);
    
public slots:
    /// React to major changes that will alter the results of isUsable function, which is precondition to use others. (This is called when volume is changed)
    virtual void reassignAxis() = 0;
    
protected:
    bool m_volumeInitialPositionToMaximum;
    Q2DViewer* m_2DViewer;

private:
    QVector<SlicingMode> m_axes;
};

}

#endif //UDGSLICINGTOOL_H
