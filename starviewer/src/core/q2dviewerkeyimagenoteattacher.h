/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERKEYIMAGENOTEATTACHER_H
#define UDGQ2DVIEWERKEYIMAGENOTEATTACHER_H

#include <QPointer>

namespace udg {

class KeyImageNote;
class Q2DViewer;
class Q2DViewerBlackBoard;

/**
Class que ens fa un "attachement" d'un Key Image Note a un Q2DViewer. S'adjunta el KIN al Q2DViewer de manera que les llesques d'interés queden marcades. També permet escollir si es vol o no es vol veure la informació adicional que van associades a cada marca.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerKeyImageNoteAttacher{
public:

    /// Constructor que se li ha de passar el viewer al que s'adjuntarà i el key image note que adjuntarà
    Q2DViewerKeyImageNoteAttacher(Q2DViewer *viewer, KeyImageNote *keyImageNote);

    ~Q2DViewerKeyImageNoteAttacher();

    /// Adjunta el key image note al viewer
    void attach();

    /// En cas que s'hagi adjuntat, deadjunta el key image note
    void deattach();

    /// Actualitza la visualització del Key Image Note rellegint-lo.
    void updateAttachment();

    /// Versió de conviniència per adjuntar/desadjuntar el key image note segons si es passa true o false
    void setAttached(bool enable);
    bool isAttached() const;

    /// Mètode que ens permet escollir si volem o no volem visualitzar la informació adicional de KIN
    void setVisibleAdditionalInformation(bool visible);

private:
    Q2DViewer *m_viewer;
    KeyImageNote *m_keyImageNote;
    bool m_isAttached;
    bool m_isVisibleAdditionalInformation;
    QPointer<Q2DViewerBlackBoard> m_board;
};

}

#endif
