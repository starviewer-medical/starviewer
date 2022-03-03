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

#include "pacsdevicemanager.h"

namespace udg {

class PacsDeviceModel;

/**
 * @brief The QPacsList class is a UI class showing a list of defined PACS. It can be reused in several places.
 */
class QPacsList : public QWidget, private Ui::QPacsListBase
{
    Q_OBJECT

public:
    explicit QPacsList(QWidget *parent = nullptr);
    ~QPacsList() override;

    /// Retorna els pacs seleccionats per l'usuari
    QList<PacsDevice> getSelectedPacs() const;
    /// Neteja els elements seleccionats de la llista
    void clearSelection();

    /// Especifica/Retorna per quin Filtre es filtren els PACS mostrats, per defecte si no s'especifica no es filtra.
    void setFilterPACSByService(PacsDeviceManager::PacsFilter);

    /// Especifica/Retorna si els PACS que estan marcats com a PACS per cercar per defecte s'han de mostrar ressaltats. Per defecte s'hi mostren
    void setShowQueryPacsDefaultHighlighted(bool showHighlighted);
    bool getShowQueryPacsDefaultHighlighted() const;

public slots:
    /// Carrega al ListView la Llista de Pacs disponibles
    void refresh();

signals:
    /// Emitted when a PACS is selected or deselected.
    void pacsSelectionChanged();

private:
    PacsDeviceModel *m_pacsDeviceModel;
    bool m_showQueryPacsDefaultHighlighted;
};

}

#endif
