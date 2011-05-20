#ifndef UDGVOLUMEPIXELDATAREADERVTKGDCM_H
#define UDGVOLUMEPIXELDATAREADERVTKGDCM_H

#include "volumepixeldatareader.h"

class vtkGDCMImageReader;
class vtkEventQtSlotConnect;

namespace udg {

/**
    Lector de dades d'imatge per Volume.
    Aquest lector fa ús de la interfície vtkGDCM per llegir les dades.
  */
class VolumePixelDataReaderVTKGDCM : public VolumePixelDataReader {
Q_OBJECT
public:
    VolumePixelDataReaderVTKGDCM(QObject *parent = 0);
    ~VolumePixelDataReaderVTKGDCM();

    int read(const QStringList &filenames);

    virtual void requestAbort();

private:
    /// Un cop llegides les dades, les processa segons l'espai de color
    /// en que estiguin definides i les assigna l'objecte vtkImageData
    // TODO Potser aquest processament s'hauria de fer al corresponent visor i no aquí?
    void applyColorProcessing();

    /// Fa DEBUG_LOGs de certa informació que pot resultar útil per debuggar un cop s'han llegit les dades
    void printDebugInfo();

private slots:
    /// Captura el senyal de vtk perquè poguem notificar el progrés de la lectura
    void slotProgress();

private:
    // Lector vtkGDCM + progress
    vtkGDCMImageReader *m_vtkGDCMReader;
    vtkEventQtSlotConnect *m_vtkQtConnections;
    // Ens serveix per saber si s'ha demanat un abort, ja que vtk no retorna cap informacio al respecte
    bool m_abortRequested;
};

} // End namespace udg

#endif
