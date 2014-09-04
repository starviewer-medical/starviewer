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

#ifndef UDGVOLUMEREADER_H
#define UDGVOLUMEREADER_H

#include <QObject>

#include <QQueue>
#include <QSharedPointer>

namespace udg {

class Postprocessor;
class Volume;
class VolumePixelDataReader;

/**
    Classe encarregada de llegir les dades que han de definir un volum.
    Li passem un punter a Volume i aquesta s'encarrega de llegir i assignar-li la informació adequada.
  */
class VolumeReader : public QObject {
Q_OBJECT
public:
    VolumeReader(QObject *parent = 0);
    ~VolumeReader();

    /// Donat un punter vàlid a Volume, llegeix les dades
    /// corresponents i li assigna.
    virtual void read(Volume *volume);

    /// Llegeix un Volume sense mostrar cap diàleg d'error
    /// TODO: S'hauria d'eliminar i fusionar amb read, ja que no s'hauria de mostrar cap error de tan avall
    /// TODO: Fer que es retornin tipus d'errors i no com ara, que només es retorna true o false i així treure responsabilitat d'ui als readers
    bool readWithoutShowingError(Volume *volume);

    /// Demana que s'aborti el procés iniciat amb read() o readWithoutShowingError() de lectura d'un Volume
    /// Si no s'ha iniciat cap procés de lectura, no farà res. Un cop cridat la lectura retornarà amb error.
    void requestAbort();

    /// Mostra per pantalla un message box informant de l'error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void showMessageBoxWithLastError() const;

    /// Retorna un missatge que es pot mostrar a un usuari amb l'explicació de l'últim error.
    /// Si no hi ha cap "últim error" es retorna un QString buit.
    QString getLastErrorMessageToUser() const;

signals:
    /// Ens indica el progrés del procés de lectura
    /// TODO: De moment quan es vulgui llegir només un fitxer, p.ex. multiframes, mamos, etc. per limitacions de la lectura,
    /// no tindrem cap tipus de progrés.
    void progress(int progress);

private:
    /// Executa el pixel reader i llegeix el volume
    void executePixelDataReader(Volume *volume);

    /// Grava un missatge de warning amb l'ultim error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void logWarningLastError(const QStringList &fileList) const;

    /// Arregla l'spacing en els casos que sabem que les llibreries fallen en aquest càlcul
    void fixSpacingIssues(Volume *volume);

private:

    /// Ens retorna els arxius que hem de llegir, sense retornar-ne cap de repetit
    QStringList getFilesToRead(Volume *volume) const;

    /// Creates and sets up the pixel data reader for the given volume. The second parameter shows progress.
    void setUpReader(Volume *volume);

    /// Treu i executa en ordre els postprocessadors de la cua sobre el volum passat.
    void runPostprocessors(Volume *volume);

private:
    /// Classe amb la qual llegirem les dades d'imatge del volum
    VolumePixelDataReader *m_volumePixelDataReader;

    /// Guardem l'ultim error que s'ha produit al llegir
    int m_lastError;

    /// Cua on guardarem els postprocessadors que s'han d'executar després de llegir el volum.
    QQueue< QSharedPointer<Postprocessor> > m_postprocessorsQueue;

    /// Used to know that abort has been requested before having the pixel data reader.
    bool m_abortRequested;

};

} // End namespace udg

#endif
