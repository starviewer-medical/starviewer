#ifndef UDGAPPLICATIONVERSIONCHECKER_H
#define UDGAPPLICATIONVERSIONCHECKER_H

#include "qreleasenotes.h"
#include <QObject>
class QUrl;

namespace udg {
    class ApplicationUpdateChecker;

/**
    Aquesta classe comprovarà si s'han de mostrar les notes d'una versió instal·lada. En cas que no s'hagin de mostrar, utilitzarà 
    ApplicationVersionCheckerOnServer per buscar si hi ha una nova versió disponible. Les notes es mostraran a través de la classe QReleaseNotes.
    Funciona de manera asíncrona. Quan es crida el mètode checkReleaseNotes es fan les comprobacions. Amb el mètode
    showIfCorrect es mostran si realment hi ha algo a mostrar. Si no ha acabat de fer les crides asíncrones, quan acabi mostrarà el
    que calgui. En mode desenvolupament no es mostraran les notes ni es faran peticions online.
    Treballa sobre els settings:
        - LastReleaseNotesVersionShown: String amb la última versió comprobada
        - NeverShowReleaseNotes: L'usuari decideix no mostrar més els missatges de les release notes
        - LastVersionChecked: Quina és la última versió que s'ha comprobat.
        - LastVersionCheckedDate: En quina data s'ha fet la comprobació.
        - CheckVersionInterval: Cada quants dies es comproba si hi ha una nova versió.
        - DontCheckNewVersionsOnline: Booleà que impedeix que es facin crides online per comprobar si hi ha una nova versió
  */
class ApplicationVersionChecker : public QObject {
Q_OBJECT

public:
    /// Constructor per defecte
    ApplicationVersionChecker(QObject *parent = 0);
    /// Destructor
    ~ApplicationVersionChecker();
    /// Comproba si s'ha instal·lat una nova versió, i en mostrà les notes. En cas de què no existeixin, o no s'hagi instal·lat cap
    /// versió nova, es consultarà al servidor online si hi ha alguna versió disponible.
    void checkReleaseNotes();
    /// Si la comprobació és correcte llavors mostrarà la finestra, si no ha acabat de carregar, la mostrarà quan acabi.
    void showIfCorrect();
    /// Permet especificar un nou interval de temps entre la comprovació de noves versions modificant el setting CheckVersionInterval
    /// Aquest és el mètode que s'ha de cridar si es vol canviar aquest interval des de per exemple un menu d'opcions
    void setCheckVersionInterval(int interval);

    /// Mostra la finestra de les release notes
    /// Es crida des del menu ajuda, així que només comprova que existeixin, i no té en compte si ja s'han mostrat.
    void showLocalReleaseNotes();

signals:
    /// Senyal per indicar que s'ha acabat de carregar
    void checkFinished();

public slots:
    /// Metode que s'invoca quan s'ha acabat de comprobar si hi ha una nova versió online. Depenent del resultat, i dels settings, decidirà si
    /// cal mostrar les release notes o no.
    void onlineCheckFinished();

private:
    enum VersionAttribute { VersionType, VersionNumber };

    /// Genera la url local del fitxer on hi ha les release notes
    QUrl createLocalUrl();
    /// Comproba que la url local de les release notes existeixi
    bool checkLocalUrl(const QUrl &url);

    /// Comprobar si ha passat prou temps per mirar si hi ha una nova versió
    bool checkTimeInterval();

    /// Guardar els settings
    void writeSettings();
    /// Llegir els settings
    void readSettings();

    /// Comproba si la versio instal·lada és més nova que la última que s'han comprobat les release notes
    bool isNewVersionInstalled();
    /// Compara la última part (0.9.1-RC1 -> 1-RC1) de dues versions i retorna si la primera és major que la segona
    bool compareVersions(const QString &current, const QString &lastShown);
    /// Si attribute es type treu el numero de versio d'una alpha, beta, RC, etc. (alpha9 -> alpha),
    /// si attribute es number treu la part de davant i deixa el numero.
    /// @pre Es considera que la versió s'ajusta a l'expressió regular correcte.
    QString getVersionAttribute(const QString &version, VersionAttribute attibute);
    /// Comprova si s'està en mode desenvolupament
    bool isDevelopmentMode();

private slots:
    /// Quan al cridar el mostrar no hagi acabat de carregar, quan ho faci es cridarà aquest mètode
    void showWhenCheckFinished();

private:
    /// Guarda l'url que es carregarà
    QUrl m_urlToShow;

    /// Indica si ha acabat de carregar-ho tot
    bool m_checkFinished;
    /// Atribut que indica si s'esta comprobant la nova versió o mostrant les release notes
    bool m_checkNewVersion;
    /// Atribut per guardar temporalment la versió que s'esta comprobant
    QString m_checkedVersion;

    /// Referencia a les QReleaseNotes que controlaran la finestra on es mostren les notes
    QReleaseNotes *m_releaseNotes;

    /// Settings
    QString m_lastReleaseNotesVersionShown;
    QString m_lastVersionCheckedDate;
    QString m_lastVersionChecked;
    int m_checkVersionInterval;
    bool m_neverShowNewVersionReleaseNotes;
    bool m_dontCheckNewVersionsOnline;

    /// Per comprovar la versió amb el servidor
    ApplicationUpdateChecker *m_applicationUpdateChecker;
};

} // End namespace udg

#endif // UDGAPPLICATIONVERSIONCHECKER_H
