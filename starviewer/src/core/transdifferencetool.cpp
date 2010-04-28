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
    m_state = None;
    m_toolName = "TransDifferenceTool";
    m_hasPersistentData = true;

    m_myData = new TransDifferenceToolData;

    //DEBUG_LOG("Tool creada");

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
        if( m_state == Moving && m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
            this->doTransDifference();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        if(m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
            this->endTransDifference();
    break;
    case vtkCommand::KeyPressEvent:
    {
        if(m_myData->getInputVolume()!= 0 && m_myData->getDifferenceVolume() != 0)
        {
            int key = m_viewer->getInteractor()->GetKeyCode();
            switch( key )
            {
            case 114: // 'R'
            case 82: // 'r'
                this->increaseSingleDifferenceImage(0, -1);
                m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(),-1);
            break;
            case 99: // 'C'
            case 67: // 'c'
                this->increaseSingleDifferenceImage(0, 1);
                m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(),1);
            break;
            case 102: // 'F'
            case 70:  // 'f'
                this->increaseSingleDifferenceImage(1, 0);
                m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(),1);
            break;
            case 100: // 'D'
            case 68:  // 'd'
                this->increaseSingleDifferenceImage(-1, 0);
                m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(),-1);
            break;
            }
        }
    }
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
    m_state = Moving;
    m_viewer->getEventPosition( m_startPosition );
    m_dx=0;
    m_dy=0;
}

void TransDifferenceTool::doTransDifference()
{
    //TODO: canviar imatge
	m_viewer->setCursor( QCursor(QPixmap(":/images/moveDifference.png")) );

    int currentPosition[2];
    m_viewer->getEventPosition( currentPosition );

    // Compute normalized delta
    m_dx = (int)( currentPosition[0] - m_startPosition[0]);
    m_dy = (int)( m_startPosition[1] - currentPosition[1]);

    this->increaseSingleDifferenceImage( m_dx, m_dy);

	m_viewer->refresh(); // necessari perquè es torni a renderitzar a alta resolució en el 3D
}

void TransDifferenceTool::endTransDifference()
{
    //DEBUG_LOG("End moving");
    m_viewer->setCursor( Qt::ArrowCursor );
    m_state = None;
    m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(),m_dx);
    m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(),m_dy);
    m_viewer->refresh(); // necessari perquè es torni a renderitzar a alta resolució en el 3D
}

void TransDifferenceTool::initializeDifferenceImage( )
{
    if(m_myData->getInputVolume() == 0)
    {
        DEBUG_LOG("ERROR: Estem inicialitzant la diferència sense tenir input!");
        return;
    }

    //Actualitzem el vector de translacions (inicialitzem a 0)
    //Potser caldria aplicar les mateixes translacions que ja hi ha?
    //m_sliceTranslations = QVector<QPair<int,int > >(mainVolume->getDimensions()[2],QPair<int,int>(0,0));

    Volume* mainVolume = m_myData->getInputVolume();
    Volume* differenceVolume = m_myData->getDifferenceVolume();

    int ext[6];
    mainVolume->getWholeExtent(ext);

    //Si no hi ha volume diferència
    if(differenceVolume == 0){
        //Allocating memory for the output image
        vtkImageData *imdif = vtkImageData::New();
        //imdif->CopyInformation(m_mainVolume->getVtkData());
        imdif->CopyTypeSpecificInformation(mainVolume->getVtkData());
        imdif->SetExtent(ext);

    	//Converting the VTK data to volume
        differenceVolume = new Volume();
        differenceVolume->setImages( mainVolume->getImages() );
        differenceVolume->setData(imdif);

        m_myData->setDifferenceVolume(differenceVolume);
    }

	int k;
    for (k=ext[4];k<=ext[5];k++)
    {
        this->computeSingleDifferenceImage(0,0,k);
    }
    
    double range[2];
    differenceVolume->getVtkData()->GetScalarRange(range);
    int max;
    if(-range[0] > range[1]){
        max = -range[0];
    }else{
        max = range[1];
    }

	m_2DViewer->setInput(differenceVolume);
	m_2DViewer->setWindowLevel((double)2*max,0.0);

	m_2DViewer->refresh();
}

