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

#ifndef UDGEXTENSIONHANDLER_H
#define UDGEXTENSIONHANDLER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMutex>

#include "extensioncontext.h"
#include "appimportfile.h"

namespace udg {

// Fordward Declarations
class QApplicationMainWindow;

/**
    Gestor de mini-aplicacions i serveis de l'aplicació principal
  */
class ExtensionHandler : public QObject {
Q_OBJECT
public:
    ExtensionHandler(QApplicationMainWindow *mainApp, QObject *parent = 0);

    ~ExtensionHandler();

public slots:
    /// Rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request(int who);
    bool request(const QString &who);

    /// Obrirà l'extensió per defecte. Si no hi ha dades de pacient vàlides, no farà res.
    void openDefaultExtension();

    /// Assigna el contexte de l'extensió
    /// @param context contexte
    void setContext(const ExtensionContext &context);

    /// Obtenim el contexte de l'extensió
    /// @return El contexte de l'extensió, es pot modificar
    ExtensionContext& getContext();

private slots:
    /// Processa un conjunt d'arxius d'input i els processa per decidir què fer amb aquests, com per exemple
    /// crear nous pacient, obrir finestres, afegir les dades al pacient actual, etc
    /// @param inputFiles Els arxius a processar, que poden ser del tipus suportat per l'aplicació o no
    void processInput(const QStringList &inputFiles);

    /// Donada una llista de pacients d'entrada, s'encarrega de posar a punt
    /// aquests i assignar-lis la finestra adequada, decidint si obrir noves finestres
    /// i/o fusionar o matxacar el pacient actual d'aquesta finestra.
    /// La llista s'assumeix que no té pacients iguals separats, és a dir,
    /// s'assumeix que cada entrada correspon a un pacient diferent, amb un o varis estudis
    /// Afegim un segon paràmetre que ens indica si els pacients a processar només cal carregar-los o fer-ne un "view"
    void processInput(QList<Patient*> patientsList, bool loadOnly = false);

    /// Es cridarà quan la QueryScreen es tanqui
    void queryScreenIsClosed();

private:
    /// Crea les connexions de signals i slots
    void createConnections();

    /// Afegeix un pacient a una mainwindow tenint en compte si cal fusionar o no i si es pot reemplaçar el pacient actual ja carregat
    /// Afegim un segon paràmetre que ens indica si els pacients a processar només cal carregar-los o fer-ne un "view"
    /// Retorna la mainwindow a on s'ha afegit el pacient.
    QApplicationMainWindow* addPatientToWindow(Patient *patient, bool canReplaceActualPatient, bool loadOnly = false);

    /// Processa el pacient donat per tal que pugui ser usat per les extensions
    //  Li crea els volums al repositori i assigna quina és la sèrie per defecte
    void generatePatientVolumes(Patient *patient, const QString &defaultSeriesUID);

    /// Scans the list and returns a list where the patients which are the same are merged.
    /// Patients are only considered to be the same if Patient::compareTo() returns SamePatients value.
    QList<Patient*> mergePatients(const QList<Patient*> &patientList);

private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;

    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile m_importFileApp;

    /// Contexte de l'extensió
    ExtensionContext m_extensionContext;

    /// Indica si a aquesta finestra li pertoca o no tancar la QueryScreen
    bool m_haveToCloseQueryScreen;

    /// Mutex to avoid concurrent access to the patient comparer singleton.
    QMutex m_patientComparerMutex;

};

};  // end namespace udg

#endif
