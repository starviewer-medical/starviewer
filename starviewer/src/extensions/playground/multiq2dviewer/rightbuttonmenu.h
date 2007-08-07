/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRIGHTBUTTONMENU_H
#define UDGRIGHTBUTTONMENU_H

#include <ui_rightbuttonmenubase.h>
#include "patient.h"
#include "rightmenuitem.h"
#include "study.h"
#include "series.h"
#include <QMenu>
#include <QLabel>

namespace udg {

/**
Classe que correspon al widget que es mostra al pulsar el botó dret del ratolí.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RightButtonMenu : public QWidget, public Ui::RightButtonMenuBase
{
Q_OBJECT
public:

    RightButtonMenu( QWidget *parent = 0 );
    ~RightButtonMenu(){}

    /// Posem el pacient al widget
    void setPatient( Patient * patient );

    /// Assignem una posició al menú
    void setPosition( QPoint point );

signals:
    /// senyal que envia la serie escollida per ser visualitzada
    void selectedSeries( Series *series );
    void selectedSeries( QString uid );

public slots:
    void showInformation( int y, QWidget * moreInformation );

private:
    /// Creem un widget amb la informació bàsica d'un estudi
    QWidget * createStudyWidget( Study * study, QWidget * parent );

    /// Creem un widget amb la informació bàsica d'una sèrie
    RightMenuItem * createSerieWidget( Series * serie, QWidget * parent );

private slots:
    void emitSelected( Series * serie );

};

}

#endif
