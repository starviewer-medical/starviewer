#include "presentationstateattacher.h"
#include "q2dviewer.h"
#include "image.h"
#include "logging.h"
#include "volume.h"
#include "q2dviewerblackboard.h"

// vtk
#include <vtkPoints.h>

// dcmtk
#include <dcmtk/dcmpstat/dvpsgr.h>
#include <dvpstat.h>
#include <dcmtk/dcmpstat/dvpstx.h>

namespace udg {

PresentationStateAttacher::PresentationStateAttacher() : m_2DViewer(0)
{
    m_presentationStateHandler = new DVPresentationState;
}

PresentationStateAttacher::~PresentationStateAttacher()
{
}

void PresentationStateAttacher::setFile(const QString &fileName)
{
    DEBUG_LOG(QString("Canvi de fitxer a %1").arg(fileName));

    DcmFileFormat presentationStateFile;
    bool isPresentationStateReaded = false;

    if (presentationStateFile.loadFile(qPrintable(fileName)).good())
    {
        if (presentationStateFile.getDataset())
        {
            if ((m_presentationStateHandler->read(*presentationStateFile.getDataset())).good())
            {
                isPresentationStateReaded = true;
            }
        }
    }

    if (isPresentationStateReaded)
    {
        applyPresentationState();
    }
}

void PresentationStateAttacher::setQ2DViewer(Q2DViewer *viewer)
{
    m_2DViewer = viewer;
    m_board = new Q2DViewerBlackBoard(m_2DViewer);
}

void PresentationStateAttacher::applyPresentationState()
{
    isSpatialTransformationApplied = false;
    // Netejem transformacions i dibuixos anteriors
    m_2DViewer->restore();
    m_board->clear();

    foreach (Image *image, m_2DViewer->getInput()->getImages())
    {
        if (isImageReferencedInPresentationState(image))
        {
            applyGraphicAnnotations(image);
            applySpatialTransformation();
            applyDisplayedAreaTransformation(image);
            applyPostSpatialTransformation(image);
        }
    }
}

bool PresentationStateAttacher::isImageReferencedInPresentationState(Image *image)
{
    bool found = false;
    int i = 0;
    int references = m_presentationStateHandler->numberOfImageReferences();
    while (!found && i < references)
    {
        OFString studyUID, seriesUID, sopClassUID, instanceUID, frames, aetitle, filesetID, filesetUID;
        m_presentationStateHandler->getImageReference(i, studyUID, seriesUID, sopClassUID, instanceUID, frames, aetitle, filesetID, filesetUID);

        if (QString(instanceUID.c_str()) == image->getSOPInstanceUID())
        {
            found = true;
        }

        i++;
    }

    return found;
}

void PresentationStateAttacher::applySpatialTransformation()
{
    if (!isSpatialTransformationApplied)
    {
        int rotation;
        switch (m_presentationStateHandler->getRotation())
        {
            case DVPSR_0_deg:
                DEBUG_LOG("No hi ha Rotació");
                rotation = 0;
                break;

            case DVPSR_90_deg:
                DEBUG_LOG("Rotació de 90º");
                rotation = 1;
                break;

            case DVPSR_180_deg:
                DEBUG_LOG("Rotació de 180º");
                rotation = 2;
                break;

            case DVPSR_270_deg:
                DEBUG_LOG("Rotació de 270º");
                rotation = 3;
            break;
        }

        m_2DViewer->rotateClockWise(rotation);
        
        if (m_presentationStateHandler->getFlip())
        {
            DEBUG_LOG("Hi ha flip horitzontal");
            m_2DViewer->horizontalFlip();
        }
        else
        {
            DEBUG_LOG("NO Hi ha flip horitzontal");
        }

        isSpatialTransformationApplied = true;
    }
}

void PresentationStateAttacher::applyDisplayedAreaTransformation(Image *image)
{
    Sint32 topLeftX = 0;
    Sint32 topLeftY = 0;
    Sint32 bottomRightX = 0;
    Sint32 bottomRightY = 0;
    
    DcmFileFormat *presentationStateFile = new DcmFileFormat;
    presentationStateFile->loadFile(qPrintable(image->getPath()));
    m_presentationStateHandler->attachImage(presentationStateFile, false);

    m_presentationStateHandler->getStandardDisplayedArea(topLeftX, topLeftY, bottomRightX, bottomRightY);
    
    m_2DViewer->getInput()->updateInformation();
    double spacing[3];
    double origin[3];
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);

