#include "autotest.h"
#include "windowsfirewallaccess.h"

using namespace udg;

class TestingWindowsFirewallAccess : public WindowsFirewallAccess {
public:
    HRESULT m_initializeWindowsFirewallResult;
    bool m_firewallOn;
    HRESULT m_windowsFirewallIsOnResult;
    bool m_exceptionsAllowed;
    HRESULT m_doesWindowsFirewallAllowExceptionsResult;
    bool m_firewallApplicationEnabled;
    HRESULT m_isApplicationEnabledAtFirewallResult;
    HRESULT m_initializeWindowsFirewallLibraryResult;

protected:
    virtual HRESULT initializeWindowsFirewall(INetFwProfile **firewallProfile)
    {
        Q_UNUSED(firewallProfile);
        return m_initializeWindowsFirewallResult;
    }

    virtual HRESULT windowsFirewallIsOn(INetFwProfile *firewallProfile, bool *firewallOn)
    {
        Q_UNUSED(firewallProfile);
        *firewallOn = m_firewallOn;
        return m_windowsFirewallIsOnResult;
    }

    virtual HRESULT doesWindowsFirewallAllowExceptions(INetFwProfile *firewallProfile, bool *exceptionsAllowed)
    {
        Q_UNUSED(firewallProfile);
        *exceptionsAllowed = m_exceptionsAllowed;
        return m_doesWindowsFirewallAllowExceptionsResult;
    }

    virtual HRESULT isApplicationEnabledAtFirewall(INetFwProfile *firewallProfile, BSTR firewallProcessImageFileName, bool *firewallApplicationEnabled)
    {
        Q_UNUSED(firewallProfile);
        Q_UNUSED(firewallProcessImageFileName);
        *firewallApplicationEnabled = m_firewallApplicationEnabled;
        return m_isApplicationEnabledAtFirewallResult;
    }

    virtual HRESULT initializeWindowsFirewallLibrary(LPVOID pvReserved, DWORD dwCoInit)
    {
        Q_UNUSED(pvReserved);
        Q_UNUSED(dwCoInit);
        return m_initializeWindowsFirewallLibraryResult;
    }

    virtual void UninitializeWindowsFirewallLibrary(){}

    virtual void CleanupWindowsFirewall(INetFwProfile *firewallProfile)
    {
        Q_UNUSED(firewallProfile);
    }
};

Q_DECLARE_METATYPE(FirewallAccess::FirewallStatus)

class test_WindowsFirewallAccess : public QObject {
Q_OBJECT

private slots:
    void doesStarviewerHaveAccesThroughFirewall_ShouldCheckIfFirewallIsAccessible_data();
    void doesStarviewerHaveAccesThroughFirewall_ShouldCheckIfFirewallIsAccessible();

    void doesStarviewerHaveAccesThroughFirewall_ShouldFailAndReturnError_data();
    void doesStarviewerHaveAccesThroughFirewall_ShouldFailAndReturnError();
};

void test_WindowsFirewallAccess::doesStarviewerHaveAccesThroughFirewall_ShouldCheckIfFirewallIsAccessible_data()
{
    /// Resultats
    QTest::addColumn<bool>("firewallIsAccessible");
    QTest::addColumn<FirewallAccess::FirewallStatus>("status");
    QTest::addColumn<QString>("errorString");

    /// Variables internes
    QTest::addColumn<HRESULT>("testingInitializeWindowsFirewallLibraryResult");
    QTest::addColumn<HRESULT>("testingInitializeWindowsFirewallResult");
    
    QTest::addColumn<HRESULT>("testingWindowsFirewallIsOnResult");
    QTest::addColumn<bool>("testingFirewallOn");
    
    QTest::addColumn<HRESULT>("testingDoesWindowsFirewallAllowExceptionsResult");
    QTest::addColumn<bool>("testingExceptionsAllowed");
    
    QTest::addColumn<HRESULT>("testingIsApplicationEnabledAtFirewallResult");
    QTest::addColumn<bool>("testingFirewallApplicationEnabled");

    /// Variables per difinir que el paràmetre no s'utilitza en el test
    bool unusedBool = true;
    HRESULT unusedHRESULT = S_OK;
    QString unusedString = "";

    // Les dades de test
    QTest::newRow("firewall disabled") << true << FirewallAccess::FirewallIsAccessible << unusedString
                                       << S_OK << S_OK
                                       << S_OK << false
                                       << unusedHRESULT << unusedBool
                                       << unusedHRESULT << unusedBool;
    
    QTest::newRow("firewall exceptions disabled") << false << FirewallAccess::FirewallIsBlocking << "Windows firewall does not allow exceptions"
                                                  << S_OK << S_OK
                                                  << S_OK << true
                                                  << S_OK << false
                                                  << unusedHRESULT << unusedBool;

    QTest::newRow("firewall starviewer disabled") << false << FirewallAccess::FirewallIsBlocking << "Application is not in the firewall exceptions list"
                                                  << S_OK << S_OK
                                                  << S_OK << true
                                                  << S_OK << true
                                                  << S_OK << false;
    
    QTest::newRow("firewall accessible") << true << FirewallAccess::FirewallIsAccessible << unusedString
                                         << S_OK << S_OK
                                         << S_OK << true
                                         << S_OK << true
                                         << S_OK << true;

}

