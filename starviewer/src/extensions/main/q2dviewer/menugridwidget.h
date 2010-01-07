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
class QLabel;

namespace udg {

/**
Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

// FWD declarations
class ItemMenu;
class HangingProtocol;

class MenuGridWidget: public QWidget {
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

    void createHangingProtocolsWidget();
    /// Mètode que omple el widget amb les els grids predefinits
    void createPredefinedGrids( int numberSeries );

    /// Mètode que omple el widget amb les els grids predefinits
    void createPredefinedGrids( QStringList list );

    /// Elimina tot el contingut del widget
    void dropContent();

	/// Posa els hanging protocols que ha de representar el menú
	void setHangingItems( QList<HangingProtocol *> listOfCandidates );

    /// Afegeix hanging protocols a la llista
    void addHangingItems( QList<HangingProtocol *> items );

    /// Posa una element que informa que s'estan carregant
    void addSearchingItem();

    /// Informa de si ha de posar un element que informi que s'està carregant o no
    void setSearchingItem( bool state );


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
	ItemMenu * createIcon( const HangingProtocol * hangingProtocol );

    /// Crea el widget que conté una animació i un label que es mostrarà quan estem buscant estudis previs
    /// per trobar hanging protocols potencials a aplicar sobre l'estudi
    void createSearchingWidget();

protected:

    /// Nombre de columnes a mostrar
    int m_maxColumns;

    /// Màxim nombre d'elements a mostrar;
    int m_maxElements;

    /// Grid per mostrar els elements predefinits
    QGridLayout *m_gridLayout;

    /// Llista dels items
    QList<ItemMenu *> m_itemList;

    /// Agrupament del menu
    QWidget *m_predefinedGridWidget;
    QWidget *m_hangingProtocolWidget;
    QWidget *m_tableGridWidget;

    /// Llista de hanging protocols a mostrar
	QList< HangingProtocol * > m_hangingItems;

    /// Fila a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolRow;
    
    /// Columna a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolColumn;

    /// Distribució regular a la zona dels hanging protocols
    QGridLayout * m_gridLayoutHanging;

    /// Indicador de si cal posar l'element de carregant o no
    bool m_putLoadingItem;

    /// Columna a on s'ha col·locat l'element de carregant
	int m_loadingColumn;

    /// Fila a on s'ha col·locat l'element de carregant
	int m_loadingRow;

    /// Widget que informa que s'esta carregant (buscant)
	QWidget * m_searchingWidget;

};

}

#endif
