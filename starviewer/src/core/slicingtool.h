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

#include "changesliceqviewercommand.h"

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
    
    int getMinimum(SlicingMode mode) const;
    int getMaximum(SlicingMode mode) const;
    unsigned int getRangeSize(SlicingMode mode) const;
    int getLocation(SlicingMode mode) const;
    int setLocation(SlicingMode mode, int location, bool dryRun = false);
    int incrementLocation(SlicingMode mode, int shift, bool dryRun = false);
    
    unsigned int getNumberOfAxes() const;
    void setNumberOfAxes(unsigned int numberOfAxes);
    
    SlicingMode getMode(unsigned int axis) const;
    void setMode(unsigned int axis, SlicingMode mode);
    
    int getMinimum(unsigned int axis) const;
    int getMaximum(unsigned int axis) const;
    unsigned int getRangeSize(unsigned int axis) const;
    int getLocation(unsigned int axis) const;
    int setLocation(unsigned int axis, int location, bool dryRun = false);
    int incrementLocation(unsigned int axis, int shift, bool dryRun = false);
    
public slots:
    /// React to major changes that will alter the results of isUsable function, which is precondition to use others. (This is called when volume is changed)
    virtual void reassignAxis() = 0;
    
protected:
    ChangeSliceQViewerCommand::SlicePosition m_volumeInitialPosition;
    Q2DViewer* m_2DViewer;

private:
    QVector<SlicingMode> m_axes;
};

}

#endif //UDGSLICINGTOOL_H
