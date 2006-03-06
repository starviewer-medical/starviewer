/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQPACSLIST_H
#define UDGQPACSLIST_H

#include <qpacslistbase.h>

namespace udg {

/** Interfície que mostra els PACS els quals es pot connectar l'aplicació, permet seleccionar quins es vol connectar l'usuari
@author marc
*/

class PacsList;
class Status;
class QPacsList : public QPacsListBase
{
Q_OBJECT

private :
    
    void refresh();
    void setSelectedDefaultPacs();
    void databaseError(Status *);

public:
    QPacsList(QWidget *parent = 0, const char *name = 0);
    Status getSelectedPacs(PacsList *);
    ~QPacsList();

};

};

#endif
