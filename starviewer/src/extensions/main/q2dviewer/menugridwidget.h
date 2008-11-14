/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMENUGRIDWIDGET_H
#define UDGMENUGRIDWIDGET_H

#include <QWidget>

// FWD declarations
class QWidget;
class QGridLayout;
class QGroupBox;
class Math;

namespace udg {

/**
Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

// FWD declarations
class ItemMenu;
class HangingProtocol;

class MenuGridWidget : public QWidget {
Q_OBJECT
public:
    MenuGridWidget( QWidget *parent = 0 );

    ~MenuGridWidget();

    /// Lista que guarda la configuració de grids
    QStringList m_predefinedGridsList;

    /// Màxim nombre de columnes a mostrar els grids predefinits
    void setMaxColumns( int columns );

    /// Màxim nombre d'elements a mostrar
    void setMaxElements( int elements );

    /// Mètode que omple el widget amb les els grids predefinits
    void createPredefinedGrids( int numberSeries );

    /// Mètode que omple el widget amb les els grids predefinits
    void createPredefinedGrids( QStringList list );

    /// Elimina tot el contingut del widget
    void dropContent();

public slots:

    /// Mètode que cada vegada que es seleccioni un dels items emet el grid resultat
    void emitSelected( ItemMenu * selected );

signals:
    
    /// Emet que s'ha escollit un grid
    void selectedGrid( int , int );
	void selectedGrid( int );

protected:

    /// Mètode que crea una icona de rows x columns
    ItemMenu * createIcon( int rows, int columns );

	/// Mètode que crea una icona segons un hanging protocol
	ItemMenu * createIcon( HangingProtocol * hangingProtocol, int hangingProtocolNumber );

protected:

    /// Nombre de columnes a mostrar
    int m_maxColumns;

    /// Màxim nombre d'elements a mostrar;
    int m_maxElements;

    /// Grid per mostrar els elements predefinits
    QGridLayout * m_gridLayout;

    /// Llista dels items
    QList<ItemMenu *> * m_itemList;

    /// Agrupament del menu
    QGroupBox * m_predefinedGridBox;
    QGroupBox * m_hangingProtocolBox;
    QGroupBox * m_tableGridBox;
};

}

#endif
