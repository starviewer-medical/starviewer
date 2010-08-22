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
#include "study.h"
#include "series.h"

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
            m_keyImageNoteToolWidget->setKeyImageNoteManager(m_keyImageNoteManager);
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


    foreach (Image *image, volume->getImages())
    {
        if (!KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(image->getParentSeries()->getParentStudy()->getParentPatient(), image).isEmpty())
        {
            double point[3];
            int orderNumberInVolume = image->getOrderNumberInVolume();

            if (volume->getImages().count() == 1)
            {
                orderNumberInVolume = 0;
            }
            point[0] = origin[0] + extent[1] * vtkSpacing[0] - 30;
            point[1] = origin[1] + extent[3] * vtkSpacing[1] - 30;
            point[2] = origin[2] + vtkSpacing[2] *orderNumberInVolume;
            
            DrawerText *keyImageNoteMark = new DrawerText;
            keyImageNoteMark->boldOn();
            keyImageNoteMark->shadowOn();
            keyImageNoteMark->setText("KIN");
            keyImageNoteMark->setAttachmentPoint(point);


            m_2DViewer->getDrawer()->draw(keyImageNoteMark, m_2DViewer->getView(), orderNumberInVolume);
            m_pointsOfKeyImageNote.append(keyImageNoteMark);
        }
    }
}

void KeyImageNoteTool::setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager)
{
    if (keyImageNoteManager != NULL)
    {
        m_keyImageNoteManager = keyImageNoteManager;
        connect(m_keyImageNoteManager, SIGNAL(imageAddedToTheCurrentSelectionOfImages(Image*)), SLOT(drawMySelectionMark(Image*)));
        connect(m_keyImageNoteManager, SIGNAL(currentSelectionCleared()), SLOT(updateMarks()));
        connect(m_keyImageNoteManager, SIGNAL(imageOfCurrentSelectionRemoved(const QString &)), SLOT(removeMySelectionMarkOfImage(const QString &)));
        
        if (m_keyImageNoteToolWidget)
        {
            m_keyImageNoteToolWidget->setKeyImageNoteManager(m_keyImageNoteManager);
        }
    }
}

void KeyImageNoteTool::drawMySelectionMark(Image *image)
{
    if (!m_pointsOfMySelection.contains(image->getSOPInstanceUID()))
    {
        Volume *volume = m_2DViewer->getInput();
        double point[3];
        double *vtkSpacing = volume->getSpacing();
        double *origin = volume->getOrigin();
        int *extent = volume->getWholeExtent();
    
        point[0] = origin[0] + extent[1] * vtkSpacing[0] - 30;
        point[1] = origin[1] + extent[3] * vtkSpacing[1] - 40;
        point[2] = origin[2] + vtkSpacing[2] * image->getOrderNumberInVolume();
    
        DrawerText *mySelectionMark = new DrawerText;
        mySelectionMark->setColor(QColor(83,195,236));
        mySelectionMark->boldOn();
        mySelectionMark->shadowOn();
        mySelectionMark->setText("NEW KIN");
        mySelectionMark->setAttachmentPoint(point);
    
        m_2DViewer->getDrawer()->draw(mySelectionMark, m_2DViewer->getView(), image->getOrderNumberInVolume());
        m_pointsOfMySelection[image->getSOPInstanceUID()] = mySelectionMark;
    }
}

void KeyImageNoteTool::updateMarks()
{
    foreach (DrawerPrimitive *primitive, m_pointsOfMySelection.values())
    {
        m_2DViewer->getDrawer()->erasePrimitive(primitive);
    }

    m_pointsOfMySelection.clear();

    foreach (DrawerPrimitive *primitive, m_pointsOfKeyImageNote)
    {
        m_2DViewer->getDrawer()->erasePrimitive(primitive);
    }

    m_pointsOfKeyImageNote.clear();
    drawKeyImageNotesInVolume();
    m_2DViewer->render();
}
void KeyImageNoteTool::removeMySelectionMarkOfImage(const QString &sopInstanceUID)
{
    if (m_pointsOfMySelection.contains(sopInstanceUID))
    {
        DrawerPrimitive *primitive = m_pointsOfMySelection.value(sopInstanceUID);
        m_2DViewer->getDrawer()->erasePrimitive(primitive);
        m_pointsOfMySelection.remove(sopInstanceUID);
        m_2DViewer->render();
    }
}
}
