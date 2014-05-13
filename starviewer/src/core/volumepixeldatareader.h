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

#ifndef UDGVOLUMEPIXELDATAREADER_H
#define UDGVOLUMEPIXELDATAREADER_H

#include <QObject>

namespace udg {

class VolumePixelData;

/**
    Classe encarregada de llegir únicament les dades de píxel d'arxius d'imatge mèdica.
    Aquesta classe únicament actuarà d'interfície pels diferents possibles motors de lectura
    que podem tenir (itk, gdcm, dcmtk, etc).
    Les dades es retornaran en format vtkImageData.
  */
class VolumePixelDataReader : public QObject {
Q_OBJECT
public:
    /// Tipus d'error que podem tenir al llegir
    enum PixelDataReadError { NoError = 1, SizeMismatch, InvalidFileName, MissingFile, OutOfMemory,
                              ZeroSpacingNotAllowed, CannotReadFile, ReadAborted, UnknownError };

    VolumePixelDataReader(QObject *parent = 0);
    ~VolumePixelDataReader();

    /// Sets the list of frame numbers in the order they must be read from a multiframe file.
    void setFrameNumbers(const QList<int> &frameNumbers);

    /// Donada una llista de noms de fitxer, la llegeix i omple
    /// l'estructura d'imatge que fem servir internament.
    /// Ens retorna un enter que ens indicarà si hi ha hagut alguna mena d'error en el
    /// procés de lectura, seguint els valors enumerats definits a aquesta mateixa classe
    virtual int read(const QStringList &filenames) = 0;

    /// Mètode que cal reimplementar per tal de permètre cancel·lar una petició de lectura iniciada amb read().
    /// En el cas de que no estigui realitzant un read, no farà res.
    /// En el cas que estigui en mig d'una operació de lectura, caldrà retornar com a resultat un error ReadAborted.
    virtual void requestAbort() = 0;

    /// Ens retorna les dades llegides
    VolumePixelData* getVolumePixelData();

signals:
    /// Ens indica el progrés del procés de lectura
    void progress(int progress);

protected:
    /// List of frame numbers in the order they must be read from a multiframe file. Can be ignored for single-frame files.
    QList<int> m_frameNumbers;

    /// Les dades d'imatge en format vtk
    VolumePixelData *m_volumePixelData;

};

} // End namespace udg

#endif
