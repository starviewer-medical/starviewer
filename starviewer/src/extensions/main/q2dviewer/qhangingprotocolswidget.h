/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGMENUGRIDWIDGET_H
#define UDGMENUGRIDWIDGET_H

#include <QWidget>

// FWD declarations
class QWidget;
class QGridLayout;
class QGroupBox;
class QLabel;

namespace udg {

// FWD declarations
class ItemMenu;
class HangingProtocol;

/**
    Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.
  */
class QHangingProtocolsWidget : public QWidget {
Q_OBJECT
public:
    QHangingProtocolsWidget(QWidget *parent = 0);
    ~QHangingProtocolsWidget();

    /// Elimina tot el contingut del widget
    void dropContent();

    /// Afegeix hanging protocols al menú
    void addHangingItems(const QList<HangingProtocol*> &items);

public slots:
    /// Posa els hanging protocols que ha de representar el menú
    void setHangingItems(const QList<HangingProtocol*> &listOfCandidates);
    
    /// Mètode que cada vegada que es seleccioni un dels items emet el grid resultat
    void emitSelected(ItemMenu *selected);

signals:
    /// Emet que s'ha escollit un grid
    void selectedGrid(int);

protected:
    /// Mètode que crea una icona segons un hanging protocol
    ItemMenu* createIcon(const HangingProtocol *hangingProtocol);

private:
    /// Inicialitza el widget i el deixa apunt per afegir-hi hanging protocols.
    void initializeWidget();

protected:
    /// Nombre de columnes a mostrar
    static const int MaximumNumberOfColumns;

    /// Grid per mostrar els elements predefinits
    QGridLayout *m_gridLayout;

    /// Llista dels items
    QList<ItemMenu*> m_itemList;

    /// Fila a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolRow;

    /// Columna a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolColumn;

    /// Distribució regular a la zona dels hanging protocols
    QGridLayout *m_gridLayoutHanging;

    /// Etiqueta que posarem com a títol del widget
    QLabel *m_caption;

    /// Etiqueta per mostrar quan no hi ha cap hanging protocol disponible per aplicar
    QLabel *m_noHangingProtocolsAvailableLabel;
};

}

#endif
