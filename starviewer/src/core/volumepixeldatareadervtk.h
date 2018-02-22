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

#ifndef VOLUMEPIXELDATAREADERVTK_H
#define VOLUMEPIXELDATAREADERVTK_H

#include "volumepixeldatareader.h"

class vtkImageReader2;

namespace udg {

/**
 *  @brief The VolumePixelDataReaderVtk class uses only VTK to read volume pixel data.
 */
class VolumePixelDataReaderVtk : public VolumePixelDataReader {

    Q_OBJECT

public:
    explicit VolumePixelDataReaderVtk(QObject *parent = nullptr);
    virtual ~VolumePixelDataReaderVtk();

    /// Given a list of filenames, it reads the files and fills the internal volume pixel data. Returns an integer to indicate error values.
    int read(const QStringList &filenames) override;

    /// Requests abortion of the current read operation.
    void requestAbort() override;

private slots:
    /// Receives the VTK progress event from the reader and emits the Qt progress signal.
    void progressSlot();

private:
    /// The reader.
    vtkImageReader2 *m_reader;
    /// True when a read abortion has been requested.
    bool m_abortRequested;

};

} // namespace udg

#endif // VOLUMEPIXELDATAREADERVTK_H
