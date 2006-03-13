/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qviewer.h"

#include "volume.h"

// include's vtk
#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <QtGui/QHBoxLayout>

namespace udg {

QViewer::QViewer( QWidget *parent )
 : QWidget( parent )
{
    m_vtkWidget = new QVTKWidget( this );

    //Afegim el layout
    QHBoxLayout* viewerLayout = new QHBoxLayout( this );
    viewerLayout->setSpacing(0);
    viewerLayout->setMargin(0);
    viewerLayout->addWidget( m_vtkWidget );
    
    // això ho fem perquè la finestra buida quedi en negre en un principi
    m_vtkWidget->GetRenderWindow()->Render();
      
    // inicialitzem el punter del volum
    m_mainVolume = 0;
    
    m_modelPointFromCursor.setValues( -1, -1 , -1 );
    
    // 2x buffer
    m_vtkWidget->GetRenderWindow()->DoubleBufferOn();
    
    m_currentCursorPosition[0] = 0;
    m_currentCursorPosition[1] = 0;
    m_currentCursorPosition[2] = 0;
    
    m_currentImageValue = -1;
}


QViewer::~QViewer()
{
}

void QViewer::computeDisplayToWorld( vtkRenderer *renderer , double x , double y , double z , double worldPoint[3] ) 
{
    if( renderer )
    {
        renderer->SetDisplayPoint( x, y, z );
        renderer->DisplayToWorld();
        renderer->GetWorldPoint( worldPoint );
        if ( worldPoint[3] )
        {
            worldPoint[0] /= worldPoint[3];
            worldPoint[1] /= worldPoint[3];
            worldPoint[2] /= worldPoint[3];
            worldPoint[3] = 1.0;
        }
    }
}  

void QViewer::computeWorldToDisplay( vtkRenderer *renderer , double x , double y , double z , double displayPoint[3] )
{
    if( renderer )
    {
        renderer->SetWorldPoint(x, y, z, 1.0);
        renderer->WorldToDisplay();
        renderer->GetDisplayPoint( displayPoint );
    }
}

};  // end namespace udg 
