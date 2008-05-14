/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONHANDLER_H
#define UDGEXTENSIONHANDLER_H

#include <QObject>
#include <QString>
#include "patient.h"
#include "extensioncontext.h"

class QProgressDialog;

namespace udg {

// Fordward Declarations
class QApplicationMainWindow;
class PatientFillerInput;
class AppImportFile;

/**
Gestor de mini-aplicacions i serveis de l'aplicació principal

@author Grup de Gràfics de Girona  ( GGG )

*/
class ExtensionHandler : public QObject{
Q_OBJECT
public:
    ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent = 0, QString name = 0);

    ~ExtensionHandler();

public slots:
    /// rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request( int who );
    void request( const QString &who );

    /**
     * Elimina totes les extensions que contingui el workspace i allibera la memòria que
     * ocupa el pacient actual (principalment,els volums del repositori)
     */
    void killBill();

    /**
     * Processa un conjunt d'arxius d'input i els processa per decidir què fer amb aquests, com per exemple
     * crear nous pacient, obrir finestres, afegir les dades al pacient actual, etc
     * @param inputFiles Els arxius a processar, que poden ser del tipus suportat per l'aplicació o no
     * @param defaultStudyUID Estudi que es vol visualitzar per defecte
     * @param defaultSeriesUID Serie que es vol veure per defecte
     * @param defaultImageInstance Imatge que es vol per defecte
     */
    void processInput( QStringList inputFiles, QString defaultStudyUID = QString(), QString  defaultSeriesUID = QString(), QString defaultImageInstance = QString() );

    /**
     * Obrirà l'extensió per defecte. Si no hi ha dades de pacient vàlides, no farà res.
     */
    void openDefaultExtension();

    /**
     * Assigna el contexte de l'extensió
     * @param context contexte
     */
    void setContext( const ExtensionContext &context );

    /**
     * Obtenim el contexte de l'extensió
     * @return El contexte de l'extensió, es pot modificar
     */
    ExtensionContext &getContext();

    /// Actualitza la configuració que se li indica
    //TODO Semi-xapussa per resoldre el problema de comunicació entre el queryscreen i la seva configuració. Cal fer un disseny de tot això.
    //Veure el signal de qconfigurationdialog configurationChanged
    void updateConfiguration(const QString &configuration);

private:
    /// Crea les connexions de signals i slots
    void createConnections();

    /// Afegeix un pacient a la mainwindow tenint en compte si cal fusionar o no i si es pot reemplaçar el pacient actual ja carregat
    void addPatientToWindow(Patient *patient, bool canReplaceActualPatient);

private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;

    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile *m_importFileApp;

    /// Contexte de l'extensió
    ExtensionContext m_extensionContext;
};

};  //  end  namespace udg

#endif
