/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerkeyimagenoteattacher.h"

#include <QFileInfo>
#include <vtkImageViewer2.h> //\TODO S'hauria d'eliminar aquesta dependència...

#include "keyimagenote.h"
#include "q2dviewer.h"
#include "q2dviewerblackboard.h"
#include "volume.h"
#include "image.h"
#include "logging.h"

namespace udg {

Q2DViewerKeyImageNoteAttacher::Q2DViewerKeyImageNoteAttacher(Q2DViewer *viewer, KeyImageNote *keyImageNote)
{
    m_viewer = viewer;
    m_keyImageNote = keyImageNote;
    m_isAttached = false;
    m_isVisibleAdditionalInformation = true;
    m_board = NULL;
}


Q2DViewerKeyImageNoteAttacher::~Q2DViewerKeyImageNoteAttacher()
{
    if (m_board)  delete m_board;
}

void Q2DViewerKeyImageNoteAttacher::attach()
{
    if (m_isAttached) return;

    m_board = new Q2DViewerBlackBoard( m_viewer );

    // Si s'ha marcat perquè té problemes de qualitat es posa vermell la marca, si no, verda.
    QColor color;
    if (m_keyImageNote->hasDocumentTitleQualityReasons() )
    {
        color = Qt::red;
    }
    else
    {
        color = Qt::green;
    }

    QStringList seriesReferenced = m_keyImageNote->getReferencedSOPInstancesUID();

    // \TODO S'hauria de mirar el num. de llesques d'una altra manera...
    int maxSlice = m_viewer->getImageViewer() ->GetSliceRange()[1];
    QString SOPClass;
    for(int slice = 0; slice <= maxSlice; ++slice )
    {
//         SOPClass = volumeInformation->getImageSOPInstanceUID(slice);
        SOPClass = m_viewer->getInput()->getImages()[slice]->getSOPInstanceUID();
        if (seriesReferenced.contains( SOPClass ))
        {
            DEBUG_LOG( QString("Llegit KIN que conté referencia a %1 en la llesca %2").arg(SOPClass).arg(slice) );
            m_board->addKeyImageNoteMark( slice, Q2DViewer::Axial, color, m_keyImageNote->getPrintableText());
        }
    }
}

void Q2DViewerKeyImageNoteAttacher::deattach()
{
    if (! m_isAttached) return;

    if (m_board) delete m_board;
}

void Q2DViewerKeyImageNoteAttacher::updateAttachment()
{
    if ( this->isAttached() )
    {
        this->deattach();
        this->attach();
    }
}

void Q2DViewerKeyImageNoteAttacher::setAttached(bool enable)
{
    if (enable) this->attach();
    else this->deattach();
}

bool Q2DViewerKeyImageNoteAttacher::isAttached() const
{
    return m_isAttached;
}

void Q2DViewerKeyImageNoteAttacher::setVisibleAdditionalInformation(bool visible)
{
    m_isVisibleAdditionalInformation = visible;
    this->updateAttachment();
}

}