    m_displayedAreaWorldTopLeft[0] = origin[0] + topLeftX * spacing[0];
    m_displayedAreaWorldTopLeft[1] = origin[1] + topLeftY * spacing[1];
    m_displayedAreaWorldBottomRight[0] = origin[0] + bottomRightX * spacing[0];
    m_displayedAreaWorldBottomRight[1] = origin[1] + bottomRightY * spacing[1];

    double presentationPixelSpacing[2];
    if(EC_Normal == m_presentationStateHandler->getDisplayedAreaPresentationPixelSpacing(presentationPixelSpacing[0], presentationPixelSpacing[1]))
    {
        DEBUG_LOG(qPrintable(QString("Presentation pixel spacing: X=%1mm Y=%2mm").arg(presentationPixelSpacing[0]).arg(presentationPixelSpacing[1])));
    }
    else
    {
        double ratio = m_presentationStateHandler->getDisplayedAreaPresentationPixelAspectRatio();
        DEBUG_LOG(qPrintable(QString("Ratio obtingut %1").arg(ratio)));
        m_2DViewer->setPixelAspectRatio(ratio);
    }

    DVPSPresentationSizeMode sizemode = m_presentationStateHandler->getDisplayedAreaPresentationSizeMode();
    double factor = 1.0;

    switch (sizemode)
    {
        case DVPSD_scaleToFit:
            DEBUG_LOG("Presentation size mode: SCALE TO FIT");
            m_2DViewer->scaleToFit(m_displayedAreaWorldTopLeft[0], m_displayedAreaWorldTopLeft[1], m_displayedAreaWorldBottomRight[0], m_displayedAreaWorldBottomRight[1]);
            break;
        
        case DVPSD_trueSize:
            DEBUG_LOG("Presentation size mode: TRUE SIZE");
            break;

        case DVPSD_magnify:
            m_presentationStateHandler->getDisplayedAreaPresentationPixelMagnificationRatio(factor);
            DEBUG_LOG(qPrintable(QString("Presentation size mode: MAGNIFY factor=%1").arg(factor)));
            m_2DViewer->setMagnificationFactor(factor);
            break;
        
        default:
            DEBUG_LOG(qPrintable(QString("Size mode no retorna cap valor dels 3 esperats: valor retornat: %1").arg(sizemode)));
            break;
    }
}

