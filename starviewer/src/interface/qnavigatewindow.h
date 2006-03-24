/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQNAVIGATEWINDOW_H
#define UDGQNAVIGATEWINDOW_H
#include <QWidget>

#include "ui_qnavigatewindowbase.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QNavigateWindow : public QWidget, private Ui::QNavigateWindowBase
{
Q_OBJECT
public:
    QNavigateWindow(QWidget *parent = 0);

    ~QNavigateWindow();

};

}

#endif
