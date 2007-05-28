/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef OPTIMALVIEWPOINTPLANEHELPER_H
#define OPTIMALVIEWPOINTPLANEHELPER_H

#include <vtkCommand.h>

namespace udg {

class OptimalViewpointPlane;

/**
 * Aquesta classe serveix per cridar el mètode synchronize de la classe
 * MagicMirrorsVolume.
 */
class OptimalViewpointPlaneHelper : public vtkCommand {

public:

    static OptimalViewpointPlaneHelper * New();
    virtual void Execute( vtkObject * caller, unsigned long eventId, void * callData );
    void setOptimalViewpointPlane( OptimalViewpointPlane * plane );

private:

    OptimalViewpointPlane * m_plane;

}; // end class OptimalViewpointPlaneHelper

}; // end namespace udg

#endif // OPTIMALVIEWPOINTPLANEHELPER_H
