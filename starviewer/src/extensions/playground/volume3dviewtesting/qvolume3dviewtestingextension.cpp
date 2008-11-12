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
#include "renderingstyle.h"
// qt
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <QStandardItemModel>
// vtk
#include <vtkImageData.h>

namespace udg {

QVolume3DViewTestingExtension::QVolume3DViewTestingExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    initializeTools();
    loadClutPresets();
    loadRenderingStyles();
    createConnections();
    readSettings();

    m_computingObscurance = false;

    m_firstInput = true;

    hideClutEditor();
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
    m_windowLevelToolButton->setDefaultAction( m_toolManager->getToolAction("WindowLevelTool") );
    m_panToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_zoomToolButton->defaultAction()->trigger();
    m_panToolButton->defaultAction()->trigger();
    m_rotate3DToolButton->defaultAction()->trigger();

    // registrem al manager les tools que van amb el viewer principal
    QStringList toolsList;
    toolsList << "ZoomTool" << "TranslateTool" << "Rotate3DTool" << "WindowLevelTool";
    m_toolManager->setViewerTools( m_3DView, toolsList );

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "Rotate3DTool" << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);
}

void QVolume3DViewTestingExtension::loadClutPresets()
{
    DEBUG_LOG( "loadClutPresets()" );

    disconnect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );

    m_clutPresetsComboBox->clear();

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

    m_clutPresetsComboBox->addItems( m_clutNameToFileName.keys() );
    m_clutPresetsComboBox->setCurrentIndex( -1 );

    connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );
}

void QVolume3DViewTestingExtension::loadRenderingStyles()
{
    m_renderingStyleModel = new QStandardItemModel( this );

    // Per la primera versió creem tot això a pèl.
    // Per cada estil creem un item i li assignem unes dades que seran les que es faran servir a l'hora d'aplicar-lo.
    QStandardItem *item;
    RenderingStyle renderingStyle;
    TransferFunction *transferFunction = TransferFunctionIO::fromXmlFile( ":/extensions/Volume3DViewTestingExtension/renderingstyles/wholebody_fullrange.xml" );

    item = new QStandardItem( QIcon( ":/extensions/Volume3DViewTestingExtension/renderingstyles/rs1.png" ), tr("Style 1") );
    renderingStyle.diffuseLighting = false;
    renderingStyle.transferFunction = *transferFunction;
    item->setData( renderingStyle.toVariant() );
    m_renderingStyleModel->appendRow( item );

    item = new QStandardItem( QIcon( ":/extensions/Volume3DViewTestingExtension/renderingstyles/rs2.png" ), tr("Style 2") );
    renderingStyle.diffuseLighting = true;
    renderingStyle.specularLighting = true;
    renderingStyle.specularPower = 64.0;
    renderingStyle.transferFunction = *transferFunction;
    item->setData( renderingStyle.toVariant() );
    m_renderingStyleModel->appendRow( item );

    m_renderingStyleListView->setModel( m_renderingStyleModel );
}

