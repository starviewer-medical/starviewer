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

#ifndef UDGQDIAGNOSISTEST_H
#define UDGQDIAGNOSISTEST_H

#include "ui_qdiagnosistestbase.h"

#include <QList>
#include <QPair>

#include "diagnosistestresult.h"
#include "diagnosistestresultwriter.h"

class QThread;

namespace udg {

class DiagnosisTest;
class RunDiagnosisTest;

/**
 * @brief The QDiagnosisTest class runs all the diagnosis tests and shows their results.
 */
class QDiagnosisTest : public QDialog, private Ui::QDiagnosisTestBase {
Q_OBJECT
public:
    QDiagnosisTest(QWidget *parent = 0);

    ~QDiagnosisTest();

    /// Executa els test i mostra el resultats. No torna d'aquest mètode fins que l'usuari tanca el QDialog
    void execAndRunDiagnosisTest();

signals:
    void start();

private slots:
    //Executa els diagnosisTest
    void runDiagnosisTest();

    /// Actualiza el progress
    void updateRunningDiagnosisTestProgress(DiagnosisTest *diagnosisTest);

    /// Mostra els resultats dels testos executats
    void finishedRunningDiagnosisTest();

    ///Slot que respón quan es click el label ViewTests. Aquest label només és visible si tots els testos han donat ok
    void viewTestsLabelClicked();

    /// Emplena la Taula amb els resultats dels testos, en funció de quins s'ha de mostrar en funció de quins botons estiguin activats (són els botons que indiquen si s'han
    /// de mostrar els testos amb un determinat resultat)
    void fillDiagnosisTestsResultTable();

    /// Guarda els resultats en un fitxer
    void saveDiagnosisTestResultsAsFile();

private:
    /// Crea les connexions
    void createConnections();

    /// Fa que el widget mostri els controls pertinents per executar els testos
    void updateWidgetToRunDiagnosisTest();

    ///Afegeix un diagnosisTest a la taula que els mostra
    void addDiagnosisTestResultToTable(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult);

    /// Indica si el resultat de tots els testos és OK
    bool allDiagnosisTestResultAreOk();

    /// Agrupa i guarda en variables internes de la classe els testos executats agrupats per l'estat del seu DiagnosisTestResult
    void groupDiagnosisTestFromRunDiagnosisTestByState();

    /// Retorna els testos a executar
    QList<DiagnosisTest*> getDiagnosisTestsToRun() const;

private:
    /// This class is used to detect when the scroll bar appears in the list of tests and react accordingly.
    class ScrollBarEventFilter : public QObject
    {   // Q_OBJECT macro not needed
    public:
        explicit ScrollBarEventFilter(QDiagnosisTest *parent);

    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;

    private:
        QDiagnosisTest *m_diagnosisTest;

    };

private:
    RunDiagnosisTest *m_runDiagnosisTest;
    QThread *m_threadRunningDiagnosisTest;

    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_errorExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_okExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_warningExecutedDiagnosisTests;

    DiagnosisTestResultWriter m_diagnosisTestResultWriter;
};

}

#endif
