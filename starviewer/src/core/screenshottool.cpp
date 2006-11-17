/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr√†fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "screenshottool.h"
#include "q2dviewer.h"
#include <vtkWindowToImageFilter.h>
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkRenderWindow.h>

#include <QFileDialog>

namespace udg {

ScreenShotTool::ScreenShotTool( Q2DViewer *viewer , QObject *parent , const char *name )
{
    if( viewer )
        m_renderWindow = viewer->getRenderWindow();
    m_windowToImageFilter = vtkWindowToImageFilter::New();
    m_windowToImageFilter->SetInput( m_renderWindow );
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
    QString fileName = QFileDialog::getSaveFileName( 0, tr("Save screenshot as..."), QString(), tr("PNG (*.png)") );

    if ( fileName.isEmpty() )
        return;

    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();
    vtkImageData *image = m_windowToImageFilter->GetOutput();

    vtkImageWriter *pngWriter = vtkPNGWriter::New();
    pngWriter->SetInput( image );
    pngWriter->SetFilePattern( "%s.png" );
    pngWriter->SetFilePrefix( qPrintable( fileName ) );
    pngWriter->Write();

//     switch( extension )
//     {
//         case PNG:
//         {
//             vtkImageWriter *pngWriter = vtkPNGWriter::New();
//             pngWriter->SetInput( image );
//             pngWriter->SetFilePattern( "%s-%d.png" );
//             pngWriter->SetFilePrefix( baseName );
//             pngWriter->Write();
//
//             break;
//         }
//         case JPEG:
//         {
//             vtkImageWriter *jpegWriter = vtkJPEGWriter::New();
//             jpegWriter->SetInput( image );
//             jpegWriter->SetFilePattern( "%s-%d.jpg" );
//             jpegWriter->SetFilePrefix( baseName );
//             jpegWriter->Write();
//
//             break;
//         }
//         // \TODO el format tiff fa petar al desar, mirar si Ès problema de compatibilitat del sistema o de les prÚpies vtk
//         case TIFF:
//         {
//             vtkImageWriter *tiffWriter = vtkTIFFWriter::New();
//             tiffWriter->SetInput( image );
//             tiffWriter->SetFilePattern( "%s-%d.tif" );
//             tiffWriter->SetFilePrefix( baseName );
//             tiffWriter->Write();
//
//             break;
//         }
//         case PNM:
//         {
//             vtkImageWriter *pnmWriter = vtkPNMWriter::New();
//             pnmWriter->SetInput( image );
//             pnmWriter->SetFilePattern( "%s-%d.pnm" );
//             pnmWriter->SetFilePrefix( baseName );
//             pnmWriter->Write();
//
//             break;
//         }
//         case BMP:
//         {
//             vtkImageWriter *bmpWriter = vtkBMPWriter::New();
//             bmpWriter->SetInput( image );
//             bmpWriter->SetFilePattern( "%s-%d.bmp" );
//             bmpWriter->SetFilePrefix( baseName );
//             bmpWriter->Write();
//
//             break;
//         }
//         case DICOM:
//         {
//             break;
//         }
//         case META:
//         {
//             vtkMetaImageWriter *metaWriter = vtkMetaImageWriter::New();
//             metaWriter->SetInput( m_mainVolume->getVtkData() );
//             metaWriter->SetFileName( baseName );
//             metaWriter->Write();
//
//             break;
//         }
//     }
}

}
