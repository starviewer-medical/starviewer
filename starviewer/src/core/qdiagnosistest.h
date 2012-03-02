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
    Aquest classe és l'encarregada de realitzar el dicomdump de la imatge que s'està visualizant per part de l'usuari.
    Totes les imatges tenen una sèrie de tags que es descodifiquen en aquesta classe i es visualitzen en forma d'arbre mitjançant un QTreeWidget
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
    RunDiagnosisTest *m_runDiagnosisTest;
    QThread *m_threadRunningDiagnosisTest;

    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_errorExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_okExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_warningExecutedDiagnosisTests;

    DiagnosisTestResultWriter m_diagnosisTestResultWriter;
};

}

#endif
