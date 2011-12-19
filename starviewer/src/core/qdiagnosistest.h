#ifndef UDGQDIAGNOSISTEST_H
#define UDGQDIAGNOSISTEST_H

#include "ui_qdiagnosistestbase.h"

#include <QList>
#include <QPair>

namespace udg {

class DiagnosisTest;
class DiagnosisTestResult;

/**
    Aquest classe és l'encarregada de realitzar el dicomdump de la imatge que s'està visualizant per part de l'usuari.
    Totes les imatges tenen una sèrie de tags que es descodifiquen en aquesta classe i es visualitzen en forma d'arbre mitjançant un QTreeWidget
  */
class QDiagnosisTest : public QDialog, private Ui::QDiagnosisTestBase {
Q_OBJECT
public:
    QDiagnosisTest(QWidget *parent = 0);

private slots:
    ///Slot que indinica que un QDiagnosisTestResultWidget de la taula ha canviat de mida
    void qdiagnosisTestResultWidgetResized();

    //Executa els diagnosisTest
    void runDiagnosisTest();

    /// Emplena la Taula amb els resultats dels testos, en funció de quins s'ha de mostrar en funció de quins botons estiguin activats (són els botons que indiquen si s'han
    /// de mostrar els testos amb un determinat resultat)
    void fillDiagnosisTestsResultTable();

    ///Slot que respón quan es click el label ViewTests. Aquest label només és visible si tots els testos han donat ok
    void viewTestsLabelClicked();

private:
    /// Crea les connexions
    void createConnections();

    /// Fa que el widget mostri els controls pertinents per executar els testos
    void updateWidgetToRunDiagnosisTest();

    ///Mostra els resultats del Test, si tots han anat bé mostra un OK, si algun ha fallat mostra un llistat amb els testos
    void showResults();

    ///Afegeix un diagnosisTest a la taula que els mostra
    void addDiagnosisTestResultToTable(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult);

    /// Indica si el resultat de tots els testos és OK
    bool allDiagnosisTestResultAreOk();

    /// Retorna els testos a executar
    QList<DiagnosisTest*> getDiagnosisTestsToRun() const;

    /// Retorna els testos que s'han de mostrar en el llistat en funció de quins botons estiguin activats (són els botons que indiquen si s'han
    /// de mostrar els testos amb un determinat resultat)
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > getDiagnosisTestsToShowInDiagnosisTestsResultTable();

private:
    ///Guardem en una llista de QPair el DiagnosisTest executat i el seu resultat
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > m_okExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > m_errorExecutedDiagnosisTests;
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > m_warningExecutedDiagnosisTests;
};

}

#endif
