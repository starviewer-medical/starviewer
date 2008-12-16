/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "screenshottool.h"
#include "qviewer.h"
#include "q2dviewer.h"
#include "logging.h"
// definicions globals d'aplicació
#include "starviewerapplication.h"
// vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
// Qt
#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QString>
#include <QApplication> // pel "wait cursor"

namespace udg {

const QString ScreenShotTool::PngFileFilter = tr("PNG (*.png)");
const QString ScreenShotTool::JpegFileFilter = tr("Jpeg (*.jpg)");
const QString ScreenShotTool::BmpFileFilter = tr("BMP (*.bmp)");

ScreenShotTool::ScreenShotTool( QViewer *viewer, QObject *parent ) : Tool(viewer,parent)
{
    m_toolName = "ScreenShotTool";
    readSettings();
    m_fileExtensionFilters << PngFileFilter << JpegFileFilter << BmpFileFilter;
    if( !viewer )
        DEBUG_LOG( "El viewer proporcionat és NUL!" );
}

ScreenShotTool::~ScreenShotTool()
{
    writeSettings();
}

void ScreenShotTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::KeyPressEvent:
    {
        int key = m_viewer->getInteractor()->GetKeyCode();
        // TODO cal mirar quina és la manera adequada de com gestionar el Ctrl+tecla amb vtk
        // CTRL+s = key code 19
        // CTRL+a = key code 1
        switch( key )
        {
        case 19: // Ctrl+s, "single shot"
            this->screenShot();
        break;

        case 1: // Ctrl+a, "multiple shot"
            this->screenShot(false);
        break;
        }
    }
    break;
    }
}

void ScreenShotTool::screenShot( bool singleShot )
{
    readSettings();
    
    
    

    QFileDialog *saveAsDialog = new QFileDialog(0);
    if( singleShot )
        saveAsDialog->setWindowTitle( tr("Save single screenshot as...") );
    else
        saveAsDialog->setWindowTitle( tr("Save multiple screenshots as...") );

    saveAsDialog->setDirectory( m_lastScreenShotPath );
    saveAsDialog->setFilters( m_fileExtensionFilters );
    saveAsDialog->selectFilter ( m_lastScreenShotExtensionFilter );
    saveAsDialog->setFileMode( QFileDialog::AnyFile );
    saveAsDialog->setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog->selectFile( compoundSelectedName() );
    saveAsDialog->setConfirmOverwrite( true );

    QStringList fileNames;
    QString selectedFilter;
    int overWrite = 0;
    QString fileName;

    connect( saveAsDialog, SIGNAL( rejected() ), SLOT( userCancellation() ) );

    do
    {
        m_userCancellation = false;

        if( saveAsDialog->exec() )
        {
            fileNames = saveAsDialog->selectedFiles();
            selectedFilter = saveAsDialog->selectedFilter();
        }

        if( fileNames.isEmpty() )
        {
            delete saveAsDialog;
            return;
        }

        fileName = fileNames.first();

        //mirem que el nom del fitxer no contingui coses com: nom.png, és a dir, que no es mostri l'extensió
        QString selectedExtension = selectedFilter.mid(selectedFilter.length() - 5, 4);

        if ( fileName.endsWith( selectedExtension ) )
            fileName.remove( fileName.lastIndexOf( selectedExtension ), 4 );

        //guardem l'últim path de la imatge per a saber on hem d'obrir per defecte l'explorador per a guardar el fitxer
        m_lastScreenShotPath = saveAsDialog->directory().path();

        if ( QFileInfo( fileName + selectedFilter.mid(selectedFilter.length() - 5, 4) ).exists() && !m_userCancellation )
        {
            //0 -> Yes; 1->No
            overWrite = QMessageBox::information( 0, tr( "Information" ), tr( "This file already exists. Do you want to replace it?" ), tr( "&Yes" ) , tr( "&No" ) , 0 , 1 );
        }

    }while( overWrite != 0 && !m_userCancellation );

    delete saveAsDialog;

    if ( overWrite == 0 && !m_userCancellation )
    {
        QViewer::FileType fileExtension;
        if( selectedFilter == PngFileFilter )
        {
            fileExtension = QViewer::PNG;
            m_lastScreenShotExtensionFilter = PngFileFilter;
        }
        else if( selectedFilter == JpegFileFilter )
        {
            fileExtension = QViewer::JPEG;
            m_lastScreenShotExtensionFilter = JpegFileFilter;
        }
        else if( selectedFilter == BmpFileFilter )
        {
            fileExtension = QViewer::BMP;
            m_lastScreenShotExtensionFilter = BmpFileFilter;
        }
        else
        {
            DEBUG_LOG("No coincideix cap patró, no es pot desar la imatge! RETURN!");
            return;
        }
        if( singleShot )
        {
            m_viewer->grabCurrentView();
        }
        else
        {
            Q2DViewer *viewer2D = dynamic_cast< Q2DViewer * >( m_viewer );
            if( viewer2D )
            {
                // tenim un  Q2DViewer, llavors podem guardar totes les imatges
                int max = viewer2D->getMaximumSlice();
                // guardem la llesca actual per restaurar
                int currentSlice = viewer2D->getCurrentSlice();
                QApplication::setOverrideCursor( Qt::WaitCursor );
                // recorrem totes les imatges i en fem captura
                for( int i = 0; i < max; i++ )
                {
                    viewer2D->setSlice(i);
                    viewer2D->grabCurrentView();
                }
                viewer2D->setSlice( currentSlice );
            }
            else // tenim un visor que no és 2D, per tant fem un "single shot"
            {
                m_viewer->grabCurrentView();
            }
        }
        // guardem totes les imatges capturades
        m_viewer->saveGrabbedViews( fileName, fileExtension );
        QApplication::restoreOverrideCursor();

        //guardem el nom de l'ultim fitxer
        m_lastScreenShotName = QFileInfo(fileName).fileName();
    }
    writeSettings();
}