void test_WindowsFirewallAccess::doesStarviewerHaveAccesThroughFirewall_ShouldCheckIfFirewallIsAccessible()
{
    QFETCH(bool, firewallIsAccessible);
    QFETCH(FirewallAccess::FirewallStatus, status);
    QFETCH(QString, errorString);
    
    QFETCH(HRESULT, testingInitializeWindowsFirewallLibraryResult);
    QFETCH(HRESULT, testingInitializeWindowsFirewallResult);

    QFETCH(HRESULT, testingWindowsFirewallIsOnResult);
    QFETCH(bool, testingFirewallOn);

    QFETCH(HRESULT, testingDoesWindowsFirewallAllowExceptionsResult);
    QFETCH(bool, testingExceptionsAllowed);
    
    QFETCH(HRESULT, testingIsApplicationEnabledAtFirewallResult);
    QFETCH(bool, testingFirewallApplicationEnabled);

    TestingWindowsFirewallAccess windowsFirewallAccess;
    windowsFirewallAccess.m_initializeWindowsFirewallResult = testingInitializeWindowsFirewallResult;
    windowsFirewallAccess.m_firewallOn = testingFirewallOn;
    windowsFirewallAccess.m_windowsFirewallIsOnResult = testingWindowsFirewallIsOnResult;
    windowsFirewallAccess.m_exceptionsAllowed = testingExceptionsAllowed;
    windowsFirewallAccess.m_doesWindowsFirewallAllowExceptionsResult = testingDoesWindowsFirewallAllowExceptionsResult;
    windowsFirewallAccess.m_firewallApplicationEnabled = testingFirewallApplicationEnabled;
    windowsFirewallAccess.m_isApplicationEnabledAtFirewallResult = testingIsApplicationEnabledAtFirewallResult;
    windowsFirewallAccess.m_initializeWindowsFirewallLibraryResult = testingInitializeWindowsFirewallLibraryResult;

    QCOMPARE(windowsFirewallAccess.doesStarviewerHaveAccesThroughFirewall(), firewallIsAccessible);
    QCOMPARE(windowsFirewallAccess.getStatus(), status);
    if (windowsFirewallAccess.getStatus() == FirewallAccess::FirewallIsBlocking)
    {
        QCOMPARE(windowsFirewallAccess.getErrorString(), errorString);
    }
}

