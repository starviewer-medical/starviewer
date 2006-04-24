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
using namespace Ui;

namespace udg {

class PacsList;
class Status;

/** Interfície que mostra els PACS els quals es pot connectar l'aplicació, permet seleccionar quins es vol connectar l'usuari
@author marc
*/

class QPacsList : public QWidget, private /*Ui::*/QPacsListBase{
Q_OBJECT
public:
    QPacsList(QWidget *parent = 0 );
    Status getSelectedPacs(PacsList *);
    ~QPacsList();
   
public slots :
    
    void refresh();   
   
private :
    
    void setSelectedDefaultPacs();
    void databaseError(Status *);

};

};

#endif
