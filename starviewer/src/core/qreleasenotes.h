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

namespace udg {

/** Aquesta classe mostrarà una finestra amb totes les novetats de la versió final.
    Treballa sobre els settings:
        - ShowReleaseNotesFirstTime: booleà que indica si es la primera vegada que s'obre des de l'actualització
        - NeverShowReleaseNotes: L'usuari decideix no mostrar més els missatges de les release notes
  */
class QReleaseNotes : public QWidget, Ui::QReleaseNotesBase {
Q_OBJECT

public:
    QReleaseNotes(QWidget *parent = 0);
    /// Quan s'hagi comprobat si es pot obrir la url, llavors es mostrara la finestra, altrament no
    void show();
    
public slots:
    /// Comproba que la url funciona
    void checkLoadResult(bool result);

protected:
    /// Redifinir el close event
    void closeEvent(QCloseEvent *event);

private:
    /// Genera la url local del fitxer on hi ha les release notes
    QUrl createUrl();

    /// Guardar els settings
    void writeSettings();

private:
    /// Atribut per indicar si s'ha trobat o no la url
    bool m_urlNotFound;
    /// Atribut per indicar que ja s'ha comprobat la url
    bool m_urlChecked; 
    // Atribut per indicar que es vol mostrar la finestra
    bool m_intendToShow; 
};

} // end namespace udg

#endif