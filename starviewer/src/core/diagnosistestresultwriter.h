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
    void write();
    
    /// Afegeix la parella de DiagnosisTest* i DiagnosisTestResult a la llista de tests que s'escriuran a fitxer
    void addDiagnosisTest(DiagnosisTest *diagnosisTest, const DiagnosisTestResult &diagnosisTestResult);

    /// Establir el path i nom del fitxer
    void setPath(const QString &path);

protected:
    // Crea el fitxer per poder-hi escriure, si ja existeix l'esborra i el torna a crear perquè quedi net.
    virtual QFile* createFile();
    // Crea el textStream que escriurà a fitxer
    virtual QTextStream* createTextStream(QFile *file);

private:
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > m_diagnosisTests;
    QString m_path;
};

}

#endif
