/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLSACTIONFACTORY_H
#define UDGTOOLSACTIONFACTORY_H

#include <QObject>
#include <QString>
#include <map>

class QAction;
class QSignalMapper;

namespace udg {

/**
Crea les QAction per cada tipu de tool. En aquesta classe unifiquem la creació de les QAction de cada tool

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolsActionFactory : public QObject {
Q_OBJECT
public:
    ToolsActionFactory( QWidget *parent = 0 );

    ~ToolsActionFactory();

    QAction *getActionFrom( QString toolName );

signals:
    /// Envia el nom de la tool activada
    void triggeredTool( QString toolName );

private:
    ///\TODO ara fem que només hi hagi una sola qaction per cada tool. No sabem si en in futur, per segons quina extensió sigui necessari tenir-ne 2 o més
    QAction *m_slicingAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_windowLevelAction;
    QAction *m_screenShotAction;
    QAction *m_rotate3dAction;
    QAction *m_distanceAction;
    QAction *m_roiAction;
    QAction *m_seedAction;
    QAction *m_voxelInformationAction;

    QSignalMapper *m_signalMapper;
    typedef std::map<QString,QAction*> ToolMapType;
    ToolMapType m_availableToolActions;
};

}

#endif
