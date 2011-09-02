#ifndef UDGQRELEASENOTES_H
#define UDGQRELEASENOTES_H

#include "ui_qreleasenotesbase.h"

#include <QWidget>

class QNetworkReply;

namespace udg {

/**
    Aquesta classe s'utilitza per mostrar les release notes, conté un QWebView per tal de mostrar-les com a html.
    Es gestionada per ApplicationVersionChecker.
  */
class QReleaseNotes : public QWidget, Ui::QReleaseNotesBase {
Q_OBJECT

public:
    /// Constructor amb els atributs del QWidget on es posa (0 per que sigui una finestra nova)
    /// i la referència al pare per tal de poder-li indicar quan es tanca la finestra
    QReleaseNotes(QWidget *parent = 0);
    /// Destructor
    ~QReleaseNotes();
    /// Retorna si esta activat el checkbox de no mostrar més
    bool isDontShowAnymoreChecked();
    /// Mostra o amaga el checkbox don't show anymore
    void setDontShowVisible(bool visible);
    
    /// Comprova que l'url existeixi i, si és així, es mostra la finestra de les release notes.
    void showIfUrlLoadsSuccessfully(const QUrl &url);

protected:
    /// Redifinir el close event per quan es tanqui, guardar els settings
    void closeEvent(QCloseEvent *event);

protected slots:
    /// comprova si l'url que se li passa relament existeix
    void loadFinished(QNetworkReply *reply);
};

} // End namespace udg

#endif // UDGQRELEASENOTES_H
