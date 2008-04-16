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

#include <QFileDialog>
#include <QSettings>

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
    QFileDialog saveAsDialog(0);
    saveAsDialog.setWindowTitle( tr("Save screenshot as...") );
    saveAsDialog.setDirectory( m_lastScreenShotPath );
    QStringList filters;
    filters << tr("PNG (*.png)") << tr("Jpeg (*.jpg)") << tr("BMP (*.bmp)");
    saveAsDialog.setFilters( filters );
    saveAsDialog.selectFilter ( m_lastScreenShotExtension );
    saveAsDialog.setFileMode( QFileDialog::AnyFile );
    saveAsDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog.selectFile( compoundSelectedName() );    
    saveAsDialog.setConfirmOverwrite( true );

    QStringList fileNames;
    if( saveAsDialog.exec() )
        fileNames = saveAsDialog.selectedFiles();

    if( fileNames.isEmpty() )
        return;
    
    QString fileName = fileNames.first();

    vtkImageWriter *imageWriter;
    QString pattern( ("%s.") );
    if( saveAsDialog.selectedFilter() == tr("PNG (*.png)") )
    {
        imageWriter = vtkPNGWriter::New();
        pattern += "png";
        m_lastScreenShotExtension = tr("PNG (*.png)");
    }
    else if( saveAsDialog.selectedFilter() == tr("Jpeg (*.jpg)") )
    {
        imageWriter = vtkJPEGWriter::New();
        pattern += "jpg";
        m_lastScreenShotExtension = tr("Jpeg (*.jpg)");
    }
    else if( saveAsDialog.selectedFilter() == tr("BMP (*.bmp)") )
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
    // \TODO en alguns casos,a la imatge resultant s'hi pinta el diàleg. Mirar de solventar-ho. Una solució seria fer un close del diàleg just abans d'executar el filtre o inclús podríem fer que fos un punter i fer-ne un delete per estar més segurs.
    
    //guardem l'últim path de la imatge per a saber on hem d'obrir per defecte l'explorador per a guardar el fitxer
    m_lastScreenShotPath = saveAsDialog.directory().path();
    
    decodeFileName( fileName );
}

void ScreenShotTool::decodeFileName( QString fileName )
{
    QStringList listUnix = fileName.split("/");
    QStringList listWindows = fileName.split("\\");
    
    if ( !listUnix.isEmpty() )
        m_lastScreenShotName = listUnix.last();
    else if ( !listWindows.isEmpty() )
        m_lastScreenShotName = listWindows.last();
    else
    {
        //en aquesta branca no hauria d'entrar mai
        m_lastScreenShotName = "image";
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

}
