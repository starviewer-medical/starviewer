/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMDIRBURNINGAPPLICATION_H
#define UDGDICOMDIRBURNINGAPPLICATION_H

#include <QString>

#include "createdicomdir.h" // Per utilitzar CreateDicomdir::recordDeviceDicomDir, per tal de conèixer si es vol gravar en CD o DVD

namespace udg {

/** Crea un arxiu d'imatge ISO a partir d'un directori o fitxer.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMDIRBurningApplication {

public:

    enum DICOMDIRBurningApplicationError { IsoPathNotFound, BurnApplicationPathNotFound, InternalError };

    DICOMDIRBurningApplication();
    DICOMDIRBurningApplication(const QString &isoPath, const CreateDicomdir::recordDeviceDicomDir &currentDevice);
    ~DICOMDIRBurningApplication();

    /// Estableix el path del fitxer iso que es vol gravar en CD o DVD
    void setIsoPath(const QString &isoPath);

    /// Retorna el path del fitxer iso que es vol gravar en CD o DVD
    QString getIsoPath() const;

    /// Estableix si es vol gravar en CD o DVD
    void setCurrentDevice(const CreateDicomdir::recordDeviceDicomDir &currentDevice);

    /// Retorna si es vol gravar en CD o DVD
    CreateDicomdir::recordDeviceDicomDir getCurrentDevice() const;

    /// Retorna la descripció de l'últim error que s'ha produit
    QString getLastErrorDescription() const;

    /// Retorna l'últim error que s'ha produit
    DICOMDIRBurningApplicationError getLastError() const;

    /** Grava el fitxer iso que es troba al path especificat a l'atribut m_isoPath en un CD o DVD
     * @return true si el fitxer iso s'ha gravat correctament i false en cas contrari
     */
    bool burnIsoImageFile();

private:

    /// Path del fitxer iso que es vol gravar en CD o DVD
    QString m_isoPath;

    /// Variable que ens permet diferenciar si es vol gravar en CD o DVD
    CreateDicomdir::recordDeviceDicomDir m_currentDevice;

    /// Descripció de l'últim error que s'ha produit
    QString m_lastErrorDescription;

    /// Últim error que s'ha produit
    DICOMDIRBurningApplicationError m_lastError;
};

}

#endif
