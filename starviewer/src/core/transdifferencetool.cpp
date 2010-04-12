/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "transdifferencetool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "transdifferencetooldata.h"

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkImageMapToWindowLevelColors.h>

namespace udg {

TransDifferenceTool::TransDifferenceTool( QViewer *viewer, QObject *parent )  
: Tool( viewer, parent )
{
    m_state = NONE;
    m_toolName = "TransDifferenceTool";
    m_hasPersistentData = true;

    m_myData = new TransDifferenceToolData;

    DEBUG_LOG("Tool creada");

    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_viewer );
    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );
}

TransDifferenceTool::~TransDifferenceTool()
{
}

void TransDifferenceTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        if(m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
            this->startTransDifference();
    break;

    case vtkCommand::MouseMoveEvent:
        if( m_state == MOVING && m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
            this->doTransDifference();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        if(m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
            this->endTransDifference();
    break;

    default:
    break;
    }
}

ToolData *TransDifferenceTool::getToolData() const
{
    return m_myData;
}

void TransDifferenceTool::setToolData(ToolData * data)
{
    //Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if( m_myData != data )
    { 
        // creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<TransDifferenceToolData *>(data);
    }
}


void TransDifferenceTool::startTransDifference()
{
    m_state = MOVING;
    DEBUG_LOG("Start moving");
    m_viewer->getEventPosition( m_startPosition );
    m_dx=0;
    m_dy=0;
}

void TransDifferenceTool::doTransDifference()
{
    DEBUG_LOG("Do moving");
    //TODO: canviar imatge
	m_viewer->setCursor( QCursor(QPixmap(":/images/move.png")) );
    m_viewer->getEventPosition( m_currentPosition );

    // Compute normalized delta
    //int dx = (int)((( m_currentPosition[0] - m_startPosition[0]) / size[0]) + 0.5);
    //int dy = (int)((( m_startPosition[1] - m_currentPosition[1]) / size[1]) + 0.5);
    m_dx = (int)( m_currentPosition[0] - m_startPosition[0]);
    m_dy = (int)( m_startPosition[1] - m_currentPosition[1]);

    this->computeDifferenceImage( m_dx, m_dy);

    DEBUG_LOG(QString("Movement: %1, %2: [%3,%4]-[%5,%6]").arg(m_dx).arg(m_dy).arg(m_currentPosition[0]).arg(m_currentPosition[1]).arg(m_startPosition[0]).arg(m_startPosition[1]));

	m_viewer->refresh(); // necessari perquè es torni a renderitzar a alta resolució en el 3D
}

void TransDifferenceTool::endTransDifference()
{
    DEBUG_LOG("End moving");
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = NONE;
    m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(),m_dx);
    m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(),m_dy);
	m_viewer->refresh(); // necessari perquè es torni a renderitzar a alta resolució en el 3D
}

void TransDifferenceTool::computeDifferenceImage(int dx, int dy)
{
    //Simplifiquem dient que la translació només pot ser per múltiples del píxel
    //Pintem la diferència al volume a la llesca "slice"
    //ho fem amb vtk pq és més ràpid
    Volume::VoxelType *valueRef, *valueMov, *valueDif;
    int indexRef[3];
    int indexMov[3];
    int indexDif[3];

    Volume* mainVolume = m_myData->getInputVolume();
    Volume* differenceVolume = m_myData->getDifferenceVolume();

    int currentSlice = m_2DViewer->getCurrentSlice();
    //les translacions són les que ja hi havia a la llesca més el que ens hem mogut amb el cursor
    int tx = m_myData->getSliceTranslationX( currentSlice ) + dx;
    int ty = m_myData->getSliceTranslationY( currentSlice ) + dy;

    indexRef[2] = m_myData->getReferenceSlice();
    indexMov[2] = currentSlice;
    indexDif[2] = currentSlice;

    Volume::ItkImageType::SizeType size = mainVolume->getItkData()->GetBufferedRegion().GetSize();

    int i,j;
    int imax,imin;
    int jmax,jmin;

    imin = tx < 0 ? 0 : tx ;
    imax = tx < 0 ? size[0]+tx : size[0] ;
    jmin = ty < 0 ? 0 : ty ;
    jmax = ty < 0 ? size[1]+ty : size[1] ;

    indexRef[0]=imin;
    indexMov[0]=imin-tx;
    indexDif[0]=imin;
    for(j=jmin;j<jmax;j++)
    {
        indexRef[1]=j;
        indexMov[1]=j-ty;
        indexDif[1]=j;
        valueRef = mainVolume->getScalarPointer(indexRef);
        valueMov = mainVolume->getScalarPointer(indexMov);
        valueDif = differenceVolume->getScalarPointer(indexDif);
        for(i=imin;i<imax;i++)
        {
            (*valueDif) = (*valueMov) - (*valueRef);
            valueRef++;
            valueMov++;
            valueDif++;
        }
    }
    //Això ho fem perquè ens refresqui la imatge diferència que hem modificat
    m_2DViewer->getWindowLevelMapper()->Modified();
    m_2DViewer->refresh();
}



}
