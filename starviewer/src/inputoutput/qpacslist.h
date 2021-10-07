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

#ifndef UDGQPACSLIST_H
#define UDGQPACSLIST_H

#include "ui_qpacslistbase.h"
#include <QWidget>
#include <QList>
#include "pacsdevicemanager.h"

namespace udg {

class Status;
class PacsDevice;

/**
    Interfície que mostra els PACS els quals es pot connectar l'aplicació, permet seleccionar quins es vol connectar l'usuari
  */
class QPacsList : public QWidget, private Ui::QPacsListBase {
Q_OBJECT
public:
    /// Constructor de la classe
    QPacsList(QWidget *parent = 0);

    /// Retorna els pacs seleccionats per l'usuari
    QList<PacsDevice> getSelectedPacs();

    /// Neteja els elements seleccionats de la llista
    void clearSelection();

    /// Destructor de la classe
    ~QPacsList();

    /// Especifica/Retorna per quin Filtre es filtren els PACS mostrats, per defecte si no s'especifica no es filtra.
    void setFilterPACSByService(PacsDeviceManager::PacsFilter);
    PacsDeviceManager::PacsFilter getFilterPACSByService();

    /// Especifica/Retorna si els PACS que estan marcats com a PACS per cercar per defecte s'han de mostrar ressaltats. Per defecte s'hi mostren
    void setShowQueryPacsDefaultHighlighted(bool showHighlighted);
    bool getShowQueryPacsDefaultHighlighted();

public slots:
    /// Carrega al ListView la Llista de Pacs disponibles
    void refresh();

signals:
    /// Emitted when a PACS is selected or deselected.
    void pacsSelectionChanged();

private slots:
    /// Fa que el PACS que està en l'item del Tree Widget quedi com a un dels PACS per defecte
    /// si aquest element està seleccionat per l'usuari
    void setDefaultPACS(QTreeWidgetItem *item);

private:
    PacsDeviceManager::PacsFilter m_pacsFilter;
    bool m_showQueryPacsDefaultHighlighted;
};

};

#endif
