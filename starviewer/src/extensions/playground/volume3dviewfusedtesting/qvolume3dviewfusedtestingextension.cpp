/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qvolume3dviewfusedtestingextension.h"
#include "volume.h"
//#include "volume3dfusedvolume.h"
#include "volume3dfusedviewer.h"
#include "volume3dfusedlight.h"
#include "toolmanager.h"
#include "transferfunctionio.h"
#include "qcluteditordialog.h"

// qt
#include <QAction>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
// vtk
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkMetaImageReader.h>
#include <vtkRenderer.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>

#include <stdlib.h>
#include <iostream.h>
namespace udg {

QVolume3DViewFusedTestingExtension::QVolume3DViewFusedTestingExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    initializeTools();
    loadClutPresets();
    createConnections();
    readSettings();
	
    m_clutEditorDialog = 0;
	m_clutEditorDialog2 = 0;
	m_FusedClutEditorDialog = 0;
	m_mode=0;
}

QVolume3DViewFusedTestingExtension::~QVolume3DViewFusedTestingExtension()
{
    writeSettings();
}

void QVolume3DViewFusedTestingExtension::initializeTools()
{
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_rotate3DToolButton->setDefaultAction( m_toolManager->getToolAction("Rotate3DTool") );
    m_panToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_zoomToolButton->defaultAction()->trigger();
    m_panToolButton->defaultAction()->trigger();
    m_rotate3DToolButton->defaultAction()->trigger();

    // registrem al manager les tools que van amb el viewer principal
    QStringList toolsList;
    toolsList << "ZoomTool" << "TranslateTool" << "Rotate3DTool";
    m_toolManager->setViewerTools( m_volume3DView, toolsList );
	m_toolManager->setViewerTools( m_volume3DViewSecondary, toolsList );
	m_toolManager->setViewerTools( m_volume3DViewFused, toolsList );
	m_applyFusionButton->setEnabled(false);
}

void QVolume3DViewFusedTestingExtension::loadClutPresets()
{
    //disconnect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );

    //m_clutPresetsComboBox->clear();

    QString path = QDir::toNativeSeparators( QDir::homePath() + "/.starviewer/cluts" );
    m_clutsDir.setPath( path );

    if ( !m_clutsDir.exists() ) m_clutsDir.mkpath( path );
    if ( !m_clutsDir.isReadable() ) return; // no es pot accedir al directori

    QStringList nameFilters;
    nameFilters << "*.tf";

    QStringList clutList = m_clutsDir.entryList( nameFilters );

    foreach ( const QString & clutName, clutList )
    {
        TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( clutName ) );
        if ( transferFunction )
        {
            m_clutNameToFileName[transferFunction->name()] = clutName;
            delete transferFunction;
        }
    }

    //m_clutPresetsComboBox->addItems( m_clutNameToFileName.keys() );
    //m_clutPresetsComboBox->setCurrentIndex( -1 );

    //connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );
}

void QVolume3DViewFusedTestingExtension::createConnections()
{
    // orientacions axial,sagital i coronal
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_volume3DView , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_volume3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_volume3DView , SLOT( resetViewToCoronal() ) );
	///3DView secundary
 	connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToCoronal() ) );
	///3Dview Fusionat
	connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_volume3DViewSecondary , SLOT( resetViewToCoronal() ) );

    // actualització del mètode de rendering
    connect( m_renderingMethodComboBox, SIGNAL( activated(int) ), SLOT( updateRenderingMethodFromCombo(int) ) );
    connect( m_fusionMethodComboBox, SIGNAL( activated(int) ), SLOT( changeFusionMethod(int) ) );


    connect( m_clutEditToolButton, SIGNAL( clicked() ), SLOT( showClutEditorDialog() ) );
	connect( m_clutEditToolButton_2, SIGNAL( clicked() ), SLOT( showClutEditorDialog2() ) );

	connect( m_FusedClutEditToolButton, SIGNAL( clicked() ), SLOT( showFusedClutEditorDialog() ) );
	//Obrir el volum secundary
	connect(m_openSecondaryButton, SIGNAL( clicked() ), SLOT( openSecondary() ) );
	//Aplicar la fusió
	connect(m_applyFusionButton, SIGNAL( clicked() ), SLOT( fuse() ) );
