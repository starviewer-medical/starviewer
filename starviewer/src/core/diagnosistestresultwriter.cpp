#include "diagnosistest.h"
#include "diagnosistestresultwriter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QRegExp>

namespace udg {

DiagnosisTestResultWriter::DiagnosisTestResultWriter()
{
    m_modified = true;
    m_generatedData = "";
}

DiagnosisTestResultWriter::~DiagnosisTestResultWriter()
{
}

QString DiagnosisTestResultWriter::getAsQString()
{
    if (m_modified)
    {
        generateData();
    }

    return m_generatedData;
}

void DiagnosisTestResultWriter::write(const QString &pathFile)
{
    if (m_modified)
    {
        generateData();
    }

    // Per tal de poder utilitzar el Q_FOREACH, hem de redefinir el tipus QPair,
    // ja que conté '<' i '>', i a les macros no els agrada.
    typedef QPair<DiagnosisTest*, DiagnosisTestResult> PairTestResult;

    QFile *file = createFile(pathFile);
    if (!file)
    {
        return;
    }

    QTextStream *writer = createTextStream(file);
    (*writer) << m_generatedData;

    delete writer;
    delete file;
}

void DiagnosisTestResultWriter::setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult> > diagnosisTests)
{
    m_diagnosisTests = diagnosisTests;
    m_modified = true;
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

void DiagnosisTestResultWriter::generateData()
{
    QXmlStreamWriter writer(&m_generatedData);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writeDTD(writer);
    writer.writeStartElement("html");

    writeHead(writer);

    writer.writeStartElement("body");
    writeButtons(writer);
    writeTests(writer);

    //end body
    writer.writeEndElement();
    //end html
    writer.writeEndElement();

    writer.writeEndDocument();

    m_modified = false;
}

void DiagnosisTestResultWriter::writeDTD(QXmlStreamWriter &writer)
{
    writer.writeDTD("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">");
}

void DiagnosisTestResultWriter::writeHead(QXmlStreamWriter &writer)
{
    writer.writeStartElement("head");

    writer.writeStartElement("script");
    writer.writeAttribute("src", "qrc:///javascript/jquery-min.js");
    writer.writeAttribute("type", "text/javascript");
    writer.writeCharacters(" ");
    writer.writeEndElement();

    QRegExp minimize("\\s*\n\\s*");
    QFile jsFile(":/javascript/qdiagnosistest.js");
    if (!jsFile.open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }
    writer.writeStartElement("script");
    writer.writeAttribute("type", "text/javascript");
    writer.writeCharacters(QString(jsFile.readAll()).remove(minimize));
    writer.writeEndElement();

    QFile cssFile(":/css/qdiagnosistest.css");
    if (!cssFile.open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }
    writer.writeStartElement("style");
    writer.writeAttribute("type", "text/css");
    writer.writeCharacters(QString(cssFile.readAll()).remove(minimize));
    writer.writeEndElement();
    writer.writeEndElement();
}

void DiagnosisTestResultWriter::writeButtons(QXmlStreamWriter &writer)
{
    writer.writeStartElement("div");
    writer.writeAttribute("class", "buttons");
    writeButton(writer, "succeededButton", QObject::tr("%1 succeeded").arg(countDiagnosisTestsOfState(DiagnosisTestResult::Ok)));
    writeButton(writer, "errorButton", QObject::tr("%1 errors").arg(countDiagnosisTestsOfState(DiagnosisTestResult::Error)));
    writeButton(writer, "warningButton", QObject::tr("%1 warnings").arg(countDiagnosisTestsOfState(DiagnosisTestResult::Warning)));
    // end div
    writer.writeEndElement();
}

void DiagnosisTestResultWriter::writeButton(QXmlStreamWriter &writer, const QString &id, const QString &text)
{
    writer.writeStartElement("span");
    writer.writeAttribute("id", id);
    writer.writeAttribute("class", "button");
    writer.writeStartElement("span");
    writer.writeAttribute("class", "buttonDescription");
    writer.writeCharacters(text);
    writer.writeEndElement();
    writer.writeEndElement();
}

void DiagnosisTestResultWriter::writeTests(QXmlStreamWriter &writer)
{
    typedef QPair<DiagnosisTest *, DiagnosisTestResult> Pair;

    //div tests
    writer.writeStartElement("div");
    writer.writeAttribute("class", "tests");
    // Per cada test
    foreach (const Pair &pair, m_diagnosisTests)
    {
        writeTestDescription(writer, pair.second.getState(), pair.first->getDescription());
        writeTestProblems(writer, pair);

        writer.writeEndElement();
    }
    //end div tests
    writer.writeEndElement();
}

void DiagnosisTestResultWriter::writeTestDescription(QXmlStreamWriter &writer, DiagnosisTestResult::DiagnosisTestResultState state, const QString &description)
{
    writer.writeStartElement("div");
    if (state == DiagnosisTestResult::Ok)
    {
        writer.writeAttribute("class", "succeededTest");
    }
    else if (state == DiagnosisTestResult::Error)
    {
        writer.writeAttribute("class", "errorTest");
    }
    else
    {
        writer.writeAttribute("class", "warningTest");
    }

    writer.writeStartElement("div");
    writer.writeAttribute("class", "result");
    writer.writeStartElement("div");
    writer.writeAttribute("class", "description");
    writer.writeCharacters(description);
    writer.writeEndElement();
    writer.writeEndElement();
}

void DiagnosisTestResultWriter::writeTestProblems(QXmlStreamWriter &writer, const QPair<DiagnosisTest *, DiagnosisTestResult> &test)
{
    if (test.second.getState() != DiagnosisTestResult::Ok)
    {
        writer.writeStartElement("div");
        writer.writeAttribute("class", "problems");

        writer.writeStartElement("ul");
        writer.writeAttribute("class", "error");
        writeTestProblems(writer, test.second.getErrors());
        writer.writeEndElement();

        writer.writeStartElement("ul");
        writer.writeAttribute("class", "warning");
        writeTestProblems(writer, test.second.getWarnings());
        writer.writeEndElement();

        writer.writeEndElement();
    }
}

void DiagnosisTestResultWriter::writeTestProblems(QXmlStreamWriter &writer, const QList<DiagnosisTestProblem> &problems)
{
    foreach (const DiagnosisTestProblem &problem, problems)
    {
        writer.writeStartElement("li");
        writer.writeTextElement("strong", problem.getDescription());
        writer.writeEmptyElement("br");
        writer.writeCharacters(problem.getSolution());
        writer.writeEndElement();
    }
}

int DiagnosisTestResultWriter::countDiagnosisTestsOfState(DiagnosisTestResult::DiagnosisTestResultState state)
{
    typedef QPair<DiagnosisTest*, DiagnosisTestResult> Pair;
    int count = 0;
    foreach(const Pair &test, m_diagnosisTests)
    {
        if (test.second.getState() == state)
        {
            count++;
        }
    }
    return count;
}

}
