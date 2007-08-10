/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWSERMENULIST_H
#define UDGPATIENTBROWSERMENULIST_H

#include <ui_patientbrowsermenubase.h>

#include "patient.h"
#include "study.h"
#include "series.h"
#include <QLabel>
#include <patientbrowsermenubasicitem.h>


namespace udg {

/**
Widget per representar visualment en un menú les dades bàsiques de l'estructura Pacient -> Estudy -> Serie mitjançant widgets seleccionables (PatientBrowserMenuBasicItem).

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuList : public QWidget, public Ui::PatientBrowserMenuBase
{
Q_OBJECT
public:

    PatientBrowserMenuList( QWidget *parent = 0 );

    ~PatientBrowserMenuList();

    /// Posem el pacient al widget
    void setPatient( Patient * patient );

private:
    /// Creem un widget amb la informació bàsica d'un estudi
    QWidget * createStudyWidget( Study * study, QWidget * parent );

    /// Creem un widget amb la informació bàsica d'una sèrie
    PatientBrowserMenuBasicItem * createSerieWidget( Series * serie, QWidget * parent );

signals:

    /// Senyal que s'emet quan el mouse entra en el widget que representa a una serie
    void isActive( int, Series * );

    /// Senyal que s'emet quan no hi ha cap serie activada
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una serie de l'item
    void selectedSerie( Series * );

};

}

#endif
