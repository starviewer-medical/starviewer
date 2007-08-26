/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "interactivewidget.h"

#include "patientitemmodel.h"
#include "patientitem.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "logging.h"
#include "patientbrowsermenu.h"

#include <QtGui>
#include <QStringListModel>

namespace udg {

InteractiveWidget::InteractiveWidget( QWidget *parent) : QTreeView(parent)
{
    setAlternatingRowColors( true );
    setMinimumHeight( 25 );
    setMaximumHeight( 25 );
}

InteractiveWidget::~InteractiveWidget()
{
}

void InteractiveWidget::enterEvent( QEvent * event )
{
    setMinimumHeight( 150 );
    setMaximumHeight( 150 );
}

void InteractiveWidget::leaveEvent( QEvent * event )
{
    setMinimumHeight( 25 );
    setMaximumHeight( 25 );
}

void InteractiveWidget::setPatient(Patient * patient)
{
    PatientItemModel * patientModel = new PatientItemModel( patient );

    this->setModel( patientModel->getModel() );
}
}
