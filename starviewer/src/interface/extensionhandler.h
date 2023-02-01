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

#include "extensioncontext.h"
#include "appimportfile.h"

namespace udg {

class QApplicationMainWindow;
class QueryScreen;

/**
    Gestor de mini-aplicacions i serveis de l'aplicació principal
  */
class ExtensionHandler : public QObject {
    Q_OBJECT

public:
    enum class Request { LocalDatabase, Pacs, Dicomdir, OpenFiles, OpenDirectory };

    ExtensionHandler(QApplicationMainWindow *mainApp, QObject *parent = 0);

    ~ExtensionHandler();

    /// Returns the query screen.
    QueryScreen* getQueryScreen() const;

public slots:
    /// Rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request(Request request);
    bool request(const QString &who);

    /// Opens the default extensions for the studies in the patient in the current context.
    /// If the optional \a newStudies is provided, it opens the default extensions for those studies.
    void openDefaultExtensions(QList<Study*> newStudies = {});

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

private:
    /// Crea les connexions de signals i slots
    void createConnections();

    /// Merges the given \a patient into the current patient of this ExtensionHandler's window.
    /// Also displays the new studies unless \a loadOnly is true, in which case it only makes them selectable.
    void mergeIntoCurrentPatient(Patient *patient, bool loadOnly = false);

    /// Scans the list and returns a list where the patients which are the same are merged.
    /// Patients are only considered to be the same if Patient::compareTo() returns SamePatients value.
    QList<Patient*> mergePatients(const QList<Patient*> &patientList);

private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;

    /// QueryScreen instance for the owning main window.
    QueryScreen *m_queryScreen;

    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile m_importFileApp;

    /// Contexte de l'extensió
    ExtensionContext m_extensionContext;
};

};  // end namespace udg

#endif
