/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "screenshottool.h"
#include "qviewer.h"
#include "q3dmprviewer.h"
#include "logging.h"

#include <vtkCommand.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QString>

namespace udg {

ScreenShotTool::ScreenShotTool( QViewer *viewer, QObject *parent ) : Tool(viewer,parent)
{
    m_toolName = "ScreenShotTool";
    readSettings();

    m_windowToImageFilter = vtkWindowToImageFilter::New();
    if( viewer )
    {
        m_renderWindow = viewer->getRenderWindow();
        if( !m_renderWindow )
            DEBUG_LOG( "La vtkRenderWindow és NUL·LA!" );
        m_windowToImageFilter->SetInput( m_renderWindow );
    }
    else
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
    case vtkCommand::LeftButtonPressEvent:
        this->screenShot();
    break;
    }
}

void ScreenShotTool::screenShot()
{
    QFileDialog *saveAsDialog = new QFileDialog(0);
    saveAsDialog->setWindowTitle( tr("Save screenshot as...") );
    saveAsDialog->setDirectory( m_lastScreenShotPath );
    QStringList filters;
    filters << tr("PNG (*.png)") << tr("Jpeg (*.jpg)") << tr("BMP (*.bmp)");
    saveAsDialog->setFilters( filters );
    saveAsDialog->selectFilter ( m_lastScreenShotExtension );
    saveAsDialog->setFileMode( QFileDialog::AnyFile );
    saveAsDialog->setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog->selectFile( compoundSelectedName() );
    saveAsDialog->setConfirmOverwrite( true );

    QStringList fileNames;
    QString selectedFilter;
    int overWrite = 0;
    QString fileName;

    connect( saveAsDialog, SIGNAL( rejected() ),this, SLOT( userCancellation() ) );

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
        vtkImageWriter *imageWriter;
        QString pattern( ("%s.") );
        if( selectedFilter == tr("PNG (*.png)") )
        {
            imageWriter = vtkPNGWriter::New();
            pattern += "png";
            m_lastScreenShotExtension = tr("PNG (*.png)");
        }
        else if( selectedFilter == tr("Jpeg (*.jpg)") )
        {
            imageWriter = vtkJPEGWriter::New();
            pattern += "jpg";
            m_lastScreenShotExtension = tr("Jpeg (*.jpg)");
        }
        else if( selectedFilter == tr("BMP (*.bmp)") )
        {
            imageWriter = vtkBMPWriter::New();
            pattern += "bmp";
            m_lastScreenShotExtension = tr("BMP (*.bmp)");
        }
        else
        {
            DEBUG_LOG("No coincideix cap patró, no es pot desar la imatge! RETURN!");
            return;
        }



        m_windowToImageFilter->Update();
        m_windowToImageFilter->Modified();
        vtkImageData *image = m_windowToImageFilter->GetOutput();

        imageWriter->SetInput( image );
        imageWriter->SetFilePrefix( qPrintable( fileName ) );
        imageWriter->SetFilePattern( qPrintable( pattern ) );
        imageWriter->Write();

        //guardem el nom de l'ultim fitxer
        m_lastScreenShotName = QFileInfo(fileName).fileName();
    }
}

QString ScreenShotTool::compoundSelectedName()
{
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
    QSettings settings("GGG", "StarViewer-App-ScreenShotTool");
    settings.beginGroup("StarViewer-App-ScreenShotTool");
    m_lastScreenShotPath = settings.value( "defaultSaveFolder", QDir::homePath() ).toString();
    m_lastScreenShotExtension = settings.value( "defaultSaveExtension", "PNG (*.png)" ).toString();
    m_lastScreenShotName = settings.value( "defaultSaveName", "" ).toString();
    settings.endGroup();
}

void ScreenShotTool::writeSettings()
{
    QSettings settings("GGG", "StarViewer-App-ScreenShotTool");
    settings.beginGroup("StarViewer-App-ScreenShotTool");
    settings.setValue("defaultSaveFolder", m_lastScreenShotPath );
    settings.setValue("defaultSaveExtension", m_lastScreenShotExtension );
    settings.setValue("defaultSaveName", m_lastScreenShotName );
    settings.endGroup();
}

void ScreenShotTool:: userCancellation()
{
    m_userCancellation = true;
}

}
