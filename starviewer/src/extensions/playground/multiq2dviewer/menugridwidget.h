/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMENUGRIDWIDGET_H
#define UDGMENUGRIDWIDGET_H

#include <ui_menugridwidgetbase.h>
#include <itemmenu.h>
#include <QWidget>

namespace udg {

/**
Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MenuGridWidget : public QWidget, public Ui::MenuGridWidgetBase {
Q_OBJECT
public:
    MenuGridWidget( QWidget *parent = 0 );

    ~MenuGridWidget();

    /// Lista que guarda la configuració de grids
    QStringList m_predefinedGridsList;

    /// Màxim nombre de columnes a mostrar els grids predefinits
    void setMaxColumns( int columns );

protected:

    /// Mètode que omple el widget amb les els grids predefinits
    void createPredefinedGrids( QStringList list );

    /// Mètode que crea una icona de rows x columns
    ItemMenu * createIcon( int rows, int columns );

    /// Nombre de columnes a mostrar
    int m_maxColumns;

public slots:

    /// Mètode que cada vegada que es seleccioni un dels items emet el grid resultat
    void emitSelected( ItemMenu * selected );

signals:
    
    /// Emet que s'ha escollit un grid
    void selectedGrid( int , int );
};

}

#endif