//	connect(m_setShadeOnButton_2, SIGNAL( clicked() ), SLOT( shadeOn() ) );

	//connectar el nou botó a un formulari que obri un volum.
}




void QVolume3DViewFusedTestingExtension::setInput( Volume * input )
{

    m_input = input;
	m_volume3DView->setInput( m_input );
	m_volume3DView->render();
	

    double range[2];
    input->getVtkData()->GetScalarRange( range );
    m_maximumValue = (short unsigned int)range[1];
}

void QVolume3DViewFusedTestingExtension::updateRenderingMethodFromCombo( int index )
{
    this->setCursor( QCursor(Qt::WaitCursor) );
    switch( index )
    {
    case 0:
        m_volume3DView->setRenderFunctionToRayCasting();
        m_volume3DViewSecondary->setRenderFunctionToRayCasting();

    break;

    case 1:
        m_volume3DView->setRenderFunctionToMIP3D();
		m_volume3DViewSecondary->setRenderFunctionToMIP3D();
    break;

    case 2:
        m_volume3DView->setRenderFunctionToTexture3D();
        m_volume3DViewSecondary->setRenderFunctionToTexture3D();
    break;

    case 3:
        m_volume3DView->setRenderFunctionToTexture2D();
        m_volume3DViewSecondary->setRenderFunctionToTexture2D();
    break;

    case 4:
        m_volume3DView->setRenderFunctionToIsoSurface();
        m_volume3DViewSecondary->setRenderFunctionToIsoSurface();
    break;
    }
    m_volume3DView->render();
	m_volume3DViewSecondary->render();
    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewFusedTestingExtension::applyPresetClut( const QString & clutName )
{
    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( QDir::toNativeSeparators( fileName ) ) );
    if ( transferFunction )
    {
        m_volume3DView->setTransferFunction( transferFunction );
    }
    m_volume3DView->render();
}

void QVolume3DViewFusedTestingExtension::showClutEditorDialog()
{
    if ( m_clutEditorDialog ) return;

    m_clutEditorDialog = new QClutEditorDialog( this );
    m_clutEditorDialog->setCluts( m_clutsDir, m_clutNameToFileName );
    m_clutEditorDialog->setMaximum( m_maximumValue );
    m_clutEditorDialog->show();

    connect( m_clutEditorDialog, SIGNAL( clutApplied(const TransferFunction&) ), SLOT( applyClut(const TransferFunction&) ) );
    connect( m_clutEditorDialog, SIGNAL( rejected() ), SLOT( manageClosedDialog() ) );
}

void QVolume3DViewFusedTestingExtension::applyClut( const TransferFunction & clut )
{
    m_volume3DView->setTransferFunction( new TransferFunction( clut ) );
    m_volume3DView->render();
}


///SEGON VISUALITZADOR

void QVolume3DViewFusedTestingExtension::applyPresetClut2( const QString & clutName )
{
    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( QDir::toNativeSeparators( fileName ) ) );
    if ( transferFunction )
    {
        m_volume3DViewSecondary->setTransferFunction( transferFunction );
    }
	m_volume3DViewSecondary->render();
}

void QVolume3DViewFusedTestingExtension::showClutEditorDialog2()
{
    if ( m_clutEditorDialog2 ) return;

    m_clutEditorDialog2 = new QClutEditorDialog( this );
    m_clutEditorDialog2->setCluts( m_clutsDir, m_clutNameToFileName );
    m_clutEditorDialog2->setMaximum( m_maximumValue );
    m_clutEditorDialog2->show();

    connect( m_clutEditorDialog2, SIGNAL( clutApplied(const TransferFunction&) ), SLOT( applyClut2(const TransferFunction&) ) );
    connect( m_clutEditorDialog2, SIGNAL( rejected() ), SLOT( manageClosedDialog() ) );
}

