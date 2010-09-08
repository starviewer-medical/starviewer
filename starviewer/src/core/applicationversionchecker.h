/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGAPPLICATIONVERSIONCHECKER_H
#define UDGAPPLICATIONVERSIONCHECKER_H

#include "qreleasenotes.h"
#include <QUrl>
#include <QObject>
class QNetworkReply;
class QNetworkProxy;
class QNetworkAccessManager;


namespace udg {

    class QReleaseNotes;

/** Aquesta classe comprovarà si s'han de mostrar les notes d'una versió instalada. En cas que no s'hagin de mostrar
    buscarà si hi ha una nova versió disponible. Les notes es mostraran a través de la classe QReleaseNotes.
    Treballa sobre els settings:
        - ShowReleaseNotesFirstTime: booleà que indica si es la primera vegada que s'obre des de l'actualització
        - NeverShowReleaseNotes: L'usuari decideix no mostrar més els missatges de les release notes
        - LastVersionChecked: Quina és la última versió que s'ha comprobat.
        - LastVersionCheckedDate: En quina data s'ha fet la comprobació. 
        - CheckVersionInterval: Cada quants dies es comproba si hi ha una nova versió.
  */
    class ApplicationVersionChecker : QObject {
Q_OBJECT


public:
    /// Constructor per defecte
    ApplicationVersionChecker();
    /// Destructor
    ~ApplicationVersionChecker();
    /// Comprobarà que els url existeixin i es puguin obrir i llavors mostrara la finestra
    void showIfCorrect();
    /// Permet especificar un nou interval de temps entre la comprovació de noves versions modificant el setting CheckVersionInterval
    /// Aquest és el mètode que s'ha de cridar si es vol canviar aquest interval des de per exemple un menu d'opcions
    void setCheckVersionInterval(int interval);
    
//public slots:

//protected:

private:
    /// Genera la url local del fitxer on hi ha les release notes
    QUrl createLocalUrl();

    /// Comproba que la url local de les release notes existeixi
    bool checkLocalUrl(QUrl url);

    /// Comprobar si ha passat prou temps per mirar si hi ha una nova versió
    bool checkTimeInterval();

    /// Fer la crida al servidor per obtenir si hi ha una nova versió
    void checkVersionOnServer();

    /// Genera la url per fer la crida al servidor i obtenir la versió
    QString createWebServiceUrl();

    /// Codifica en base64 una QByteArray i a més el transforma per que sigui una part d'una url vàlida
    QString encryptBase64Url(QString url);

    /// Assigna el proxy per defecte, si n'hi ha, a un QNetworkAccessManager, si no n'hi ha el busca.
    void setProxy(QUrl url);

    /// Guardar els settings
    void writeSettings();

    /// Llegir els settings
    void readSettings();

private slots:
    /// Tracta la resposta del webservice obtenint la versió i la url de les notes d'aquesta nova versió
    void webServiceReply(QNetworkReply *reply);
    /// Tracta la resposta de les notes de la nova versió disponible i posa el seu contingut al WebView
    void updateNotesUrlReply(QNetworkReply *reply);
    /// Event que es crida quan es tanca la finestra de les QReleaseNotes
    void closeEvent();

private:
    /// Atribut que indica si s'esta comprobant la nova versió o mostrant les release notes
    bool m_checkNewVersion;
    /// Atribut per guardar temporalment la versió que s'esta comprobant
    QString m_checkedVersion;

    /// Atribut per gestionar les connexions http
    QNetworkAccessManager *m_manager;

    /// Referencia a les QReleaseNotes que controlaran la finestra on es mostren les notes
    QReleaseNotes *m_releaseNotes;

    /// Settings
    bool m_showReleaseNotesFirstTime;
    bool m_neverShowReleaseNotes;
    QString m_lastVersionCheckedDate;
    QString m_lastVersionChecked;
    int m_checkVersionInterval;
};

} // end namespace udg

#endif