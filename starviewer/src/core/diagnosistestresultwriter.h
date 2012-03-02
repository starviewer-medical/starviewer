#ifndef UDGDIAGNOSISTESTRESULTWRITER_H
#define UDGDIAGNOSISTESTRESULTWRITER_H

#include "diagnosistestresult.h"

#include <QList>
#include <QPair>
#include <QString>
#include <QXmlStreamWriter>

class QFile;
class QTextStream;

namespace udg {

class DiagnosisTest;

/**
    Classe encarregada de generar un fitxer de text amb els resultats d'un conjunt de tests de diagnosi.
  */
class DiagnosisTestResultWriter {
public:
    DiagnosisTestResultWriter();

    ~DiagnosisTestResultWriter();

    /// Mètode que crea i escriu un fitxer txt per guardar resultats dels testos de diagnosi.
    void write(const QString &pathFile);

    QString getAsQString();

    /// Afegeix els testos a gravar al fitxer
    void setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult> > diagnosisTests);

protected:
    // Crea el fitxer per poder-hi escriure, si ja existeix l'esborra i el torna a crear perquè quedi net.
    virtual QFile* createFile(const QString &pathFile);
    // Crea el textStream que escriurà a fitxer
    virtual QTextStream* createTextStream(QFile *file);

    /// Genera el codi HTML a partir de la llista passada amb el mètode setDiagnosisTests()
    void generateData();

    /// Mètodes interns per generar el codi HTML
    void writeDTD(QXmlStreamWriter &writer);
    virtual void writeHead(QXmlStreamWriter &writer);
    void writeButtons(QXmlStreamWriter &writer);
    void writeButton(QXmlStreamWriter &writer, const QString &id, const QString &text);
    void writeTests(QXmlStreamWriter &writer);
    void writeTestDescription(QXmlStreamWriter &writer, DiagnosisTestResult::DiagnosisTestResultState state, const QString &description);
    void writeTestProblems(QXmlStreamWriter &writer, const QPair<DiagnosisTest *, DiagnosisTestResult> &test);
    void writeTestProblems(QXmlStreamWriter &writer, const QList<DiagnosisTestProblem> &problems);
    int countDiagnosisTestsOfState(DiagnosisTestResult::DiagnosisTestResultState state);

private:
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_diagnosisTests;
    bool m_modified;
    QString m_generatedData;
};

}

#endif
