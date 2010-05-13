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
#include <QList>
#include "patient.h"
#include "extensioncontext.h"

namespace udg {

// Fordward Declarations
class QApplicationMainWindow;
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
    bool request( const QString &who );

    /**
     * Elimina totes les extensions que contingui el workspace i allibera la memòria que
     * ocupa el pacient actual (principalment,els volums del repositori)
     */
    void killBill();

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

private slots:
    /**
     * Processa un conjunt d'arxius d'input i els processa per decidir què fer amb aquests, com per exemple
     * crear nous pacient, obrir finestres, afegir les dades al pacient actual, etc
     * @param inputFiles Els arxius a processar, que poden ser del tipus suportat per l'aplicació o no
     */
    void processInput(const QStringList &inputFiles);

    /**
     * Donada una llista de pacients d'entrada, s'encarrega de posar a punt
     * aquests i assignar-lis la finestra adequada, decidint si obrir noves finestres
     * i/o fusionar o matxacar el pacient actual d'aquesta finestra.
     * La llista s'assumeix que no té pacients iguals separats, és a dir,
     * s'assumeix que cada entrada correspon a un pacient diferent, amb un o varis estudis
     * Afegim un segon paràmetre que ens indica si els pacients a processar només cal carregar-los o fer-ne un "view"
     */
    void processInput(QList<Patient *> patientsList, bool loadOnly = false );

private:
    /// Crea les connexions de signals i slots
    void createConnections();

    /// Afegeix un pacient a la mainwindow tenint en compte si cal fusionar o no i si es pot reemplaçar el pacient actual ja carregat
    /// Afegim un segon paràmetre que ens indica si els pacients a processar només cal carregar-los o fer-ne un "view"
    void addPatientToWindow(Patient *patient, bool canReplaceActualPatient, bool loadOnly = false );

    /// Processa el pacient donat per tal que pugui ser usat per les extensions
    //  Li crea els volums al repositori i assigna quina és la sèrie per defecte
    void generatePatientVolumes(Patient *patient, const QString &defaultSeriesUID);

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
