#ifndef UDGCHECKFORUPDATES_H
#define UDGCHECKFORUPDATES_H

#include <QObject>
class QUrl;
class QNetworkReply;
class QNetworkAccessManager;

namespace udg {

/**
    Aquesta classe fa una crida al webservice per comprobar si hi ha una versió disponible de starviewer més nova que la que tenim instal·lada.
  */
class CheckForUpdates : public QObject {
Q_OBJECT

public:
    /// Constructor per defecte
    CheckForUpdates(QObject *parent = 0);
    /// Destructor
    ~CheckForUpdates();
    /// Fer la crida al servidor per obtenir si hi ha una nova versió
    void checkForUpdates();
    
    /// Retorna l'url, en forma de QString, de les notes de la nova versió. El resultat és correcte sempre que isNewVersionAvailable sigui cert.
    QString getReleaseNotesUrl() const;
    /// Retorna l'url, en forma de QString, la nova versió. El resultat és correcte sempre que isNewVersionAvailable sigui cert.
    QString getVersion() const;
    /// Retorna si hi ha una versió disponible més nova de la que tenim instal·lada.
    bool isNewVersionAvailable() const;

    /// Retorna si no hi ha hagut error
    bool isOnlineCheckOk() const;
    /// Retorna la descripció de l'error. Si el test ha acabat bé, aquest valor no té sentit.
    QString getErrorDescription() const;

signals:
    /// Senyal per indicar que s'ha acabat de carregar
    void checkFinished();

private:
    /// Genera la url per fer la crida al servidor i obtenir la versió
    QString createWebServiceUrl();
    /// Assigna el proxy per defecte, si n'hi ha, a un QNetworkAccessManager, si no n'hi ha el busca.
    void setProxy(const QUrl &url);

    /// Posa l'atribut finished a true i emet la senyal de checkFinished
    void setCheckFinished();

    /// Comprova que la resposta del servidor web no sigui un error i crida el metode parseJSON
    void parseWebServiceReply(QNetworkReply *reply);
    /// Interpreta la resposta del servidor en format JSON i emplena els atributs corresponents.
    void parseJSON(const QString &json);
    
private slots:
    /// Tracta la resposta del webservice obtenint la versió i la url de les notes de la nova versió
    void checkForUpdatesReply(QNetworkReply *reply);
    /// Comprova si la url de les release notes és vàlida
    void doesUpdateNotesUrlExistOnServerReply(QNetworkReply *reply);

private:
    /// Indica si ha acabat de carregar-ho tot
    bool m_checkFinished;

    /// Gestiona les connexions http
    QNetworkAccessManager *m_manager;

    /// La última versió que hi ha al server
    QString m_checkedVersion;
    /// La url de les notes
    QString m_releaseNotesURL;
    /// Si hi ha o no una nova versió
    bool m_updateAvailable;

    /// Si hi ha algun error de connexió amb el servidor
    bool m_checkOk;
    QString m_errorDescription;
};

} // End namespace udg

#endif // UDGCHECKFORUPDATES_H
