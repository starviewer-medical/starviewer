// Starviewer
#include "applicationversiontest.h"
#include "starviewerapplication.h"
#include "applicationupdatechecker.h"
// Qt
#include <QString>
#include <QThread>

namespace udg {

ApplicationVersionTest::ApplicationVersionTest(QObject *parent)
 : DiagnosisTest(parent)
{
}

ApplicationVersionTest::~ApplicationVersionTest()
{
}

DiagnosisTestResult ApplicationVersionTest::run()
{
    /// Fer el check online. Després de cridar aquest mètode, les variables m_onlineCheckOk i m_newVersionAvailable prenen valor.
    onlineCheck();

    DiagnosisTestResult result;

    if (m_onlineCheckOk)
    {
        if (m_newVersionAvailable)
        {
            DiagnosisTestProblem problem;
            problem.setState(DiagnosisTestProblem::Warning);
            problem.setDescription(tr("There is a new version available."));
            problem.setSolution(tr("Contact technical service to request the software update."));
            result.addWarning(problem);
        }
    }
    else
    {
        // En mode Debug retorna error, ja que la caden de text de la versió de l'aplicació que s'envia al webservice conté -devel
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Error);
        QRegExp url("http://[\\S]+", Qt::CaseSensitive);
        problem.setDescription(m_olineCheckerErrorDescription.remove(url));
        // TODO Proposar una solució quan hi ha hagut error en el check online de les release notes
        problem.setSolution("");
        result.addError(problem);
    } 

    return result;
}

QString ApplicationVersionTest::getDescription()
{
    return tr("%1 is updated to the last version").arg(ApplicationNameString);
}

void ApplicationVersionTest::onlineCheck()
{
    ApplicationUpdateChecker onlineChecker;
    QThread onlineCheckerThread;
    // Moure l'onlineChecker a un altre thread
    onlineChecker.moveToThread(&onlineCheckerThread);
    // La conexió de checkFinished s'ha de fer en mode DirectConnection per tal de què s'executi correctament al thread que toca
    connect(&onlineChecker, SIGNAL(checkFinished()), &onlineCheckerThread, SLOT(quit()), Qt::DirectConnection);
    // Per tal d'executar el mètode check for updates a l'altre thread, el que farem és connectar-lo a un signal, i fer un emit quan sigui el moment
    connect(this, SIGNAL(startCheckForUpdates()), &onlineChecker, SLOT(checkForUpdates()));

    // Iniciar el thread, cridar el checkForUpdates i esperar síncronament a qué acabi
    onlineCheckerThread.start();
    emit startCheckForUpdates();
    onlineCheckerThread.wait();

    m_onlineCheckOk = onlineChecker.isOnlineCheckOk();
    m_newVersionAvailable = onlineChecker.isNewVersionAvailable();
    m_olineCheckerErrorDescription = onlineChecker.getErrorDescription();
}

}