void QVolume3DViewFusedTestingExtension::applyClut2( const TransferFunction & clut )
{
    m_volume3DViewSecondary->setTransferFunction( new TransferFunction( clut ) );
    m_volume3DViewSecondary->render();
}










///Copiem els metodes per assignar la funció de transferència del model fusionat
void QVolume3DViewFusedTestingExtension::applyFusedPresetClut( const QString & clutName )
{
    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( QDir::toNativeSeparators( fileName ) ) );
    if ( transferFunction )
    {
        m_volume3DViewFused->setTransferFunction( transferFunction );		
    }
    m_volume3DViewFused->render();
}

void QVolume3DViewFusedTestingExtension::showFusedClutEditorDialog()
{
    if ( m_FusedClutEditorDialog ) return;

    m_FusedClutEditorDialog = new QClutEditorDialog( this );
    m_FusedClutEditorDialog->setCluts( m_clutsDir, m_clutNameToFileName );
    m_FusedClutEditorDialog->setMaximum( m_maximumValue );
    m_FusedClutEditorDialog->show();

    connect( m_FusedClutEditorDialog, SIGNAL( clutApplied(const TransferFunction&) ), SLOT( applyFusedClut(const TransferFunction&) ) );
    connect( m_FusedClutEditorDialog, SIGNAL( rejected() ), SLOT( manageClosedDialog() ) );
}

void QVolume3DViewFusedTestingExtension::applyFusedClut( const TransferFunction & clut )
{
    m_volume3DViewFused->setTransferFunction( new TransferFunction( clut ) );
	
///En la fusió de color s'ha de tenir en compte que no ha de renderitzar un altre cop!!!!
	m_volume3DViewFused->render();	
}

void QVolume3DViewFusedTestingExtension::manageClosedDialog()
{
    m_clutEditorDialog = 0;
	m_FusedClutEditorDialog=0;
    loadClutPresets();
}

void QVolume3DViewFusedTestingExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-3DTesting");
    settings.endGroup();
}

void QVolume3DViewFusedTestingExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-3DTesting");
    settings.endGroup();
}

void QVolume3DViewFusedTestingExtension::openSecondary(){
bool ok;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open secondary file"), m_workingDirectory, tr("MetaImage Files (*.mhd)"));
    QApplication::setOverrideCursor( QCursor ( Qt::WaitCursor ) );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).suffix() != "mhd" )
            fileName += ".mhd";

      m_workingDirectory = QFileInfo( fileName ).absolutePath();

        vtkMetaImageReader *reader = vtkMetaImageReader::New();
        reader->SetFileName(qPrintable( fileName ));

        switch( reader->CanReadFile( qPrintable(fileName) ) )
        {
            case 0: // no és un arxiu mhd :(
                ok = false;
                break;

            case 1: // I think I can read the file but I cannot prove it
            case 2: // I definitely can read the file
            case 3: // I can read the file and I have validated that I am the correct reader for this file
                ok = true;
                break;
        }
		
        reader->Update();
		

        m_secondaryVolume = new Volume(reader->GetOutput());
		m_volume3DViewSecondary->reset();
		
		m_volume3DViewSecondary->setInput( m_secondaryVolume );
		m_volume3DViewSecondary->render();
		reader->Delete();
		
    }
	if(ok)
	{	
		m_applyFusionButton->setEnabled(true);
	}
    QApplication::restoreOverrideCursor();
}

void QVolume3DViewFusedTestingExtension::shadeOn()
{	
//	disconnect(m_setShadeOnButton_2, SIGNAL( clicked() ),this, SLOT( shadeOn() ) );
//	m_setShadeOnButton_2->setText("Shade Off");
	m_volume3DViewFused->setShade(true);
	m_volume3DViewFused->refresh();
	
//	connect(m_setShadeOnButton_2, SIGNAL( clicked() ), SLOT( shadeOff() ) );
}
void QVolume3DViewFusedTestingExtension::shadeOff()
{
//	disconnect(m_setShadeOnButton_2, SIGNAL( clicked() ),this, SLOT( shadeOff() ) );
//	m_setShadeOnButton_2->setText("Shade On");
	m_volume3DViewFused->setShade(false);
	m_volume3DViewFused->refresh();	
//	connect(m_setShadeOnButton_2, SIGNAL( clicked() ), SLOT( shadeOn() ) );
}

