/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "optimalviewpointplanehelper.h"


#include <iostream>

#include "optimalviewpointplane.h"



namespace udg {



OptimalViewpointPlaneHelper * OptimalViewpointPlaneHelper::New()
{
    return new OptimalViewpointPlaneHelper();
}



void OptimalViewpointPlaneHelper::Execute( vtkObject * /*caller*/, unsigned long eventId, void * /*callData*/ )
{
    if ( m_plane )
    {
        if ( eventId == vtkCommand::StartEvent ) m_plane->startEntropyComputing();
        else if ( eventId == vtkCommand::EndEvent ) m_plane->endEntropyComputing();
    }
    else
        std::cerr << "OptimalViewpointPlaneHelper: Execute(): No hi ha cap OptimalViewpointPlane assignat" << std::endl;
}



void OptimalViewpointPlaneHelper::setOptimalViewpointPlane( OptimalViewpointPlane * plane )
{
    m_plane = plane;
}



}
