/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTHELPER_H
#define UDGOPTIMALVIEWPOINTHELPER_H

#include <vtkCommand.h>

/// Diu a MagicMirrors quan ha d'actualitzar els Miralls o ajustar un model registrat.
namespace udg {
class OptimalViewpoint;

class OptimalViewpointHelper : public vtkCommand
{
private:
    OptimalViewpoint * m_optimalViewpoint;
    
public:
    static OptimalViewpointHelper * New();
    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData);
    void setOptimalViewpoint( OptimalViewpoint * optimalViewpoint );
};

}; // end namespace udg

#endif // UDGOPTIMALVIEWPOINTHELPER_H
