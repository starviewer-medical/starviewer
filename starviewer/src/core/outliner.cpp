/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "outliner.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

Outliner::Outliner( Q2DViewer *viewer, QObject *parent )
 : QObject(parent)
{
    m_2DViewer = viewer;
}

Outliner::~Outliner()
{

}


}