void test_WindowsFirewallAccess::doesStarviewerHaveAccesThroughFirewall_ShouldFailAndReturnError_data()
{
    /// Resultats
    QTest::addColumn<bool>("firewallIsAccessible");
    QTest::addColumn<FirewallAccess::FirewallStatus>("status");
    QTest::addColumn<QString>("errorString");

    /// Variables internes
    QTest::addColumn<HRESULT>("testingInitializeWindowsFirewallLibraryResult");
    QTest::addColumn<HRESULT>("testingInitializeWindowsFirewallResult");
    
    QTest::addColumn<HRESULT>("testingWindowsFirewallIsOnResult");
    QTest::addColumn<bool>("testingFirewallOn");
    
    QTest::addColumn<HRESULT>("testingDoesWindowsFirewallAllowExceptionsResult");
    QTest::addColumn<bool>("testingExceptionsAllowed");
    
    QTest::addColumn<HRESULT>("testingIsApplicationEnabledAtFirewallResult");
    QTest::addColumn<bool>("testingFirewallApplicationEnabled");

    /// Variables per difinir que el paràmetre no s'utilitza en el test
    bool unusedBool = true;
    HRESULT unusedHRESULT = S_OK;

    // Les dades del test
    QTest::newRow("initialize firewall library fail") << false << FirewallAccess::FirewallError << "Error initializing firewall library"
                                                      << E_FAIL << unusedHRESULT
                                                      << unusedHRESULT << unusedBool 
                                                      << unusedHRESULT << unusedBool
                                                      << unusedHRESULT << unusedBool;

    QTest::newRow("initialize firewall fail") << false << FirewallAccess::FirewallError << "Unable to initialize firewall"
                                              << S_OK << E_FAIL
                                              << unusedHRESULT << unusedBool 
                                              << unusedHRESULT << unusedBool
                                              << unusedHRESULT << unusedBool;
    
    QTest::newRow("firewall is on fail") << false << FirewallAccess::FirewallError << "Unable to check if firewall is enabled"
                                         << S_OK << S_OK
                                         << E_FAIL << unusedBool
                                         << unusedHRESULT << unusedBool
                                         << unusedHRESULT << unusedBool;

    QTest::newRow("firewall allow exceptions fail") << false << FirewallAccess::FirewallError << "Unable to check if firewall allows exceptions"
                                                    << S_OK << S_OK
                                                    << S_OK << true 
                                                    << E_FAIL << unusedBool
                                                    << unusedHRESULT << unusedBool;

    QTest::newRow("application enabled at firewall fail") << false << FirewallAccess::FirewallError
                                                          << "Unable to check if application is in the firewall exceptions list"
                                                          << S_OK << S_OK
                                                          << S_OK << true 
                                                          << S_OK << true 
                                                          << E_FAIL << unusedBool;
}

void test_WindowsFirewallAccess::doesStarviewerHaveAccesThroughFirewall_ShouldFailAndReturnError()
{
    QFETCH(bool, firewallIsAccessible);
    QFETCH(FirewallAccess::FirewallStatus, status);
    QFETCH(QString, errorString);
    
    QFETCH(HRESULT, testingInitializeWindowsFirewallResult);
    
    QFETCH(HRESULT, testingWindowsFirewallIsOnResult);
    QFETCH(bool, testingFirewallOn);
    
    QFETCH(HRESULT, testingDoesWindowsFirewallAllowExceptionsResult);
    QFETCH(bool, testingExceptionsAllowed);
    
    QFETCH(HRESULT, testingIsApplicationEnabledAtFirewallResult);
    QFETCH(bool, testingFirewallApplicationEnabled);

    QFETCH(HRESULT, testingInitializeWindowsFirewallLibraryResult);

    TestingWindowsFirewallAccess windowsFirewallAccess;
    windowsFirewallAccess.m_initializeWindowsFirewallResult = testingInitializeWindowsFirewallResult;
    windowsFirewallAccess.m_firewallOn = testingFirewallOn;
    windowsFirewallAccess.m_windowsFirewallIsOnResult = testingWindowsFirewallIsOnResult;
    windowsFirewallAccess.m_exceptionsAllowed = testingExceptionsAllowed;
    windowsFirewallAccess.m_doesWindowsFirewallAllowExceptionsResult = testingDoesWindowsFirewallAllowExceptionsResult;
    windowsFirewallAccess.m_firewallApplicationEnabled = testingFirewallApplicationEnabled;
    windowsFirewallAccess.m_isApplicationEnabledAtFirewallResult = testingIsApplicationEnabledAtFirewallResult;
    windowsFirewallAccess.m_initializeWindowsFirewallLibraryResult = testingInitializeWindowsFirewallLibraryResult;

    QCOMPARE(windowsFirewallAccess.doesStarviewerHaveAccesThroughFirewall(), firewallIsAccessible);
    QCOMPARE(windowsFirewallAccess.getStatus(), status);
    if (windowsFirewallAccess.getStatus() == FirewallAccess::FirewallError)
    {
        QCOMPARE(windowsFirewallAccess.getErrorString(), errorString);
    }
}

DECLARE_TEST(test_WindowsFirewallAccess)

#include "test_windowsfirewallaccess.moc"

