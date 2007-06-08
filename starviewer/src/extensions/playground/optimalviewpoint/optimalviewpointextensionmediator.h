/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef OPTIMALVIEWPOINTEXTENSIONMEDIATOR_H
#define OPTIMALVIEWPOINTEXTENSIONMEDIATOR_H



#include "extensionmediator.h"


#include "installextension.h"
#include "qoptimalviewpointextension.h"



namespace udg {



/**
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class OptimalViewpointExtensionMediator : public ExtensionMediator {


public:

    OptimalViewpointExtensionMediator( QObject * parent = 0 );
    ~OptimalViewpointExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension( QWidget * extension, ExtensionHandler * extensionHandler, Identifier mainVolumeID );


};



static InstallExtension< QOptimalViewpointExtension, OptimalViewpointExtensionMediator > registerOptimalViewpointExtension;



}



#endif
