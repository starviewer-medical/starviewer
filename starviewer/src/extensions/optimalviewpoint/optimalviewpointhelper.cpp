/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointhelper.h"

#include "optimalviewpoint.h"

namespace udg {

OptimalViewpointHelper * OptimalViewpointHelper::New() {
    return new OptimalViewpointHelper();
}

void OptimalViewpointHelper::Execute(vtkObject *caller, unsigned long eventId, void *callData) {
    if ( !m_optimalViewpoint ) std::cerr << "MagicMirrorsUpdater: Execute(): Has d'assignar un MagicMirrors!" << std::endl;
    else {
//         m_optimalViewpoint->setPlanesToRecompute();
    }
}

void OptimalViewpointHelper::setOptimalViewpoint( OptimalViewpoint * optimalViewpoint ) {
    m_optimalViewpoint = optimalViewpoint;
}

}; // end namespace udg