void PresentationStateAttacher::applyGraphicAnnotations(Image *image)
{
    size_t max;

    double origin[3],spacing[3];
    m_2DViewer->getInput()->updateInformation();
    m_2DViewer->getInput()->getOrigin(origin);
    m_2DViewer->getInput()->getSpacing(spacing);

    DcmFileFormat *presentationStateFile = new DcmFileFormat;
    presentationStateFile->loadFile(qPrintable(image->getPath()));
    m_presentationStateHandler->attachImage(presentationStateFile, false);
    
    m_presentationStateHandler->sortGraphicLayers();
    QColor color;
    
    for (size_t layer = 0; layer < m_presentationStateHandler->getNumberOfGraphicLayers(); layer++)
    {
        color = this->getRecommendedColor(layer);
        DVPSGraphicObject *pgraphic = NULL;
        max = m_presentationStateHandler->getNumberOfGraphicObjects(layer);
        for (size_t graphicidx=0; graphicidx < max; graphicidx++)
        {
            pgraphic = m_presentationStateHandler->getGraphicObject(layer, graphicidx);
            
            if (pgraphic && pgraphic->getAnnotationUnits() == DVPSA_pixels)
            {
                int j = pgraphic->getNumberOfPoints();
                Float32 fx = 0.0, fy = 0.0;
                
                vtkPoints *points = vtkPoints::New();
                
                double point[2];
                for (int k = 0; k < j; k++)
                {
                    if (EC_Normal == pgraphic->getPoint(k,fx,fy))
                    {
                        point[0] = origin[0] + (fx - 1) * spacing[0];
                        point[1] = origin[1] + (fy - 1) * spacing[1];
                        points->InsertPoint( k , point[0], point[1], 0.0);
                    }
                }

                    double radius;
                    double center[2];
                    double xAxis1[2];
                    double xAxis2[2];
                    double yAxis1[2];
                    double yAxis2[2];
                    
                    switch (pgraphic->getGraphicType())
                    {
                        case DVPST_polyline:
                            m_board->addPolyline(points, image->getOrderNumberInVolume(),  Q2DViewer::Axial, false, pgraphic->isFilled(), color);
                            break;

                        case DVPST_interpolated:
                            m_board->addPolyline(points, image->getOrderNumberInVolume(), Q2DViewer::Axial, true, pgraphic->isFilled(), color );
                            break;
                        
                        case DVPST_circle:
                            radius = sqrt(pow((points->GetPoint(0)[0]-points->GetPoint(1)[0]) ,2) + pow( (points->GetPoint(0)[1]-points->GetPoint(1)[1]) ,2)  );
                            center[0] = points->GetPoint(0)[0];
                            center[1] = points->GetPoint(0)[1];
                            m_board->addCircle(center, radius, image->getOrderNumberInVolume(), Q2DViewer::Axial, pgraphic->isFilled(), color );
                            break;
                        
                        case DVPST_ellipse:
                            xAxis1[0] = points->GetPoint(0)[0];
                            xAxis1[1] = points->GetPoint(0)[1];

                            xAxis2[0] = points->GetPoint(1)[0];
                            xAxis2[1] = points->GetPoint(1)[1];
                            
                            yAxis1[0] = points->GetPoint(2)[0];
                            yAxis1[1] = points->GetPoint(2)[1];
                            
                            yAxis2[0] = points->GetPoint(3)[0];
                            yAxis2[1] = points->GetPoint(3)[1];
                            m_board->addEllipse( xAxis1, xAxis2, yAxis1, yAxis2, image->getOrderNumberInVolume(), Q2DViewer::Axial, pgraphic->isFilled(), color );
                            break;
                        
                        case DVPST_point:
                            center[0] = points->GetPoint(0)[0];
                            center[1] = points->GetPoint(0)[1];
                            m_board->addPoint( center, image->getOrderNumberInVolume(), Q2DViewer::Axial, color );
                            break;
                    }
            }
        }
    }

    m_2DViewer->render();
}

QColor PresentationStateAttacher::getRecommendedColor(int layer)
{
    QColor color = Qt::white;
    if(m_presentationStateHandler->haveGraphicLayerRecommendedDisplayValue( layer ) )
    {
        Uint16 r, g, b;
        if(EC_Normal == m_presentationStateHandler->getGraphicLayerRecommendedDisplayValueGray(layer, g))
        {
            DEBUG_LOG(qPrintable(QString("Valor de gris recomenat pel display: %1 al layer %2").arg(g).arg(layer)));
            // normalitzem el valor
            color.setRedF(g/65535.0);
            color.setBlueF(g/65535.0);
            color.setGreenF(g/65535.0);
        }
        else
        {
            DEBUG_LOG(qPrintable(QString("No hi ha valor de gris recomenat pel display al layer %1").arg(layer)));
        }

        if(EC_Normal == m_presentationStateHandler->getGraphicLayerRecommendedDisplayValueRGB(layer, r, g, b))
        {
            DEBUG_LOG(qPrintable( QString("Valor de color RGB recomenat pel display: %1,%2,%3").arg(r).arg(g).arg(b) ) );
            color.setRedF(r/65535.0);
            color.setBlueF(g/65535.0);
            color.setGreenF(b/65535.0);
        }
        else
        {
            DEBUG_LOG(qPrintable(QString("No hi ha valor de color rgb recomenat pel display al layer %1").arg(layer)));
        }
    }
    else
    {
        DEBUG_LOG(qPrintable(QString("No hi ha valor recomenat pel display al layer %1").arg(layer)));
    }
    
    return color;
}

