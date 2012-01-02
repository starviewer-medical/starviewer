#include "diagnosistest.h"
#include "diagnosistestresultwriter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace udg {

DiagnosisTestResultWriter::DiagnosisTestResultWriter()
{
}

DiagnosisTestResultWriter::~DiagnosisTestResultWriter()
{
}

void DiagnosisTestResultWriter::write(const QString &pathFile)
{
    // Per tal de poder utilitzar el Q_FOREACH, hem de redefinir el tipus QPair,
    // ja que conté '<' i '>', i a les macros no els agrada.
    typedef QPair<DiagnosisTest*, DiagnosisTestResult> PairTestResult;

    QFile *file = createFile(pathFile);
    if (!file)
    {
        return;
    }

    QTextStream *writer = createTextStream(file);

    Q_FOREACH(PairTestResult pair, m_diagnosisTests)
    {
        DiagnosisTest *test = pair.first;
        DiagnosisTestResult result = pair.second;
        *writer << "Diagnosis test: ";
        *writer << test->getDescription();
        *writer << "\n";
        *writer << "Result: ";

        switch(result.getState())
        {
            case DiagnosisTestResult::Ok:
                *writer << "Ok\n";
                break;
            case DiagnosisTestResult::Warning:
                *writer << "Warning\n";
                break;
            case DiagnosisTestResult::Error:
                *writer << "Error\n";
                break;
            default:
                *writer << "Invalid\n";
                break;
        }
        *writer << "Description: " << result.getDescription() << "\n";
        *writer << "Solution: " << result.getSolution() << "\n\n";
    }

    delete writer;
    delete file;
}

void DiagnosisTestResultWriter::setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult> > diagnosisTests)
{
    m_diagnosisTests = diagnosisTests;
}

QFile* DiagnosisTestResultWriter::createFile(const QString &pathFile)
{
    if (pathFile.isEmpty() || QFileInfo(pathFile).isDir())
    {
        return NULL;
    }

    QFile *file = new QFile(pathFile);
    if (file->exists())
    {
        // Si ja existeix l'esborrem ja que si és més llarg que el nou, hi quedaran caràcters del vell
        QFile::remove(pathFile);
    }
    if (!file->open(QFile::ReadWrite | QFile::Text))
    {
        return NULL;
    }
    return file;
}

QTextStream* DiagnosisTestResultWriter::createTextStream(QFile *file)
{
    return new QTextStream(file);
}

}
