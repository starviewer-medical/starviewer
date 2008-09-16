/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html?langu=uk                           *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qcluteditordialog.h"

#include "transferfunctionio.h"
// qt
#include <QFileDialog>
#include <QSettings>


namespace udg {


QClutEditorDialog::QClutEditorDialog( QWidget * parent )
    : QDialog( parent )
{
    setupUi( this );

    setAttribute( Qt::WA_DeleteOnClose );

    connect( m_loadClutPushButton, SIGNAL( clicked() ), SLOT( loadClut() ) );
    connect( m_saveClutPushButton, SIGNAL( clicked() ), SLOT( saveClut() ) );
    connect( m_buttonBox, SIGNAL( clicked(QAbstractButton*) ), SLOT( manageClick(QAbstractButton*) ) );
    connect( m_switchEditorPushButton, SIGNAL( clicked() ), SLOT( switchEditor() ) );
}


QClutEditorDialog::~QClutEditorDialog()
{
}


void QClutEditorDialog::setCluts( const QDir & clutsDir, const QMap<QString, QString> & clutNameToFileName )
{
    m_clutsDir = clutsDir;
    m_clutNameToFileName = clutNameToFileName;

    m_clutPresetsComboBox->addItems( m_clutNameToFileName.keys() );
    m_clutPresetsComboBox->setCurrentIndex( -1 );

    connect( m_clutPresetsComboBox, SIGNAL( currentIndexChanged(const QString&) ), SLOT( loadPresetClut(const QString&) ) );
}


void QClutEditorDialog::setCurrentClut( const TransferFunction &currentClut )
{
    m_gradientEditor->setTransferFunction( currentClut );
    m_editorByValues->setTransferFunction( currentClut );
    m_clutPresetsComboBox->setCurrentIndex( m_clutPresetsComboBox->findText( currentClut.name() ) );
}


void QClutEditorDialog::setMaximum( unsigned short maximum )
{
    m_gradientEditor->setMaximum( maximum );
    m_editorByValues->setMaximum( maximum );
}


void QClutEditorDialog::loadPresetClut( const QString & clutName )
{
    const QString & fileName = m_clutNameToFileName[clutName];
    TransferFunction * transferFunction = TransferFunctionIO::fromFile( m_clutsDir.absoluteFilePath( fileName ) );
    if ( transferFunction )
    {
        QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
        currentEditor->setTransferFunction( *transferFunction );
        delete transferFunction;
    }
}


void QClutEditorDialog::loadClut()
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

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "customClutsDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QClutEditorDialog::saveClut()
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


void QClutEditorDialog::manageClick( QAbstractButton * button )
{
    QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );

    switch ( m_buttonBox->buttonRole( button ) )
    {
        case 
            QDialogButtonBox::ApplyRole: emit clutApplied( currentEditor->getTransferFunction() ); 
        break;
        
        default: 
        break;
    }
}


void QClutEditorDialog::switchEditor()
{
    QTransferFunctionEditor * currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
    const TransferFunction & currentTransferFunction = currentEditor->getTransferFunction();
    m_editorsStackedWidget->setCurrentIndex( 1 - m_editorsStackedWidget->currentIndex() );
    currentEditor = qobject_cast<QTransferFunctionEditor*>( m_editorsStackedWidget->currentWidget() );
    currentEditor->setTransferFunction( currentTransferFunction );
}


}
