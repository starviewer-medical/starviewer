/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQRELEASENOTES_H
#define UDGQRELEASENOTES_H

#include "ui_qreleasenotesbase.h"

#include <QWidget>
class QNetworkProxy;

namespace udg {

/** Aquesta classe s'utilitza per mostrar les release notes, conté un QWebView per tal de mostrar-les com a html.
    Es gestionada per ApplicationVersionChecker.
  */
class QReleaseNotes : public QWidget, Ui::QReleaseNotesBase {
Q_OBJECT

public:
    /// Constructor amb els atributs del QWidget on es posa (0 per que sigui una finestra nova)
    /// i la referència al pare per tal de poder-li indicar quan es tanca la finestra
    QReleaseNotes(QWidget *parent);
    /// Destructor
    ~QReleaseNotes();
    /// Retorna si esta activat el checkbox de no mostrar més
    bool isDontShowAnymoreChecked();
    /// Mostra o amaga el checkbox don't show anymore
    void setDontShowVisible(bool visible);
    /// Determina quin url haurà d'obrir la finestra de les release notes
    void setUrl(QUrl url);
    
signals:
    // Senyal per indicar a ApplicationVersionChecker que es tanca la finestra
    void closing();

//public slots:

protected:
    /// Redifinir el close event
    void closeEvent(QCloseEvent *event);

//private:

//private:
    
};

} // end namespace udg

#endif