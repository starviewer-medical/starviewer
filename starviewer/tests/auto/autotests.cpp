#include "autotest.h"
#include "gtest/gtest.h"

#include <QDir>

/// A part dels paràmetres que QTest defineix de cada test, se n'han afegint més:
///     -saveOutputToDir <dirPath>: guarda l'output de cada testsuite a un fitxer del format <nomDelTest>.txt.
///                                 Requeriments: el directori NO ha d'existir o ha d'estar buit.
///     -testsToExecute string1 [string2 ... stringN]: executarà només els testos que el seu nom contingui algun dels strings especificats. És case insensitive.
///                                                    D'aquesta manera es permet executar un test concret o tots aquells que continguin una certa paraula, per exemple, "pacs".

namespace AutoTest
{
inline TestList selectTestsToExecute(const QStringList& options)
{
    if (options.isEmpty())
    {
        return testList();
    }

    TestList selectedTests;
    foreach (QObject *test, testList())
    {
        bool found = false;
        QStringListIterator iterator(options);
        while (!found && iterator.hasNext())
        {
            if (test->objectName().contains(iterator.next(), Qt::CaseInsensitive))
            {
                found = true;
                selectedTests << test;
            }
        }
    }
    return selectedTests;
}

inline int run()
{
    int ret = 0;
    QString dirToSaveTests;
    QStringList testsToExecuteOptions;

    // Comprovem paràmetres nostres. Si ni ha, els eliminem perquè al executar el test no es queixi.
    QStringList arguments = qApp->arguments();
    if (arguments.contains("-saveOutputToDir"))
    {
        int argumentPosition = arguments.indexOf("-saveOutputToDir");
        arguments.removeAt(argumentPosition);
        dirToSaveTests = arguments.value(argumentPosition);
        if (dirToSaveTests.isEmpty() || dirToSaveTests.startsWith("-"))
        {
            std::cerr << "ERROR: Argument -saveOutputToDir needs a directory path as option" << std::endl;
            return -1;
        }
        else
        {
            arguments.removeAt(argumentPosition);
        }

        QDir dir(dirToSaveTests);
        if (dir.exists()) // Si és correcte eliminem els paràmetres
        {
            if (!dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty())
            {
                std::cerr << "ERROR: The directory to save the ouput files has to be empty." << std::endl;
                return -1;
            }
        }
        else
        {
            if (!dir.mkpath(dir.absolutePath()))
            {
                std::cerr << qPrintable(QString("ERROR: Unable to create dir: %1").arg(dir.absolutePath())) << std::endl;
                return -1;
            }
        }
    }
    else if (arguments.contains("-testsToExecute"))
    {
        int argumentPosition = arguments.indexOf("-testsToExecute");
        arguments.removeAt(argumentPosition);
        while (argumentPosition < arguments.size() && !arguments.at(argumentPosition).startsWith("-"))
        {
            testsToExecuteOptions << arguments.at(argumentPosition);
            arguments.removeAt(argumentPosition);
        }

        if (testsToExecuteOptions.isEmpty())
        {
            std::cerr << "ERROR: Argument -testsToExecute needs a list of tests to execute" << std::endl;
            return -1;
        }

    }

    // Generem els paràmetres argc i argv correctes sense els nostres paràmetres
    int modifiedArgc = arguments.count();
    char **modifiedArgv = new char*[modifiedArgc];
    int i = 0;
    foreach (const QString &argument, arguments)
    {
        char *tempArgument = new char[argument.length()];
        strcpy(tempArgument, argument.toStdString().c_str());
        modifiedArgv[i] = tempArgument;
        i++;
    }

    foreach (QObject *test, selectTestsToExecute(testsToExecuteOptions))
    {
        if (!dirToSaveTests.isEmpty())
        {
            freopen(qPrintable(QString("%1/%2.txt").arg(dirToSaveTests).arg(test->objectName())), "w", stdout);
        }
        ret += QTest::qExec(test, modifiedArgc, modifiedArgv);
    }

    //Eliminem la llista de paràmetres
    for (int i = 0; i < modifiedArgc; i++)
    {
        delete modifiedArgv[i];
    }

    delete modifiedArgv;
    return ret;
}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    return AutoTest::run();
}