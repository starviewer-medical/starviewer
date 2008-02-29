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

    PatientBrowserMenuExtendedItem(QWidget *parent = 0);

    PatientBrowserMenuExtendedItem( Series * series, QWidget *parent = 0 );

    ~PatientBrowserMenuExtendedItem();

    // Tractament dels events
    bool event( QEvent * event );

public slots:

    /// Assigna la serie a mostrar
    void setSeries(Series * series);

signals:

    /// Signal que s'emet quan es rep l'event de tancar el menu
    void close();

private:

    /// Crea el widget inicial sense cap informació
    void createInitialWidget();

    /// Crea un thumbnail buit, amb un texte apropiat
    QPixmap makeEmptyThumbnail();

private:
    /// Serie a la qual es representa la informació addicional.
    Series * m_series;

    /// Label que conté el text addicional a mostrar
    QLabel * m_text;

    /// Label que conté la icona a mostrar
    QLabel * m_icon;
};

}

#endif