void PresentationStateAttacher::applyPostSpatialTransformation(Image *image)
{
    const char *c;
    size_t max;

    int rotation = m_presentationStateHandler->getRotation();
    int xIndex = (rotation == DVPSR_0_deg || rotation == DVPSR_180_deg) ? 0 : 1;
    int yIndex = (rotation == DVPSR_0_deg || rotation == DVPSR_180_deg) ? 1 : 0;

    double displayedWidth = m_displayedAreaWorldBottomRight[xIndex] - m_displayedAreaWorldTopLeft[xIndex];
    double displayedHeight = m_displayedAreaWorldBottomRight[yIndex] - m_displayedAreaWorldTopLeft[yIndex];

    double topLeft[2] = {m_displayedAreaWorldTopLeft[xIndex], m_displayedAreaWorldTopLeft[yIndex]};

    DcmFileFormat *presentationStateFile = new DcmFileFormat;
    presentationStateFile->loadFile(qPrintable(image->getPath()));
    m_presentationStateHandler->attachImage(presentationStateFile, false);
    
    m_presentationStateHandler->sortGraphicLayers();  // to order of display
    
    QColor color;
    
    for(size_t layer=0; layer < m_presentationStateHandler->getNumberOfGraphicLayers(); layer++)
    {
        c = m_presentationStateHandler->getGraphicLayerName(layer);
        c = m_presentationStateHandler->getGraphicLayerDescription(layer);
        
        color = this->getRecommendedColor(layer);
        processTextObjects( layer, image->getOrderNumberInVolume());
        
        max = m_presentationStateHandler->getNumberOfGraphicObjects(layer);
        DVPSGraphicObject *pgraphic = NULL;
        
        for(size_t graphicidx = 0; graphicidx < max; graphicidx++)
        {
            pgraphic = m_presentationStateHandler->getGraphicObject(layer, graphicidx);
            
            if(pgraphic && pgraphic->getAnnotationUnits() == DVPSA_display)
            {
                int j = pgraphic->getNumberOfPoints();
                Float32 fx = 0.0, fy = 0.0;
                
                vtkPoints *points = vtkPoints::New();
                
                double point[2];
                
                for (int k = 0; k < j; k++)
                {
                    if (EC_Normal == pgraphic->getPoint(k,fx,fy))
                    {
                        point[xIndex] = topLeft[0] + displayedWidth * fx;
                        point[yIndex] = topLeft[1] + displayedHeight * fy;
                        points->InsertPoint(k, point[0], point[1],0.0);
                    }
                }
                double radius;
                double center[2];
                double xAxis1[2];
                double xAxis2[2];
                double yAxis1[2];
                double yAxis2[2];
                
                switch(pgraphic->getGraphicType())
                {
                case DVPST_polyline:
                    m_board->addPolyline(points, image->getOrderNumberInVolume(), Q2DViewer::Axial, false, pgraphic->isFilled(), color);
                    break;
                
                case DVPST_interpolated:
                    m_board->addPolyline(points, image->getOrderNumberInVolume(), Q2DViewer::Axial, true, pgraphic->isFilled(), color);
                    break;
                
                case DVPST_circle:
                    radius = sqrt( pow( (points->GetPoint(0)[0]-points->GetPoint(1)[0]) ,2) + pow( (points->GetPoint(0)[1]-points->GetPoint(1)[1]) ,2)  );
                    center[0] = points->GetPoint(0)[0];
                    center[1] = points->GetPoint(0)[1];
                    m_board->addCircle(center, radius, image->getOrderNumberInVolume(), Q2DViewer::Axial, pgraphic->isFilled(), color);
                    break;
                
                case DVPST_ellipse:
                    xAxis1[0] = points->GetPoint(0)[0];
                    xAxis1[1] = points->GetPoint(0)[1];
                    
                    xAxis2[0] = points->GetPoint(1)[0];
                    xAxis2[1] = points->GetPoint(1)[1];
                    
                    yAxis1[0] = points->GetPoint(2)[0];
                    yAxis1[1] = points->GetPoint(2)[1];
                    
                    yAxis2[0] = points->GetPoint(3)[0];
                    yAxis2[1] = points->GetPoint(3)[1];
                    m_board->addEllipse(xAxis1, xAxis2, yAxis1, yAxis2, image->getOrderNumberInVolume(), Q2DViewer::Axial, pgraphic->isFilled(), color);
                    break;
                
                case DVPST_point:
                    center[0] = points->GetPoint(0)[0];
                    center[1] = points->GetPoint(0)[1];
                    m_board->addPoint(center, image->getOrderNumberInVolume(), Q2DViewer::Axial, color);
                    break;
                }
            }
        }
    }
    
    m_2DViewer->render();
}

