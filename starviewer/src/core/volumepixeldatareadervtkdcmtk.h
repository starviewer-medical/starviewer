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

#ifndef VOLUMEPIXELDATAREADERVTKDCMTK_H
#define VOLUMEPIXELDATAREADERVTKDCMTK_H

#include "volumepixeldatareader.h"

class vtkEventQtSlotConnect;

namespace udg {

class VtkDcmtkImageReader;

/**
    Volume image data reader that uses VTK and DCMTK to read data.
  */
class VolumePixelDataReaderVTKDCMTK : public VolumePixelDataReader {

    Q_OBJECT

public:

    VolumePixelDataReaderVTKDCMTK(QObject *parent = 0);
    ~VolumePixelDataReaderVTKDCMTK();

    /// Given a list of filenames, it reads the files and fills the internal volume pixel data. Returns an integer to indicate error values.
    virtual int read(const QStringList &filenames);

    /// Requests abortion of the current read operation.
    virtual void requestAbort();

private slots:

    /// Receives the VTK progress event from the reader and emits the Qt progress signal.
    void progressSlot();

private:

    /// VTK-DCMTK reader.
    VtkDcmtkImageReader *m_reader;
    /// Connects VTK events to Qt slots.
    vtkEventQtSlotConnect *m_vtkQtConnections;
    /// True when a read abortion has been requested.
    bool m_abortRequested;

};

} // end namespace udg

#endif // VOLUMEPIXELDATAREADERVTKDCMTK_H