QString ScreenShotTool::compoundSelectedName()
{
    // TODO això estaria millor si es fes amb la classe QRegExp,
    // produint un codi molt més net i clar
    QString compoundFile = "";

    if ( !m_lastScreenShotName.isEmpty() )
    {
        QChar lastChar = m_lastScreenShotName[m_lastScreenShotName.length()-1];

        if ( lastChar.isNumber() )
        {
            int i = m_lastScreenShotName.length()-1;

            do
            {
                i--;
                lastChar = m_lastScreenShotName[i];
            }while ( i > 0 && lastChar.isNumber() );

            bool ok;
            int sufix = m_lastScreenShotName.right(m_lastScreenShotName.length()-(i+1)).toInt( &ok, 10 );

            if ( ok )
                compoundFile = m_lastScreenShotName.mid(0, i+1) + QString::number(sufix+1, 10);
            else
                compoundFile = m_lastScreenShotName;
        }
        else
            compoundFile = m_lastScreenShotName + "1";
    }
    return compoundFile;
}

void ScreenShotTool::readSettings()
{
    QSettings settings;
    settings.beginGroup("ScreenshotTool");
    m_lastScreenShotPath = settings.value( "defaultSaveFolder", QDir::homePath() ).toString();
    m_lastScreenShotExtensionFilter = settings.value( "defaultSaveExtension", PngFileFilter ).toString();
    m_lastScreenShotName = settings.value( "defaultSaveName", "" ).toString();
    settings.endGroup();
}

void ScreenShotTool::writeSettings()
{
    QSettings settings;
    settings.beginGroup("ScreenshotTool");
    settings.setValue("defaultSaveFolder", m_lastScreenShotPath );
    settings.setValue("defaultSaveExtension", m_lastScreenShotExtensionFilter );
    settings.setValue("defaultSaveName", m_lastScreenShotName );
    settings.endGroup();
}

void ScreenShotTool:: userCancellation()
{
    m_userCancellation = true;
}

}
