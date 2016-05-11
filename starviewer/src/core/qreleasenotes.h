/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
    void loadFinished(bool ok);
};

} // End namespace udg

#endif // UDGQRELEASENOTES_H
