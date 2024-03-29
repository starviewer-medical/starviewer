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

#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "ui_queryscreenbase.h"

namespace udg {

class DicomMask;
class QCreateDicomdir;
class StatsWatcher;
class StudyOperationResult;

/**
    Aquesta classe crea la interfície princial de cerca, i connecta amb el PACS i la bd dades local per donar els resultats finals
  */
class QueryScreen : public QDialog, private Ui::QueryScreenBase {
Q_OBJECT
public:
    explicit QueryScreen(QWidget *parent = nullptr);
    ~QueryScreen() override;

public slots:
    /// Obre un dicomdir
    void openDicomdir();

    /// Reacts to certain messages from MessageBus.
    void onMessage(const QString &key, const QVariant &value);

    /// Si la finestra no és visible o està radera d'una altra, la fa visible i la porta al davant de les finestres.
    void bringToFront();

    /// Mostra la pestanya de PACS.
    /// TODO Deixar els camps de cerca com estaven, fer un clear o posar valors per defecte?
    /// Es podria passar un paràmetre "bool clear" amb valor per defecte (false, per exemple)
    void showPACSTab();

    /// Mostra tots els estudis en local i reseteja els camps de cerca
    void showLocalExams();

    /// Neteja els LineEdit del formulari
    void clearTexts();

    /// Es comunica amb el widget de la base de dades i visualitzar un estudi descarregat del PACS
    void viewStudyFromDatabase(QString studyInstanceUID);

    /// Demana que es carregui un estudi descarregat. Útil per casos com el de carregar prèvies, per exemple.
    void loadStudyFromDatabase(QString studyInstanceUID);

signals:
    /// Signal que ens indica quins pacients s'han seleccionat per visualitzar
    /// Afegim un segon paràmetre per indicar si aquests pacients s'han de carregar únicament i si s'han de visualitzar
    void selectedPatients(QList<Patient*> selectedPatients, bool loadOnly = false);

    /// S'emet quan la finestra es tanca
    void closed();

protected:
    ///  Event que s'activa al tancar al rebren un event de tancament
    ///  @param event de tancament
    void closeEvent(QCloseEvent *event) override;

private slots:
    /// Escull a on fer la cerca, si a nivell local o PACS
    void searchStudy();

    /// Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
    /// @param index del tab al que s'ha canviat
    void refreshTab(int index);

    /// Mostra/amaga els camps de cerca avançats
    void setAdvancedSearchVisible(bool visible);

#ifndef STARVIEWER_LITE
    /// Mostra la pantalla QOperationStateScreen
    void showOperationStateScreen();
#endif

    /// Notifica quins estudis s'han escollit per carregar i/o veure
    /// Afegim un segon paràmetre per indicar si volem fer view o únicament carregar les dades.
    void viewPatients(QList<Patient*>, bool loadOnly = false);

    /// Called when a retrieve or store operation is requested from anywhere. It starts the animation.
    void onPacsOperationRequested(StudyOperationResult *result);

    /// Called when an operation has finished. If it is the last operation stops the animation.
    void onPacsOperationFinishedOrCancelled();

    /// Actualitza segons el tab en el que ens trobem la visibilitat del llistat de PACS
    /// El llistat només es podrà habilitar o deshabilitar quan estem en la pestanya PACS
    void updatePACSNodesVisibility();

private:
#ifdef STARVIEWER_LITE
    // Al fer remove de la pestanya del pacs es canvia el index de cada tab, per això hem de redefinir-lo pel cas de StarviewerLite
    enum TabType { PACSQueryTab = -1, LocalDataBaseTab = 0, DICOMDIRTab = 1 };
#else
    enum TabType { LocalDataBaseTab = 0, PACSQueryTab = 1, DICOMDIRTab = 2 };
#endif

    /// Connecta els signals i slots pertinents
    void createConnections();

    /// Construeix la màscara d'entrada pels dicom a partir dels widgets de cerca
    /// @return retorna la màscara d'un objecte dicom
    DicomMask buildDicomMask();

    /// Comprova els requeriments necessaris per poder utilitzar la QueryScreen
    void checkRequirements();

    /// Es comprova la integritat de la base de dades i les imatges, comprovant que la última vegada l'starviewer
    /// no s'hagués tancat amb un estudi a mig baixar, i si és així esborra l'estudi a mig descarregar i deixa la base de dades en un estat integre
    void checkDatabaseImageIntegrity();

    /// Comprova que el port pel qual es reben els objectes dicom a descarregar no estigui sent utitlitzat
    /// per cap altre aplicació, si és aixì donar una missatge d'error
    void checkIncomingConnectionsPacsPortNotInUse();

    /// Inicialitza les variables necessaries, es cridat pel constructor
    void initialize();

    /// Llegeix i aplica dades de configuració
    void readSettings();

    /// Guarda els settings de la QueryScreen
    void writeSettings();

private:
    QCreateDicomdir *m_qcreateDicomdir;

    StatsWatcher *m_statsWatcher;

    /// Indica quans jobs tenim pendents de finalitzar (s'estan esperant per executar o s'estan executant)
    int m_pacsOperationsRunning;
};

};

#endif
