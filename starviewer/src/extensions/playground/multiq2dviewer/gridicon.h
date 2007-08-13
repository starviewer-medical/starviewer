/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGGRIDICON_H
#define UDGGRIDICON_H

#include <ui_gridiconbase.h>

namespace udg {

/**
Classe que representa un element del tipus icona per poder crear una graella de visualitzadors

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class GridIcon : public QFrame, public Ui::GridIconBase {
Q_OBJECT
public:

    GridIcon( QWidget *parent = 0 );

    ~GridIcon();

};

}

#endif
