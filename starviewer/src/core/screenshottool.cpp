/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "screenshottool.h"
#include "q2dviewer.h"
#include "q3dviewer.h"
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

namespace udg {

ScreenShotTool::ScreenShotTool( Q2DViewer *viewer, QObject *parent )
{
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

ScreenShotTool::ScreenShotTool( Q3DViewer *viewer, QObject *parent )
{
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

ScreenShotTool::ScreenShotTool( Q3DMPRViewer *viewer, QObject *parent )
{
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
    saveAsDialog.setDirectory( QDir::homePath() );
    QStringList filters;
    filters << tr("PNG (*.png)") << tr("Jpeg (*.jpg)") << tr("BMP (*.bmp)");
    saveAsDialog.setFilters( filters );
    saveAsDialog.setFileMode( QFileDialog::AnyFile );
    saveAsDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveAsDialog.setConfirmOverwrite( true );

    QStringList fileNames;
    if( saveAsDialog.exec() )
        fileNames = saveAsDialog.selectedFiles();

    if( fileNames.isEmpty() )
        return;
    QString fileName = fileNames.first();

    vtkImageWriter *imageWriter;
    QString pattern( ("%s-%d.") );
    if( saveAsDialog.selectedFilter() == tr("PNG (*.png)") )
    {
        imageWriter = vtkPNGWriter::New();
        pattern += "png";
    }
    else if( saveAsDialog.selectedFilter() == tr("Jpeg (*.jpg)") )
    {
        imageWriter = vtkJPEGWriter::New();
        pattern += "jpg";
    }
    else if( saveAsDialog.selectedFilter() == tr("BMP (*.bmp)") )
    {
        imageWriter = vtkBMPWriter::New();
        pattern += "bmp";
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
}

}
