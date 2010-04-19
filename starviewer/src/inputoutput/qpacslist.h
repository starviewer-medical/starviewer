/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPACSLIST_H
#define UDGQPACSLIST_H

#include "ui_qpacslistbase.h"
#include <QWidget>
#include <QList>
#include "pacsdevicemanager.h"

namespace udg {

class Status;
class PacsDevice;

/** Interfície que mostra els PACS els quals es pot connectar l'aplicació, permet seleccionar quins es vol connectar l'usuari
@author marc
*/

class QPacsList : public QWidget, private Ui::QPacsListBase
{
Q_OBJECT
public:

    ///Constructor de la classe
    QPacsList(QWidget *parent = 0 );

    /// Retorna els pacs seleccionats per l'usuari
    QList<PacsDevice> getSelectedPacs();

    ///Neteja els elements seleccionats de la llista
    void clearSelection();

    ///Destructor de la classe
    ~QPacsList();
    
    ///Especifica/Retorna per quin Filtre es filtren els PACS mostrats, per defecte si no s'especifica no es filtra.
    void setFilterPACSByService(PacsDeviceManager::FilterPACSByService);
    PacsDeviceManager::FilterPACSByService getFilterPACSByService();

    ///Especifica/Retorna si els PACS que estan marcats com a PACS per cercar per defecte s'han de mostrar ressaltats. Per defecte s'hi mostren
    void setShowQueryPacsDefaultHighlighted(bool showHighlighted);
    bool getShowQueryPacsDefaultHighlighted();

public slots:
    /// Carrega al ListView la Llista de Pacs disponibles
    void refresh();

private slots:
    /// Fa que el PACS que està en l'item del Tree Widget quedi com a un dels PACS per defecte
    /// si aquest element està seleccionat per l'usuari
    void setDefaultPACS(QTreeWidgetItem *item);

private:

    PacsDeviceManager::FilterPACSByService m_filterPacsByService;
    bool m_showQueryPacsDefaultHighlighted;
};

};

#endif