void QVolume3DViewTestingExtension::createConnections()
{
    // orientacions axial,sagital i coronal
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToCoronal() ) );

    // actualització del mètode de rendering
    connect( m_renderingMethodComboBox, SIGNAL( currentIndexChanged(int) ), SLOT( updateRenderingMethodFromCombo(int) ) );

    // mètodes de rendering

    m_shadingOptionsWidget->hide();
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerLabel, SLOT( setEnabled(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_3DView, SLOT( setSpecular(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), this, SLOT( render() ) );
    connect( m_specularPowerDoubleSpinBox, SIGNAL( valueChanged(double) ), m_3DView, SLOT( setSpecularPower(double) ) );
    connect( m_specularPowerDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( render() ) );

    m_obscuranceOptionsWidget->hide();
    m_obscuranceCheckBox->hide(); m_obscuranceFactorLabel->hide(); m_obscuranceFactorDoubleSpinBox->hide();
    connect( m_obscuranceComputeCancelPushButton, SIGNAL( clicked() ), this, SLOT( computeOrCancelObscurance() ) );
    connect( m_3DView, SIGNAL( obscuranceProgress(int) ), m_obscuranceProgressBar, SLOT( setValue(int) ) );
    connect( m_3DView, SIGNAL( obscuranceComputed() ), this, SLOT( endComputeObscurance() ) );
    connect( m_3DView, SIGNAL( obscuranceCancelledByProgram() ), this, SLOT( autoCancelObscurance() ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorLabel, SLOT( setEnabled(bool ) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_3DView, SLOT( setObscurance(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), this, SLOT( render() ) );
    connect( m_obscuranceFactorDoubleSpinBox, SIGNAL( valueChanged(double) ), m_3DView, SLOT( setObscuranceFactor(double) ) );
    connect( m_obscuranceFactorDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( render() ) );

    m_isosurfaceOptionsWidget->hide();
    connect( m_isoValueSpinBox, SIGNAL( valueChanged(int) ), m_3DView, SLOT( setIsoValue(int) ) );
    connect( m_isoValueSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( render() ) );

    // clut editor
    connect( m_loadClutPushButton, SIGNAL( clicked() ), SLOT( loadClut() ) );
    connect( m_saveClutPushButton, SIGNAL( clicked() ), SLOT( saveClut() ) );
    connect( m_switchEditorPushButton, SIGNAL( clicked() ), SLOT( switchEditor() ) );
    connect( m_applyPushButton, SIGNAL( clicked() ), SLOT( applyEditorClut() ) );

    connect( m_customStyleToolButton, SIGNAL( clicked() ), SLOT( toggleClutEditor() ) );
    connect( m_hidePushButton, SIGNAL( clicked() ), SLOT( hideClutEditor() ) );

    connect( m_3DView, SIGNAL( transferFunctionChanged () ), SLOT( changeViewerTransferFunction() ) );
    connect( this, SIGNAL( newTransferFunction () ), m_3DView, SLOT( setNewTransferFunction() ) );

    // visor 3d
    connect( m_3DView, SIGNAL( scalarRange(double,double) ), SLOT( setScalarRange(double,double) ) );

    // rendering styles
    connect( m_renderingStyleListView, SIGNAL( activated(const QModelIndex&) ), SLOT( applyRenderingStyle(const QModelIndex&) ) );
}

void QVolume3DViewTestingExtension::setInput( Volume * input )
{
    m_input = input;
    m_3DView->setInput( m_input );

    applyClut( m_currentClut );
}

void QVolume3DViewTestingExtension::setScalarRange( double min, double max )
{
    unsigned short maximum = static_cast<unsigned short>( qRound( max ) );
    m_gradientEditor->setMaximum( maximum );
    m_editorByValues->setMaximum( maximum );

    if ( m_firstInput )
    {
        m_currentClut.addPoint( min, QColor( 0, 0, 0, 0 ) );
        m_currentClut.addPoint( max, QColor( 255, 255, 255, 255 ) );
        m_firstInput = false;
        emit newTransferFunction ();
    }
}

void QVolume3DViewTestingExtension::updateRenderingMethodFromCombo( int index )
{
    this->setCursor( QCursor(Qt::WaitCursor) );

    m_shadingOptionsWidget->hide();
    m_obscuranceOptionsWidget->hide();
    m_isosurfaceOptionsWidget->hide();

    switch( index )
    {
        case 0:
            m_3DView->setRenderFunctionToRayCasting();
            break;

        case 1:
            m_shadingOptionsWidget->show();
            m_3DView->setRenderFunctionToRayCastingShading();
            break;

        case 2:
            m_obscuranceOptionsWidget->show();
            m_3DView->setRenderFunctionToRayCastingObscurance();
            break;

        case 3:
            m_shadingOptionsWidget->show();
            m_obscuranceOptionsWidget->show();
            m_3DView->setRenderFunctionToRayCastingShadingObscurance();
            break;

        case 4:
            m_3DView->setRenderFunctionToMIP3D();
            break;

        case 5:
            m_3DView->setRenderFunctionToTexture3D();
            break;

        case 6:
            m_3DView->setRenderFunctionToTexture2D();
            break;

        case 7:
            m_isosurfaceOptionsWidget->show();
            m_3DView->setIsoValue( m_isoValueSpinBox->value() );    // necessari per la primera vegada
            m_3DView->setRenderFunctionToIsoSurface();
            break;

        case 8:
            m_3DView->setRenderFunctionToContouring();
            break;
    }

    m_3DView->render();

    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewTestingExtension::applyPresetClut( const QString & clutName )
{
    DEBUG_LOG( "applyPresetClut()" );

    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( QDir::toNativeSeparators( fileName ) ) );
    if ( transferFunction )
    {
        //m_currentClut = *transferFunction;
        applyClut( *transferFunction, true );
        //m_3DView->setTransferFunction( transferFunction );
    }
    //this->render();
}


void QVolume3DViewTestingExtension::applyClut( const TransferFunction & clut, bool preset )
{
    DEBUG_LOG( "applyClut()" );

    m_currentClut = clut;
    if ( !preset )
    {
        // cal fer el disconnect per evitar un bucle infinit
        disconnect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );
        m_clutPresetsComboBox->setCurrentIndex( -1 );
        connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT( applyPresetClut(const QString&) ) );
    }
    m_gradientEditor->setTransferFunction( m_currentClut );
    m_editorByValues->setTransferFunction( m_currentClut );
    m_3DView->setTransferFunction( new TransferFunction( m_currentClut ) );
    emit newTransferFunction();
    this->render();
}

void QVolume3DViewTestingExtension::changeViewerTransferFunction( )
{
    //Actualitzem l'editor de cluts quan es canvia per la funció pel w/l del visor
    m_gradientEditor->setTransferFunction( *(m_3DView->getTransferFunction()) );
    m_editorByValues->setTransferFunction( *(m_3DView->getTransferFunction()) );
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

void QVolume3DViewTestingExtension::computeOrCancelObscurance()
{
    this->setCursor( QCursor(Qt::WaitCursor) );

    if ( !m_computingObscurance )   // no s'estan calculant -> comencem
    {
        m_computingObscurance = true;

        enableObscuranceRendering( false );

        m_obscuranceComputeCancelPushButton->setText( tr("Cancel") );
        m_obscuranceProgressBar->setValue( 0 );

        switch ( m_obscuranceQualityComboBox->currentIndex() )
        {
            case 0: m_3DView->computeObscurance( Q3DViewer::Low ); break;
            case 1: m_3DView->computeObscurance( Q3DViewer::Medium ); break;
            case 2: m_3DView->computeObscurance( Q3DViewer::High ); break;
            default: ERROR_LOG( QString( "Valor inesperat per a la qualitat de les obscurances: %1" ).arg( m_obscuranceQualityComboBox->currentIndex() ) );
        }
    }
    else    // s'estan calculant -> aturem-ho
    {
        m_computingObscurance = false;

        m_obscuranceComputeCancelPushButton->setText( tr("Compute") );

        m_3DView->cancelObscurance();
    }

    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewTestingExtension::autoCancelObscurance()
{
    Q_ASSERT( m_computingObscurance );

    this->setCursor( QCursor(Qt::WaitCursor) );
    m_computingObscurance = false;
    m_obscuranceComputeCancelPushButton->setText( tr("Compute") );
    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewTestingExtension::endComputeObscurance()
{
    m_computingObscurance = false;
    m_obscuranceComputeCancelPushButton->setText( tr("Compute") );
    enableObscuranceRendering( true );
}

void QVolume3DViewTestingExtension::enableObscuranceRendering( bool on )
{
    if ( !on ) m_obscuranceCheckBox->setChecked( false );

    m_obscuranceCheckBox->setEnabled( on );
    m_obscuranceCheckBox->setVisible( on );
    m_obscuranceFactorLabel->setVisible( on );
    m_obscuranceFactorDoubleSpinBox->setVisible( on );
    m_obscuranceProgressBar->setVisible( !on );
}

void QVolume3DViewTestingExtension::render()
{
    this->setCursor( QCursor(Qt::WaitCursor) );
    m_3DView->render();
    this->setCursor( QCursor(Qt::ArrowCursor) );
}

void QVolume3DViewTestingExtension::loadClut()
{
    QSettings settings;
    settings.beginGroup( "Starviewer-App-3DTesting" );
    QString customClutsDirPath = settings.value( "customClutsDir", QString() ).toString();

    QString transferFunctionFileName =
            QFileDialog::getOpenFileName( this, tr("Load CLUT"),
                                          customClutsDirPath, tr("Transfer function files (*.tf);;All files (*)") );

    if ( !transferFunctionFileName.isNull() )
    {
        TransferFunction * transferFunction = TransferFunctionIO::fromFile( transferFunctionFileName );
        QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
        currentEditor->setTransferFunction( *transferFunction );
        delete transferFunction;
        emit newTransferFunction();

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "customClutsDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QVolume3DViewTestingExtension::saveClut()
{
    QSettings settings;
    settings.beginGroup( "Starviewer-App-3DTesting" );
    QString customClutsDirPath = settings.value( "customClutsDir", QString() ).toString();

    QFileDialog saveDialog( this, tr("Save CLUT"), customClutsDirPath, tr("Transfer function files (*.tf);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "tf" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString transferFunctionFileName = saveDialog.selectedFiles().first();
        QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
        TransferFunctionIO::toFile( transferFunctionFileName, currentEditor->getTransferFunction() );

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "customClutsDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QVolume3DViewTestingExtension::switchEditor()
{
    QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
    const TransferFunction & currentTransferFunction = currentEditor->getTransferFunction();
    m_editorsStackedWidget->setCurrentIndex( 1 - m_editorsStackedWidget->currentIndex() );
    currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
    currentEditor->setTransferFunction( currentTransferFunction );
}


void QVolume3DViewTestingExtension::applyEditorClut()
{
    QTransferFunctionEditor *currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
    applyClut( currentEditor->getTransferFunction() );
}


void QVolume3DViewTestingExtension::toggleClutEditor()
{
    if ( m_editorSplitter->sizes()[0] == 0 )    // show
    {
        m_editorSplitter->setSizes( QList<int>() << 1 << 1 );
        m_gradientEditor->setTransferFunction( m_currentClut );
        m_editorByValues->setTransferFunction( m_currentClut );
    }
    else    // hide
        hideClutEditor();
}


void QVolume3DViewTestingExtension::hideClutEditor()
{
    m_editorSplitter->setSizes( QList<int>() << 0 << 0 );
}


void QVolume3DViewTestingExtension::applyRenderingStyle( const QModelIndex &index )
{
    QStandardItem *item = m_renderingStyleModel->itemFromIndex( index );
    RenderingStyle renderingStyle = RenderingStyle::fromVariant( item->data() );

    m_renderingMethodComboBox->setCurrentIndex( renderingStyle.diffuseLighting ? 1 : 0 );

    if ( renderingStyle.diffuseLighting )
    {
        m_specularCheckBox->setChecked( renderingStyle.specularLighting );
        if ( renderingStyle.specularLighting ) m_specularPowerDoubleSpinBox->setValue( renderingStyle.specularPower );
    }

    applyClut( renderingStyle.transferFunction );
}


}

