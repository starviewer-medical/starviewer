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
// qt
#include <QAction>
#include <QSettings>

namespace udg {

QVolume3DViewTestingExtension::QVolume3DViewTestingExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    initializeTools();
    loadClutPresets();
    createConnections();
    readSettings();
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
    m_toolManager->refreshConnections();
}

void QVolume3DViewTestingExtension::loadClutPresets()
{
    m_clutsDir.setPath( QDir::homePath() + "/.starviewer/cluts" );

    if ( !( m_clutsDir.exists() && m_clutsDir.isReadable() ) ) return;  // no es pot accedir al directori

    QStringList nameFilters;
    nameFilters << "*.tf";

    QStringList clutList = m_clutsDir.entryList( nameFilters );

    m_clutPresetsComboBox->addItems( clutList );
}

void QVolume3DViewTestingExtension::createConnections()
{
    // orientacions axial,sagital i coronal
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToCoronal() ) );

    // actualització del mètode de rendering
    connect( m_renderingMethodComboBox, SIGNAL( activated(int) ), SLOT( updateRenderingMethodFromCombo(int) ) );

    // funció de transferència
    connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), SLOT( applyPresetClut(const QString&) ) );
}

void QVolume3DViewTestingExtension::setInput( Volume * input )
{
    m_input = input;
    m_3DView->setInput( m_input );
    m_3DView->render();
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
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( clutName ) );
    if ( transferFunction ) m_3DView->setTransferFunction( transferFunction );
    m_3DView->render();
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