void QVolume3DViewFusedTestingExtension::changeFusionMethod(int method)
{
	//this->setCursor( QCursor(Qt::WaitCursor) );
	cout << method << endl;
    switch( method )
    {
    case 0:
		m_mode=0;
		
	break;
	case 1:
		m_mode=1;
		
	break;	
	case 2:
		m_mode=2;
		
    break;
	case 3:
		m_mode=3;
    break;
	case 4:
		m_mode=4;
    break;
	case 5:
		m_mode=5;
    break;
    }
	cout << method << endl;

	//QMessageBox::information(0,"Starviewer", "El mode es: " + m_mode );

    //this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewFusedTestingExtension::fuse()
{
	this->setCursor( QCursor(Qt::WaitCursor) );
    switch( m_mode )
    {
	///
    case 0:
		m_volume3DViewFused->deactivateGradient();
		this->fuseModels();
	break;
	///
	case 1:
		m_volume3DViewFused->deactivateGradient();
		this->fuseModels2();
	break;	
	///
	case 2:
		m_volume3DViewFused->activateGradient();
		this->fuseModels();
		
    break;
	///
	case 3:
		m_volume3DViewFused->activateGradient();
		this->fuseModels2();
    break;
	case 4:
		m_volume3DViewFused->activateGradient();
		this->fuseModels3();
    break;
	case 5:
		m_volume3DViewFused->activateGradient();
		this->fuseModels3();
    break;
    }
	
    this->setCursor( QCursor(Qt::ArrowCursor) );

}

///Fusió intercalant propietats
void QVolume3DViewFusedTestingExtension::fuseModels2()
{
	///Abans reescalem les imatges per si estan fora de rang Aprofitem per fer el casting

	double * range = m_secondaryVolume->getVtkData()->GetScalarRange();
	vtkImageShiftScale* rescale = vtkImageShiftScale::New();
    rescale->SetInput( m_secondaryVolume->getVtkData() );
    rescale->SetShift(0);
    rescale->SetScale( 256.0 / range[1] );
    rescale->SetOutputScalarType( VTK_UNSIGNED_CHAR );
	rescale->Update();
	m_image=rescale->GetOutput();
	m_image->Register(0);
	cout << "image data: " << m_image->GetScalarTypeAsString() << endl;
	///Extreiem el punter dels valors de propietats
	m_data=reinterpret_cast<unsigned char*>(m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	///Obtenim la mida del "vector" de propietats
	m_dataSize= m_image->GetPointData()->GetScalars()->GetSize();
	///Repetim el proces amb el volum primari
	range = m_input->getVtkData()->GetScalarRange();
	vtkImageShiftScale* rescale2 = vtkImageShiftScale::New();
    rescale2->SetInput( m_input->getVtkData() );
    rescale2->SetShift(0);
    rescale2->SetScale( 256.0 / range[1] );
    rescale2->SetOutputScalarType( VTK_UNSIGNED_CHAR );
	rescale2->Update();
	m_image2=rescale2->GetOutput();
	m_image2->Register( 0 );
	rescale->Delete();
	rescale2->Delete();
	//	m_data = reinterpret_cast<unsigned char*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_data2=reinterpret_cast<unsigned char*>(m_image2->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_dataSize2= m_image2->GetPointData()->GetScalars()->GetSize();
	
	///unsigned char* per poder copiar els valors al image del resultat
	unsigned char * fusedData;
	int fusedDataSize;
	unsigned char * fixIt = m_data;
	unsigned char * fixItEnd = m_data + m_dataSize;
	unsigned char * fixIt2 = m_data2;
	//unsigned char * fixItEnd2 = m_data2 + m_dataSize2;

	if(m_dataSize2 == m_dataSize)
	{
		m_imageCopy = vtkImageData::New();   // Si fem new no cal Register
		m_imageCopy->DeepCopy( m_image2 );
		fusedData=reinterpret_cast<unsigned char*>(m_imageCopy->GetPointData()->GetScalars()->GetVoidPointer( 0 ));
		fusedDataSize= m_imageCopy->GetPointData()->GetScalars()->GetSize();
		int i=0;
		unsigned char* cont=fusedData;
		while ( fixIt < fixItEnd )
		{
			if (i==0)
			{
				(*cont)=(*fixIt);
				//cout << "0= " << (*cont) << ":" << (*fixIt) << endl;
				i=1;
			}
			else
			{
				(*cont)=(*fixIt2);
				//cout << "1= " << (*cont) << ":" << (*fixIt2) << endl;
				i=0;
			}
			///És correcte
			fixIt++;
			fixIt2++;
			cont++;
		}
		
		///Això ha de ser amb m_3dview o amb un objecte nou		
		m_fused = new Volume(m_imageCopy);
		//m_volume3DViewFused->reset();
		
		m_volume3DViewFused->setInput(m_fused);
		m_maximumValue=256;
		m_volume3DViewFused->render();
	}

    QApplication::restoreOverrideCursor();
}

///Fusió calculant la mitjana
void QVolume3DViewFusedTestingExtension::fuseModels()
{
	///Abans reescalem les imatges per si estan fora de rang
	double * range = m_secondaryVolume->getVtkData()->GetScalarRange();
	double min = range[0], max = range[1];

	vtkImageShiftScale* rescale = vtkImageShiftScale::New();
    rescale->SetInput( m_secondaryVolume->getVtkData() );
    rescale->SetShift(0);
    rescale->SetScale( 256.0 / range[1] );
    rescale->SetOutputScalarType( VTK_UNSIGNED_CHAR );
	rescale->Update();
	m_image=rescale->GetOutput();
	m_image->Register(0);
	
	///Extreiem el punter dels valors de propietats
	m_data=reinterpret_cast<unsigned char*>(m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	///Obtenim la mida del "vector" de propietats
	
	m_dataSize= m_image->GetPointData()->GetScalars()->GetSize();
	
	///Repetim el proces amb el volum primari
	
	range = m_input->getVtkData()->GetScalarRange();
	
	vtkImageShiftScale* rescale2 = vtkImageShiftScale::New();

    rescale2->SetInput( m_input->getVtkData() );
    rescale2->SetShift(0);
    rescale2->SetScale( 256.0 / range[1] );
    rescale2->SetOutputScalarType( VTK_UNSIGNED_CHAR );
	rescale2->Update();
	
	m_image2=rescale2->GetOutput();
	m_image2->Register( 0 );
	rescale->Delete();
	rescale2->Delete();
	
	//	m_data = reinterpret_cast<unsigned char*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_data2=reinterpret_cast<unsigned char*>(m_image2->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_dataSize2= m_image2->GetPointData()->GetScalars()->GetSize();
	
	
	///unsigned char* per poder copiar els valors al image del resultat
	unsigned char * fusedData;
	int fusedDataSize;

	unsigned char * fixIt = m_data;
	unsigned char * fixItEnd = m_data + m_dataSize;
	unsigned char * fixIt2 = m_data2;
	//unsigned char * fixItEnd2 = m_data2 + m_dataSize2;
///Falta guardar les funcions en un unsigned char*, fer diferents mètodes de fusió(potser després de la funció de transferència), i lo dels voxels shaders.
	if(m_dataSize2 == m_dataSize)
	{
		m_imageCopy = vtkImageData::New();   // Si fem new no cal Register
		m_imageCopy->DeepCopy( m_image2 );
		fusedData=reinterpret_cast<unsigned char*>(m_imageCopy->GetPointData()->GetScalars()->GetVoidPointer( 0 ));
		//cout << "fusedData: " << (unsigned int)(*fusedData) << endl;
		fusedDataSize= m_imageCopy->GetPointData()->GetScalars()->GetSize();
		
		unsigned char* cont=fusedData;

		while ( fixIt < fixItEnd )
		{
			unsigned int value = (((unsigned int)(*fixIt))+((unsigned int)(*fixIt2)))/2;
			(*cont)=(unsigned char)(value);
			///És correcte
			fixIt++;
			fixIt2++;
			cont++;
		}
		
	///Això ha de ser amb m_3dview o amb un objecte nou		
		m_fused = new Volume(m_imageCopy);
		DEBUG_LOG( m_fused->toString());
		//m_volume3DViewFused->reset();
		m_volume3DViewFused->setInput(m_fused);
		m_maximumValue=256;
		//m_volume3DViewFused->showLights();
		m_volume3DViewFused->render();
		
	}
	
    QApplication::restoreOverrideCursor();
}

void QVolume3DViewFusedTestingExtension::userCancellation()
{
    m_userCancellation = true;	
}

void QVolume3DViewFusedTestingExtension::fuseModels3()
{
///canviar el rescalat.

	double * range = m_secondaryVolume->getVtkData()->GetScalarRange();
    double min = range[0], max = range[1];

	///Afegir l'if tal i com ho fa al createImages del opimalViewport.
	if ( min >= 0.0 && max <= 255.0 )
	{
		vtkImageCast *caster = vtkImageCast::New();
        caster->SetInput( m_secondaryVolume->getVtkData() );
        caster->SetOutputScalarTypeToUnsignedChar();
        caster->Update();

        m_image = caster->GetOutput(); m_image->Register( 0 );  // el register és necessari (comprovat)
        caster->Delete();
		
	}	
	else
	{
		double shift = -min;
        double slope = 255.0 / ( max - min );

		vtkImageShiftScale* rescale = vtkImageShiftScale::New();
		rescale->SetInput( m_secondaryVolume->getVtkData() );
		rescale->SetShift( shift );
        rescale->SetScale( slope );
		
		rescale->SetOutputScalarType( VTK_UNSIGNED_CHAR );
		rescale->Update();
		m_image=rescale->GetOutput();
		m_image->Register(0);
		rescale->Delete();

	}

	///Extreiem el punter dels valors de propietats
	m_data=reinterpret_cast<unsigned char*>(m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_dataSize= m_image->GetPointData()->GetScalars()->GetSize();

	range = m_input->getVtkData()->GetScalarRange();
	min = range[0], max = range[1];

	vtkImageShiftScale* rescale2 = vtkImageShiftScale::New();

    rescale2->SetInput( m_input->getVtkData() );
    rescale2->SetShift(0);
    ///rescale2->SetScale( 255.0 / range[1] );
	rescale2->SetScale(1);
    rescale2->SetOutputScalarType( VTK_UNSIGNED_CHAR );
	rescale2->Update();
	
	m_image2=rescale2->GetOutput();
	m_image2->Register( 0 );
	
	rescale2->Delete();
	
	m_data2=reinterpret_cast<unsigned char*>(m_image2->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
	m_dataSize2= m_image2->GetPointData()->GetScalars()->GetSize();
	
	m_imageCopy = vtkImageData::New();   // Si fem new no cal Register
	m_imageCopy->DeepCopy( m_image2 );
	m_fused = new Volume(m_imageCopy);
	
///Falta precalcular el volum
	//m_volume3DViewFused->setInput(m_fused);
	//m_volume3DViewFused->getRenderer()->Render();
	//m_volume3DViewFused->setImage(m_fused->getVtkData());
	m_maximumValue=range[1];
	cout << "crido el fusion" << endl;
	m_volume3DViewFused->fusion(1,range,m_data,m_data2,m_volume3DView->getTransferFunction(),m_volume3DViewSecondary->getTransferFunction());
	
	
	
	cout << "renderitzem" << endl;

}
}

