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

// Espai reservat pels fwd decl de les mini-apps
class AppImportFile;

// Fi de l'Espai reservat pels fwd decl de les mini-apps
class QueryScreen;
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

private:
    /// Crea les connexions de signals i slots
    void createConnections();

private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;

    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile *m_importFileApp;

    /// La pantalla d'accés al pacs
    QueryScreen *m_queryScreen;

    /// Contexte de l'extensió
    ExtensionContext m_extensionContext;
};

};  //  end  namespace udg

#endif
