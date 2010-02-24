/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qangiosubstractionextension.h"

#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "series.h"
#include "image.h"
#include "study.h"
#include "patient.h"
#include "toolmanager.h"
#include "patientbrowsermenu.h"
#include "angiosubstractionsettings.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QContextMenuEvent>

// VTK
#include <vtkCommand.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>

// ITK
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkCastImageFilter.h>

namespace udg {

QAngioSubstractionExtension::QAngioSubstractionExtension( QWidget *parent )
 : QWidget( parent ), m_mainVolume(0), m_differenceVolume(0)
{
    setupUi( this );
    AngioSubstractionSettings().init();

    initializeTools();
    createConnections();
    readSettings();
}

QAngioSubstractionExtension::~QAngioSubstractionExtension()
{
    delete m_toolManager;
    writeSettings();
}

void QAngioSubstractionExtension::initializeTools()
{
    m_toolManager = new ToolManager(this);
    // Registrem les tools
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_translateToolButton->setDefaultAction( m_toolManager->registerTool("TranslateTool") );
    m_windowLevelToolButton->setDefaultAction( m_toolManager->registerTool("WindowLevelTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->registerTool("ScreenShotTool") );
    m_toolManager->registerTool("SynchronizeTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "SlicingTool" << "TranslateTool" << "WindowLevelTool" << "ScreenShotTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools( defaultTools );

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // posem a punt les tools pels visors
    m_toolManager->setupRegisteredTools( m_2DView_1->getViewer() );
    m_toolManager->setupRegisteredTools( m_2DView_2->getViewer() );

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool("RotateClockWiseActionTool") );
    m_toolManager->enableRegisteredActionTools( m_2DView_1->getViewer() );
    m_toolManager->enableRegisteredActionTools( m_2DView_2->getViewer() );
}

void QAngioSubstractionExtension::createConnections()
{
	//Només es pot canviar l'input d'un dels viewers (el de l'input)
    disconnect( m_2DView_1->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_2DView_1->getViewer(), SLOT( setInput( Volume * ) ) );
    disconnect( m_2DView_2->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), m_2DView_2->getViewer(), SLOT( setInput( Volume * ) ) );
    connect( m_2DView_1->getViewer()->getPatientBrowserMenu(), SIGNAL( selectedVolume(Volume *) ), SLOT( setInput( Volume * ) ) );
}

void QAngioSubstractionExtension::setInput( Volume *input )
{
    m_mainVolume = input;

	//eliminem l'anterior m_differenceVolume si n'hi ha
	if(m_differenceVolume){
		delete m_differenceVolume;
		m_differenceVolume = 0;
	}


	//Compute the new difference Volume
	m_2DView_1->getViewer()->setInput(m_mainVolume);
	m_imageSelectorSpinBox->setMinimum(0);
	m_imageSelectorSpinBox->setMaximum(m_mainVolume->getDimensions()[2] - 1);

	computeDifferenceImage( m_imageSelectorSpinBox->value() );

	m_2DView_2->getViewer()->setInput(m_differenceVolume);

	m_2DView_1->setSynchronized( true );
	m_2DView_2->setSynchronized( true );

	m_2DView_1->getViewer()->refresh();
	m_2DView_2->getViewer()->refresh();
}

void QAngioSubstractionExtension::computeDifferenceImage( int imageid )
{
	int i,j,k;
    //Allocating memory for the output image
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_mainVolume->getItkData()->GetBufferedRegion().GetSize();
    region.SetSize(size);
    region.SetIndex(start);
    Volume::ItkImageType::Pointer difImage = Volume::ItkImageType::New();
    difImage->SetRegions( region );
    difImage->Allocate();

	typedef itk::ImageRegionIterator<Volume::ItkImageType> Iterator;
    Iterator difIter( difImage, difImage->GetBufferedRegion() );
    Iterator imIter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );
	typedef itk::ImageRegionIteratorWithIndex<Volume::ItkImageType> IteratorWithIndex;
    IteratorWithIndex baseIter( m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion() );

    Volume::ItkImageType::IndexType initialSlice;
	initialSlice[0] = start[0];
	initialSlice[1] = start[1];
	initialSlice[2] = start[2] + imageid;

	difIter.GoToBegin();
    imIter.GoToBegin();
    for (k=0;k<size[2];k++)
    {
	    baseIter.SetIndex(initialSlice);
        for (j=0;j<size[1];j++)
        {
            for (i=0;i<size[0];i++)
            {
                difIter.Set(abs(imIter.Get()-baseIter.Get()));
                ++difIter;
                ++imIter;
                ++baseIter;
            }
        }
    }
	

	//Converting the ITK data to volume
	if(m_differenceVolume == 0){
		m_differenceVolume = new Volume();
		m_differenceVolume->setImages( m_mainVolume->getImages() );
	}
    m_differenceVolume->setData(difImage);

}

void QAngioSubstractionExtension::angioEventHandler( unsigned long id )
{
    switch( id )
    {
    case vtkCommand::MouseMoveEvent:
        //setPaintCursor();
    break;

    case vtkCommand::LeftButtonPressEvent:
        //leftButtonEventHandler();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        //setLeftButtonOff();
    break;

    case vtkCommand::RightButtonPressEvent:
    break;

    default:
    break;
    }

}

void QAngioSubstractionExtension::readSettings()
{
//    Settings settings;
}

void QAngioSubstractionExtension::writeSettings()
{
//    Settings settings;
}

}
