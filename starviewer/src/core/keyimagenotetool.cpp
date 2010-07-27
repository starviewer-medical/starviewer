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
#include "drawerpoint.h"
#include "keyimagenotetoolwidget.h"
#include "drawerprimitive.h"

#include <vtkCommand.h>

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
            showKeyImageNoteDescriptor();
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
    bool containsPrimitive = false;
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    DEBUG_LOG(qPrintable(QString("%1").arg(clickedWorldPoint[0])));
    DEBUG_LOG(qPrintable(QString("%1").arg(clickedWorldPoint[1])));
    DEBUG_LOG(qPrintable(QString("%1").arg(clickedWorldPoint[2])));
    
    DrawerPrimitive *drawerPrimitive = m_2DViewer->getDrawer()->getPrimitiveNearerToPoint(clickedWorldPoint, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    Volume *volume = m_2DViewer->getInput(); // Aixo ha de ser si trobem primitiva, pero no estem ben situats

    if (!(KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(volume->getPatient(), m_2DViewer->getCurrentDisplayedImage()).isEmpty()))
    {
        //containsPrimitive = m_pointsOfKeyImageNote.contains(drawerPrimitive);
        containsPrimitive = true;
    }

    return containsPrimitive;
}

void KeyImageNoteTool::drawKeyImageNotesInVolume()
{
    Volume *volume = m_2DViewer->getInput();

    foreach (Image *image, volume->getImages())
    {
        if (!(KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(volume->getPatient(), image).isEmpty()))
        {
            double *vtkSpacing = m_2DViewer->getInput()->getSpacing();
            double origin[3];
            int extent[6];
            
            volume->getOrigin(origin);
            volume->getWholeExtent(extent);
            
            double point[3];
            point[0] = origin[0] + extent[1] * vtkSpacing[0] - 30;
            point[1] = origin[1] + extent[3] * vtkSpacing[1] - 30;
            point[2] = origin[2] + 5 + vtkSpacing[2] * image->getOrderNumberInVolume();
            
            DrawerPoint *drawerPoint = new DrawerPoint;
            drawerPoint->setRadius(10);
            drawerPoint->setPosition(point);

            DEBUG_LOG(qPrintable(QString("%1").arg(point[0])));
            DEBUG_LOG(qPrintable(QString("%1").arg(point[1])));
            DEBUG_LOG(qPrintable(QString("%1").arg(point[2])));

            m_2DViewer->getDrawer()->draw(drawerPoint, m_2DViewer->getView(), image->getOrderNumberInVolume());
            m_pointsOfKeyImageNote.append(drawerPoint);
        }
    }
}
}
