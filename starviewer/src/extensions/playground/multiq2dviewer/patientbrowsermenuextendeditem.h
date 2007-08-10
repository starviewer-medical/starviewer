/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWSERMENUEXTENDEDITEM_H
#define UDGPATIENTBROWSERMENUEXTENDEDITEM_H

#include <QWidget>
#include <QFrame>
#include <series.h>
#include <QLabel>


namespace udg {

/**
Classe per mostrar la informació addicional referent a una serie d'un estudi d'un pacient.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuExtendedItem : public QFrame
{
Q_OBJECT
public:

    PatientBrowserMenuExtendedItem( QFrame *parent = 0 );

    PatientBrowserMenuExtendedItem( Series * serie, QFrame *parent = 0 );

    ~PatientBrowserMenuExtendedItem();

public slots:

    /// Canvia la serie a mostrar
    void showSerie( int y , Series * serie );

private:

    /// Crea el widget inicial sense cap informació
    void createInitialWidget();

    /// Assigna una serie a la classe
    void setSerie( Series * serie );

    /// Serie a la qual es representa la informació addicional.
    Series * m_serie;

    /// Label que conté el text addicional a mostrar
    QLabel * m_text;
        
    /// Label que conté la icona a mostrar
    QLabel * m_icon;

signals:

    void setPosition( PatientBrowserMenuExtendedItem * , int );
};

}

#endif
