/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "keyimagenotetool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "image.h"
#include "keyimagenotemanager.h"
#include "drawer.h"
#include "drawertext.h"
#include "keyimagenotetoolwidget.h"
#include "drawerprimitive.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

KeyImageNoteTool::KeyImageNoteTool(QViewer *viewer, QObject *parent): Tool(viewer, parent), m_keyImageNoteToolWidget(0)
{
    m_toolName = "KeyImageNoteTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume *)), SLOT(initialize()));
}

KeyImageNoteTool::~KeyImageNoteTool()
{

}

void KeyImageNoteTool::initialize()
{
    DEBUG_LOG("Volume changed");
    drawKeyImageNotesInVolume();
}

void KeyImageNoteTool::handleEvent(long unsigned eventID)
{
    switch(eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (m_2DViewer->getInteractor()->GetRepeatCount() > 0)
            {
                showKeyImageNoteDescriptor();
            }
            break;
        default:
            break;
    }
}

void KeyImageNoteTool::showKeyImageNoteDescriptor()
{
    Image *selectedImage = m_2DViewer->getCurrentDisplayedImage();
    if (qobject_cast<Image*>(selectedImage) != NULL)
    {
        if (hasKeyImageNoteToolWidgetBeDisplayed())
        {
            if (!m_keyImageNoteToolWidget)
            {
                m_keyImageNoteToolWidget = new KeyImageNoteToolWidget();
            }
            // Ara es fa dos cops, aquest s'ha de fer, el del hasKeyImageNoteToolWidget no!
            m_keyImageNoteToolWidget->setKeyImageNotes(KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(m_2DViewer->getInput()->getPatient(), selectedImage));
            m_keyImageNoteToolWidget->exec();
        }
    }
}

bool KeyImageNoteTool::hasKeyImageNoteToolWidgetBeDisplayed()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // La coordenada z no es correcta, l'hem de recalcular nosaltres
    Volume *volume = m_2DViewer->getInput();
    double *vtkSpacing = volume->getSpacing();
    double *origin = volume->getOrigin();
    clickedWorldPoint[2] = origin[2] + vtkSpacing[2] * m_2DViewer->getCurrentSlice();
    DEBUG_LOG(qPrintable(QString("llesca: %4, clickedWorldPoint: (%1, %2, %3)").arg(clickedWorldPoint[0]).arg(clickedWorldPoint[1]).arg(clickedWorldPoint[2]).arg(m_2DViewer->getCurrentSlice()+1)));

    DrawerPrimitive *drawerPrimitive = m_2DViewer->getDrawer()->getPrimitiveNearerToPoint(clickedWorldPoint, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    if (!KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(volume->getPatient(), m_2DViewer->getCurrentDisplayedImage()).isEmpty())
    {
        return m_pointsOfKeyImageNote.contains(drawerPrimitive);
    }

    return false;
}

void KeyImageNoteTool::drawKeyImageNotesInVolume()
{
    Volume *volume = m_2DViewer->getInput();
    double *vtkSpacing = volume->getSpacing();
    double *origin = volume->getOrigin();
    int *extent = volume->getWholeExtent();

    DEBUG_LOG(qPrintable(QString("Origin: (%1, %2, %3) ").arg(origin[0]).arg(origin[1]).arg(origin[2])));
    DEBUG_LOG(qPrintable(QString("vtkSpacing: (%1, %2, %3)").arg(vtkSpacing[0]).arg(vtkSpacing[1]).arg(vtkSpacing[2])));

    foreach (Image *image, volume->getImages())
    {
        if (!KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(volume->getPatient(), image).isEmpty())
        {
            double point[3];
            point[0] = origin[0] + extent[1] * vtkSpacing[0] - 30;
            point[1] = origin[1] + extent[3] * vtkSpacing[1] - 30;
            point[2] = origin[2] + vtkSpacing[2] * image->getOrderNumberInVolume();
            
            DrawerText *keyImageNoteMark = new DrawerText;
            keyImageNoteMark->boldOn();
            keyImageNoteMark->shadowOn();
            keyImageNoteMark->setText("KIN");
            keyImageNoteMark->setAttachmentPoint(point);

            DEBUG_LOG(qPrintable(QString("OrderNumberInVolume: %2, llesca: %6; punt (%3,%4,%5)").arg(image->getOrderNumberInVolume()).arg(point[0]).arg(point[1]).arg(point[2]).arg(image->getOrderNumberInVolume()+1)));

            m_2DViewer->getDrawer()->draw(keyImageNoteMark, m_2DViewer->getView(), image->getOrderNumberInVolume());
            m_pointsOfKeyImageNote.append(keyImageNoteMark);
        }
    }
}
}
