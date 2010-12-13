/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGAPPLICATIONVERSIONCHECKER_H
#define UDGAPPLICATIONVERSIONCHECKER_H

#include "qreleasenotes.h"
#include <QObject>
class QUrl;
class QNetworkReply;
class QNetworkProxy;
class QNetworkAccessManager;

namespace udg {

    class QReleaseNotes;

/** Aquesta classe comprovarà si s'han de mostrar les notes d'una versió instalada. En cas que no s'hagin de mostrar
    buscarà si hi ha una nova versió disponible. Les notes es mostraran a través de la classe QReleaseNotes.
    Funciona de manera asíncrona, quan es crida el mètode check checkReleaseNotes es fan les comprobacions. Amb el mètode
    showIfCorrect es mostra si hi ha algo a mostrar. Si no ha acabat de fer les crides asíncrones, quan acabi mostrarà el
    que calgui. En mode desenvolupament no es mostraran les notes ni es faran peticions online.
    Treballa sobre els settings:
        - LastReleaseNotesVersionShown: String amb la última versió comprobada
        - NeverShowReleaseNotes: L'usuari decideix no mostrar més els missatges de les release notes
        - LastVersionChecked: Quina és la última versió que s'ha comprobat.
        - LastVersionCheckedDate: En quina data s'ha fet la comprobació. 
        - CheckVersionInterval: Cada quants dies es comproba si hi ha una nova versió.
        - DontCheckNewVersionsOnline: Booleà que impedeix que es facin crides online per comprobar si hi ha una nova versió
  */
class ApplicationVersionChecker : QObject {
Q_OBJECT

public:
    /// Constructor per defecte
    ApplicationVersionChecker();
    /// Destructor
    ~ApplicationVersionChecker();
    /// Comprobarà que els url existeixin, fara les crides al webservice si cal i quan tot estigui correcte i a punt per mostrar
    /// m_correct tindra el valor de si ha anat tot bé o no, i m_finished loading valdrà cert.
    void checkReleaseNotes();
    /// Si la comprobació és correcte llavors mostrarà la finestra, si no ha acabat de carregar, la mostrarà quan acabi
    void showIfCorrect();
    /// Permet especificar un nou interval de temps entre la comprovació de noves versions modificant el setting CheckVersionInterval
    /// Aquest és el mètode que s'ha de cridar si es vol canviar aquest interval des de per exemple un menu d'opcions
    void setCheckVersionInterval(int interval);

signals:
    /// Senyal per indicar que s'ha acabat de carregar
    void checkFinished();

private:
    /// Genera la url local del fitxer on hi ha les release notes
    QUrl createLocalUrl();
    /// Comproba que la url local de les release notes existeixi
    bool checkLocalUrl(const QUrl &url);

    /// Comprobar si ha passat prou temps per mirar si hi ha una nova versió
    bool checkTimeInterval();
    /// Fer la crida al servidor per obtenir si hi ha una nova versió
    void checkVersionOnServer();
    
    /// Genera la url per fer la crida al servidor i obtenir la versió
    QString createWebServiceUrl();
    /// Codifica en base64 una QByteArray i a més el transforma per que sigui una part d'una url vàlida
    QString encryptBase64Url(const QString &url);
    /// Assigna el proxy per defecte, si n'hi ha, a un QNetworkAccessManager, si no n'hi ha el busca.
    void setProxy(const QUrl &url);
    
    /// Guardar els settings
    void writeSettings();
    /// Llegir els settings
    void readSettings();
    
    /// Posa l'atribut finished a true i emet la senyal de checkFinished
    void setCheckFinished();

    /// Comproba si la versio instal·lada és més nova que la última que s'han comprobat les release notes
    bool isNewVersionInstalled();
    /// Compara la última part de dues versions (0.9.1-RC1, 1-RC1) i retorna si la primera és major que la segona 
    bool compareVersions(const QString &current, const QString &lastShown);
    /// Si attribute es type treu el numero de versio d'una alpha, beta, RC, etc. (alpha9 -> alpha), 
    /// si attribute es number treu la part de davant i deixa el numero.
    /// @pre Es considera que la versió s'ajusta a l'expressió regular correcte. 
    QString getVersionAttribute(const QString &version, const QString &attibute);
    /// Comprova si s'està en mode desenvolupament
    bool isDevelopmentMode();

private slots:
    /// Tracta la resposta del webservice obtenint la versió i la url de les notes d'aquesta nova versió
    void webServiceReply(QNetworkReply *reply);
    /// Tracta la resposta de les notes de la nova versió disponible i posa el seu contingut al WebView
    void updateNotesUrlReply(QNetworkReply *reply);
    /// Event que es crida quan es tanca la finestra de les QReleaseNotes
    void closeEvent();
    /// Quan al cridar el mostrar no hagi acabat de carregar, quan ho faci es cridarà aquest mètode
    void showWhenCheckFinished();

private:
    /// Atribut per decidir si tot s'ha carregat correctament i es pot mostrar la finestra
    bool m_somethingToShow;
    /// Indica si ha acabat de carregar-ho tot
    bool m_checkFinished;
    /// Atribut que indica si s'esta comprobant la nova versió o mostrant les release notes
    bool m_checkNewVersion;
    /// Atribut per guardar temporalment la versió que s'esta comprobant
    QString m_checkedVersion;

    /// Atribut per gestionar les connexions http
    QNetworkAccessManager *m_manager;
    /// Referencia a les QReleaseNotes que controlaran la finestra on es mostren les notes
    QReleaseNotes *m_releaseNotes;

    /// Settings
    QString m_lastReleaseNotesVersionShown;
    QString m_lastVersionCheckedDate;
    QString m_lastVersionChecked;
    int m_checkVersionInterval;
    bool m_neverShowNewVersionReleaseNotes;
    bool m_dontCheckNewVersionsOnline;
};

} // end namespace udg

#endif //UDGAPPLICATIONVERSIONCHECKER_H
