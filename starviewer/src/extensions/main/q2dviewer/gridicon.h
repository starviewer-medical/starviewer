/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGGRIDICON_H
#define UDGGRIDICON_H

#include <QLabel>
#include <QGridLayout>

namespace udg {

/**
Classe que representa un element del tipus icona per poder crear un menu per escollir una graella de visualitzadors

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class GridIcon : public QFrame {
Q_OBJECT
public:

    GridIcon( QWidget *parent = 0 );

    ~GridIcon();

private:

    /// Label on posarem la icona
    QLabel *label;

};

}

#endif