void PresentationStateAttacher::processTextObjects(int layer, int slice)
{
    double origin[3],spacing[3];
    m_2DViewer->getInput()->updateInformation();
    m_2DViewer->getInput()->getOrigin(origin);
    m_2DViewer->getInput()->getSpacing(spacing);

    unsigned int max;
    max = m_presentationStateHandler->getNumberOfTextObjects(layer);
    DEBUG_LOG(qPrintable( QString("Nombre d'objectes de text: %1").arg(max)));
    DVPSTextObject *ptext = NULL;

    // per les coordenades relatives a display
    int rotation = m_presentationStateHandler->getRotation();
    int xIndex = (rotation == DVPSR_0_deg || rotation == DVPSR_180_deg) ? 0 : 1;
    int yIndex = (rotation == DVPSR_0_deg || rotation == DVPSR_180_deg) ? 1 : 0;

    double displayedWidth = m_displayedAreaWorldBottomRight[xIndex] - m_displayedAreaWorldTopLeft[xIndex];
    double displayedHeight = m_displayedAreaWorldBottomRight[yIndex] - m_displayedAreaWorldTopLeft[yIndex];

    double topLeft[2] = {m_displayedAreaWorldTopLeft[xIndex], m_displayedAreaWorldTopLeft[yIndex]};
    for (size_t textidx=0; textidx < max; textidx++)
    {
        ptext = m_presentationStateHandler->getTextObject(layer, textidx);
        
        if (ptext)
        {
            QString msg = QString("Text #%1: ['%2']").arg(textidx+1).arg( ptext->getText());
            int orientation = Q2DViewerBlackBoard::NormalTextOrientation;
            double attachPoint[2] = {0.0, 0.0};
            bool hasAnchor = false;
            
            if(ptext->haveAnchorPoint())
            {
                hasAnchor = true;
                if(ptext->getAnchorPointAnnotationUnits() == DVPSA_pixels)
                {
                    attachPoint[0] = origin[0] + (ptext->getAnchorPoint_x()-1) * spacing[0];
                    attachPoint[1] = origin[1] + (ptext->getAnchorPoint_y()-1) * spacing[1];
                }
                else
                {
                    attachPoint[xIndex] = topLeft[0] + displayedWidth * ptext->getAnchorPoint_x();
                    attachPoint[yIndex] = topLeft[ 1] + displayedHeight * ptext->getAnchorPoint_y();
                }

            }

            double position[2] = {0.0, 0.0}, position2[2] = {0.0, 0.0};
            if (ptext->haveBoundingBox())
            {
                double referencePoint[2], referencePoint2[2];
                if (ptext->getBoundingBoxBRHC_x() >= ptext->getBoundingBoxTLHC_x())
                {
                    if (ptext->getBoundingBoxBRHC_y() >= ptext->getBoundingBoxTLHC_y()) // sentit d'esquerra a dreta
                    {
                        referencePoint[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint[1] = ptext->getBoundingBoxBRHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxTLHC_y();
                        orientation = Q2DViewerBlackBoard::NormalTextOrientation;
                    }
                    else // sentit d'abaix cap amunt
                    {
                        referencePoint[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint[1] = ptext->getBoundingBoxTLHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxBRHC_y();
                        orientation = Q2DViewerBlackBoard::LeftRotatedTextOrientation;
                    }
                }
                else // sentit de dreta esquerra cap per avall
                {
                    if (ptext->getBoundingBoxBRHC_y() < ptext->getBoundingBoxTLHC_y()) // && position2[0] < position1[0]
                    {
                        referencePoint[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint[1] = ptext->getBoundingBoxTLHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxBRHC_y();
                        orientation = Q2DViewerBlackBoard::UpsideDownTextOrientation;

                    }
                    else // sentit de dalt a baix
                    {
                        referencePoint[0] = ptext->getBoundingBoxBRHC_x();
                        referencePoint[1] = ptext->getBoundingBoxBRHC_y();
                        referencePoint2[0] = ptext->getBoundingBoxTLHC_x();
                        referencePoint2[1] = ptext->getBoundingBoxTLHC_y();
                        orientation = Q2DViewerBlackBoard::RightRotatedTextOrientation;
                    }
                }
                // Atenció: les posicions en l'actor no són TLHC i BRHC, Position = lower-left Position2 = upper-right, per això posem les coordenades de la següent manera
                //\TODO cal comprovar que els reference point són correctes!
                if (ptext->getBoundingBoxAnnotationUnits() == DVPSA_pixels)
                {
                    position[0] = origin[0] + (referencePoint[0] - 1) * spacing[0];
                    position[1] = origin[1] + (referencePoint[1] - 1) * spacing[1];

                    position2[0] = origin[0] + (referencePoint2[0] - 1) * spacing[0];
                    position2[1] = origin[1] + (referencePoint2[1] - 1) * spacing[1];
                }
                else
                {
                    position[xIndex] = topLeft[0] + displayedWidth * referencePoint[0];
                    position[yIndex] = topLeft[1] + displayedHeight * referencePoint[1];

                    position2[xIndex] = topLeft[0] + displayedWidth * referencePoint2[0];
                    position2[yIndex] = topLeft[1] + displayedHeight * referencePoint2[1];
                }
            }
            else
            {
                DEBUG_LOG("L'anotació de texte no té Bounding Box");
            }

            DVPSTextJustification justification = ptext->getBoundingBoxHorizontalJustification();
            // l'enumeració de les dcmtk respecte a les de vtk en quant a la justificació són diferents, cal controlar això. En Dcmtk left = 0, right = 1, center = 2, a vtk VTK_TEXT_LEFT = 0, VTK_TEXT_CENTERED = 1, VTK_TEXT_RIGHT = 2
            int textJustification;
            switch (justification)
            {
                case DVPSX_left:
                    DEBUG_LOG( "Format: Justificat esquerra" );
                    textJustification = 0;
                    break;

                case DVPSX_right:
                    DEBUG_LOG( "Format: Justificat dreta" );
                    textJustification = 2;
                    break;

                case DVPSX_center:
                    DEBUG_LOG( "Format: Justificat centrat" );
                    textJustification = 1;
                    break;
            }
            
            m_board->addTextAnnotation(ptext->getText(), slice, Q2DViewer::Axial, ptext->haveBoundingBox(), ptext->haveAnchorPoint(), attachPoint, position, position2, orientation, textJustification, this->getRecommendedColor( layer), ptext->haveBoundingBox(), ptext->anchorPointIsVisible());
        }
    }
}
}
