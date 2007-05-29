/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "qoptimalviewpointextension.h"

#include <iostream>

#include "optimalviewpointdirector.h"
#include "optimalviewpointparameters.h"
#include "optimalviewpointinputparametersform.h"



namespace udg {



QOptimalViewpointExtension::QOptimalViewpointExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );


    // Creem el director i els paràmetres
    m_optimalViewpointDirector = new OptimalViewpointDirector( m_viewerWidget, this );
    m_optimalViewpointParameters = new OptimalViewpointParameters( this );

    // Li assignem al director quins són els seus paràmetres
    m_optimalViewpointDirector->setParameters( m_optimalViewpointParameters );

    // Creem els widgets que aniran al toolbox
    QHBoxLayout * controlLayout = new QHBoxLayout( m_controlWidget );
    m_optimalViewpointInputParametersForm = new OptimalViewpointInputParametersForm( m_controlWidget );
    controlLayout->addWidget( m_optimalViewpointInputParametersForm );
    controlLayout->setMargin( 0 );
//     m_optimalViewpointInputParametersForm->setName( "Optimal Viewpoint Page" );
    // li assignem els paràmetres
    m_optimalViewpointInputParametersForm->setParameters( m_optimalViewpointParameters );

    QHBoxLayout * viewerLayout = new QHBoxLayout( m_viewerWidget );
    viewerLayout->setMargin( 0 );

    // connectem l'acció amb el director
//     m_optimalViewpointAction = new Q3Action( this );
//     m_optimalViewpointAction->setMenuText( tr("&Optimal Viewpoint") );

//     connect( m_optimalViewpointAction, SIGNAL( activated() ), m_optimalViewpointDirector, SLOT( execute() ) );
    connect( m_optimalViewpointInputParametersForm, SIGNAL( executionRequested() ), m_optimalViewpointDirector, SLOT( execute() ) );
    // connectem els paràmetres amb les interfícies
    // quan un paràmetre s'actualitzi, s'actualitzarà a tots els widgets que el tinguin com a input
    connect( m_optimalViewpointParameters, SIGNAL( changed(int) ), m_optimalViewpointInputParametersForm, SLOT( readParameter(int) ) );
}



QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
    std::cout << "destructor QOptimalViewpointExtension" << std::endl;
}



void QOptimalViewpointExtension::setInput( Volume * input )
{
    m_optimalViewpointParameters->setVolumeObject( input );
//     m_optimalViewpointInputParametersForm->readParameter( OptimalViewpointParameters::VolumeObject );
}



}

