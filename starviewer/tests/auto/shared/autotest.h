#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <QTest>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QDir>
#include <iostream>

namespace AutoTest
{
typedef QList<QObject*> TestList;

inline TestList& testList()
{
    static TestList list;
    return list;
}

inline bool findObject(QObject* object)
{
    TestList& list = testList();
    if (list.contains(object))
    {
        return true;
    }
    foreach (QObject* test, list)
    {
        if (test->objectName() == object->objectName())
        {
            return true;
        }
    }
    return false;
}

inline void addTest(QObject* object)
{
    TestList& list = testList();
    if (!findObject(object))
    {
        list.append(object);
    }
}


inline int run()
{
    int ret = 0;
    QString dirToSaveTests;

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

    // Generem els paràmetres argc i argv correctes sense els nostres paràmetres
    int modifiedArgc = arguments.count();
    char *modifiedArgv[modifiedArgc];
    int i = 0;
    foreach (const QString &argument, arguments)
    {
        char *tempArgument = new char[argument.length()];
        strcpy(tempArgument, argument.toStdString().c_str());
        modifiedArgv[i] = tempArgument;
        i++;
    }

    foreach (QObject *test, testList())
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

    return ret;
}
}

template <class T>
class Test
{
public:
    QSharedPointer<T> child;

    Test(const QString& name) : child(new T)
    {
        child->setObjectName(name);
        AutoTest::addTest(child.data());
    }
};

#define DECLARE_TEST(className) static Test<className> t(#className);

#define TEST_MAIN \
int main(int argc, char *argv[]) \
{ \
    QApplication app(argc, argv); \
    return AutoTest::run(); \
}

#endif // AUTOTEST_H
