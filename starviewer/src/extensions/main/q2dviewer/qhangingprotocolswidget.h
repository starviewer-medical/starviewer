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

class QLabel;

namespace udg {

class HangingProtocol;
class QHangingProtocolsGroupWidget;

/**
    Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.
  */
class QHangingProtocolsWidget : public QWidget {
Q_OBJECT
public:
    QHangingProtocolsWidget(QWidget *parent = 0);
    ~QHangingProtocolsWidget();

public slots:
    /// Posa els hanging protocols que ha de representar el menú
    void setHangingItems(const QList<HangingProtocol*> &listOfCandidates);

signals:
    /// Emet que s'ha escollit un grid
    void selectedGrid(int);

private:
    /// Inicialitza el widget i el deixa apunt per afegir-hi hanging protocols.
    void initializeWidget();

protected:
    /// Nombre de columnes a mostrar
    static const int MaximumNumberOfColumns;

    /// The widget that shows the hanging protocol items.
    QHangingProtocolsGroupWidget *m_hangingProtocolsGroupWidget;

    /// Etiqueta per mostrar quan no hi ha cap hanging protocol disponible per aplicar
    QLabel *m_noHangingProtocolsAvailableLabel;
};

}

#endif