void TransDifferenceTool::increaseSingleDifferenceImage(int dx, int dy)
{
    int tx = m_myData->getSliceTranslationX(m_2DViewer->getCurrentSlice()) + dx;
    int ty = m_myData->getSliceTranslationY(m_2DViewer->getCurrentSlice()) + dy;
    this->computeSingleDifferenceImage( tx, ty );
}

void TransDifferenceTool::setSingleDifferenceImage(int dx, int dy)
{
    this->computeSingleDifferenceImage( dx, dy );
    m_myData->setSliceTranslationX(m_2DViewer->getCurrentSlice(), dx);
    m_myData->setSliceTranslationY(m_2DViewer->getCurrentSlice(), dy);
}

void TransDifferenceTool::computeSingleDifferenceImage(int dx, int dy, int slice)
{
    //\TODO: Fer-ho amb un filtre

    //Simplifiquem dient que la translació només pot ser per múltiples del píxel
    //Pintem la diferència al volume a la llesca "slice"
    //ho fem amb vtk pq és més ràpid
    Volume::VoxelType *valueRef, *valueMov, *valueDif;
    int indexRef[3];
    int indexMov[3];
    int indexDif[3];

    int currentSlice;
    //Si no ens han posat slice agafem la que està el visor
    if( slice == -1){
        currentSlice = m_2DViewer->getCurrentSlice();
    }else{
        currentSlice = slice;
    }

    Volume* mainVolume = m_myData->getInputVolume();
    Volume* differenceVolume = m_myData->getDifferenceVolume();

    //les translacions són les que ja hi havia a la llesca més el que ens hem mogut amb el cursor
    int tx = dx;
    int ty = dy;

    //Restem 1 al reference slice perquè aquest considera la primera llesca com la 1
    indexRef[2] = m_myData->getReferenceSlice() - 1;
    indexMov[2] = currentSlice;
    indexDif[2] = currentSlice;

    int size[3];
    mainVolume->getDimensions(size);

    int i,j;
    int imax,imin;
    int jmax,jmin;

    imin = tx < 0 ? 0 : tx ;
    imax = tx < 0 ? size[0]+tx : size[0] ;
    jmin = ty < 0 ? 0 : ty ;
    jmax = ty < 0 ? size[1]+ty : size[1] ;

    if( imin-tx < size[0] && imin < size[0])
    {
        indexRef[0]=imin;
        indexMov[0]=imin-tx;
        //l'index de la diferència el posem a 0 perquè assignem valors a tota la fila
        indexDif[0]=0;
        for(j=jmin;j<jmax;j++)
        {
            indexRef[1]=j;
            indexMov[1]=j-ty;
            indexDif[1]=j;
            valueRef = mainVolume->getScalarPointer(indexRef);
            valueMov = mainVolume->getScalarPointer(indexMov);
            valueDif = differenceVolume->getScalarPointer(indexDif);
            for(i=0;i<imin;i++)
            {
                (*valueDif) = 0;
                valueDif++;
            }
            for(i=imin;i<imax;i++)
            {
                (*valueDif) = (*valueMov) - (*valueRef);
                valueRef++;
                valueMov++;
                valueDif++;
            }
            for(i=imax;i<size[0];i++)
            {
                (*valueDif) = 0;
                valueDif++;
            }
        }
    }
    //Posem 0 a les files que no hem fet perquè es visualitzi més bonic
    indexDif[0]=0;
    //Per evitar que se'ns en vagi de rang
    if(jmin > size[1]) jmin=size[1];

    for(j=0;j<jmin;j++)
    {
        indexDif[1]=j;
        valueDif = differenceVolume->getScalarPointer(indexDif);
        for(i=0;i<size[0];i++)
        {
            (*valueDif) = 0;
            valueDif++;
        }
    }
    if(jmax < 0) jmax = 0;
    for(j=jmax;j<size[1];j++)
    {
        indexDif[1]=j;
        valueDif = differenceVolume->getScalarPointer(indexDif);
        for(i=0;i<size[0];i++)
        {
            (*valueDif) = 0;
            valueDif++;
        }
    }

    //Això ho fem perquè ens refresqui la imatge diferència que hem modificat
    if(slice == -1)
    {
        m_2DViewer->getWindowLevelMapper()->Modified();
        m_2DViewer->refresh();
    }
}

}
