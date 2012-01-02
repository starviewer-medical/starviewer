#ifndef UDGDIAGNOSISTESTRESULTWRITER_H
#define UDGDIAGNOSISTESTRESULTWRITER_H

#include "diagnosistestresult.h"

#include <QList>
#include <QPair>
#include <QString>

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

    /// Afegeix els testos a gravar al fitxer
    void setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult> > diagnosisTests);

protected:
    // Crea el fitxer per poder-hi escriure, si ja existeix l'esborra i el torna a crear perquè quedi net.
    virtual QFile* createFile(const QString &pathFile);
    // Crea el textStream que escriurà a fitxer
    virtual QTextStream* createTextStream(QFile *file);

private:
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_diagnosisTests;
};

}

#endif
