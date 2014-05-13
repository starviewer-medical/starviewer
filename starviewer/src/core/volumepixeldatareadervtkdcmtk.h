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
