#ifndef UDGABORTRENDERCOMMAND_H
#define UDGABORTRENDERCOMMAND_H


#include <vtkCommand.h>


namespace udg {

/**
 * Aborta el render quan cal.
 */
class AbortRenderCommand : public vtkCommand {

public:

    static AbortRenderCommand* New();
    virtual void Execute( vtkObject *caller, unsigned long eventId, void *callData );

protected:

    AbortRenderCommand();
    ~AbortRenderCommand();

};


}


#endif
