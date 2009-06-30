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

namespace udg {

class Status;
class PacsDevice;

/** Interfície que mostra els PACS els quals es pot connectar l'aplicació, permet seleccionar quins es vol connectar l'usuari
@author marc
*/

class QPacsList : public QWidget, private ::Ui::QPacsListBase
{
Q_OBJECT
public:

    ///Constructor de la classe
    QPacsList(QWidget *parent = 0 );

    /// Retorna els pacs seleccionats per l'usuari
    QList<PacsDevice> getSelectedPacs();

    ///Destructor de la classe
    ~QPacsList();

public slots:

    /// Carrega al ListView la Llista de Pacs disponibles
    void refresh();

};

};

#endif
