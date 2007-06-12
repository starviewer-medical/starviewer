/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGAPPIMPORTFILE_H
#define UDGAPPIMPORTFILE_H

#include <QObject>
#include <QString>

#include "identifier.h"

class QProgressDialog;

namespace udg {

// Fordward Declarations
class VolumeRepository;
class Input;

/**
Mini-aplicació encarregada de permetre carregar un model des del sistema de fitxers al repositori de volums

@author Grup de Gràfics de Girona  ( GGG )
*/
class AppImportFile : public QObject{
Q_OBJECT
public:

    AppImportFile(QObject *parent = 0, QString name = 0);
    ~AppImportFile();

    /// Obre el diàleg per poder obrir arxius, retornant true si s'ha obert una imatge, false si no
    bool open();

    /// Obre el diàleg per poder obrir arxius d'un directori DICOM
    bool openDirectory();

    /// Retorna l'id de l'últim volum que ha depositat al repositori
    Identifier getVolumeIdentifier(){ return m_volumeID; }

    /// fa la feina 'bruta' d'obrir l'arxiu
    int loadFile( QString fileName );

    /// fa la feina 'bruta' d'obrir un directori d'arxius dicom
    int loadDirectory( QString directoryName );

    /// Ens retorna el nom de l'últim arxiu que s'ha obert amb èxit
    QString getLastOpenedFilename();

signals:
    /// Signal que s'emet quan s'escull obrir un Key Image Note. Es passa el path d'aquest
    void openKeyImageNote( const QString & path);

    /// Signal que s'emet quan s'escull obrir un Presentation State. Es passa el path d'aquest
    void openPresentationState( const QString & path);


private:
    /// Directori de treball per fitxers ordinaris
    QString m_workingDirectory;

    /// Directori de treball per directoris dicom
    QString m_workingDicomDirectory;

    /// Nom de l'últim arxiu que s'ha obert amb èxit
    QString m_lastOpenedFilename;

    /// llegeix escriu configuracions
    void readSettings();
    void writeSettings();

    /// Mostra el progrés al carregar un arxiu
    QProgressDialog *m_progressDialog;

    // :::::::::::::::::::::::::::::::::::::::::
    // Recursos
    // :::::::::::::::::::::::::::::::::::::::::

    /// L'id del volum amb el que estem treballant
    Identifier m_volumeID;

    /// El repository de volums
    udg::VolumeRepository* m_volumeRepository;

    /// accés a l'entrada de dades -> això hauria de formar part d'una mini-app, per tant és temporal
    Input *m_inputReader;
};

};  //  end  namespace udg

#endif
