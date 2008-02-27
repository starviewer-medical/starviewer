/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qvolume3dviewtestingextension.h"
#include "volume.h"
#include "toolmanager.h"
#include "transferfunctionio.h"
#include "qcluteditordialog.h"
// qt
#include <QAction>
#include <QSettings>
// vtk
#include <vtkImageData.h>

namespace udg {

QVolume3DViewTestingExtension::QVolume3DViewTestingExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    initializeTools();
    loadClutPresets();
    createConnections();
    readSettings();

    m_clutEditorDialog = 0;
}

QVolume3DViewTestingExtension::~QVolume3DViewTestingExtension()
{
    writeSettings();
}

void QVolume3DViewTestingExtension::initializeTools()
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
    m_toolManager->setViewerTools( m_3DView, toolsList );
}

void QVolume3DViewTestingExtension::loadClutPresets()
{
    disconnect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );

    m_clutPresetsComboBox->clear();

    QString path = QDir::homePath() + "/.starviewer/cluts";
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

    m_clutPresetsComboBox->addItems( m_clutNameToFileName.keys() );
    m_clutPresetsComboBox->setCurrentIndex( -1 );

    connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );
}

void QVolume3DViewTestingExtension::createConnections()
{
    // orientacions axial,sagital i coronal
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToCoronal() ) );

    // actualització del mètode de rendering
    connect( m_renderingMethodComboBox, SIGNAL( activated(int) ), SLOT( updateRenderingMethodFromCombo(int) ) );

    connect( m_clutEditToolButton, SIGNAL( clicked() ), SLOT( showClutEditorDialog() ) );
}

void QVolume3DViewTestingExtension::setInput( Volume * input )
{
    m_input = input;
    m_3DView->setInput( m_input );
    m_3DView->render();

    double range[2];
    input->getVtkData()->GetScalarRange( range );
    m_maximumValue = range[1];
}

void QVolume3DViewTestingExtension::updateRenderingMethodFromCombo( int index )
{
    this->setCursor( QCursor(Qt::WaitCursor) );
    switch( index )
    {
    case 0:
        m_3DView->setRenderFunctionToRayCasting();
    break;

    case 1:
        m_3DView->setRenderFunctionToMIP3D();
    break;

    case 2:
        m_3DView->setRenderFunctionToTexture3D();
    break;

    case 3:
        m_3DView->setRenderFunctionToTexture2D();
    break;

    case 4:
        m_3DView->setRenderFunctionToIsoSurface();
    break;
    }
    m_3DView->render();
    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewTestingExtension::applyPresetClut( const QString & clutName )
{
    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( fileName ) );
    if ( transferFunction )
    {
        m_3DView->setTransferFunction( transferFunction );
    }
    m_3DView->render();
}

void QVolume3DViewTestingExtension::showClutEditorDialog()
{
    if ( m_clutEditorDialog ) return;

    m_clutEditorDialog = new QClutEditorDialog( this );
    m_clutEditorDialog->setCluts( m_clutsDir, m_clutNameToFileName );
    m_clutEditorDialog->setMaximum( m_maximumValue );
    m_clutEditorDialog->show();

    connect( m_clutEditorDialog, SIGNAL( clutApplied(const TransferFunction&) ), SLOT( applyClut(const TransferFunction&) ) );
    connect( m_clutEditorDialog, SIGNAL( rejected() ), SLOT( manageClosedDialog() ) );
}

void QVolume3DViewTestingExtension::applyClut( const TransferFunction & clut )
{
    m_3DView->setTransferFunction( new TransferFunction( clut ) );
    m_3DView->render();
}

void QVolume3DViewTestingExtension::manageClosedDialog()
{
    m_clutEditorDialog = 0;
    loadClutPresets();
}

void QVolume3DViewTestingExtension::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-3DTesting");
    settings.endGroup();
}

void QVolume3DViewTestingExtension::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-3DTesting");
    settings.endGroup();
}

}

