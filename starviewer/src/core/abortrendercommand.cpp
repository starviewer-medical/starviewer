#include "abortrendercommand.h"

#include <vtkRenderWindow.h>

#include "logging.h"


namespace udg {


AbortRenderCommand* AbortRenderCommand::New()
{
    return new AbortRenderCommand();
}


void AbortRenderCommand::Execute( vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData) )
{
    Q_ASSERT( eventId == AbortCheckEvent );

    vtkRenderWindow *renderWindow = vtkRenderWindow::SafeDownCast( caller );

    if ( !renderWindow )
    {
        DEBUG_LOG( "El caller no és una vtkRenderWindow!" );
        return;
    }

    if ( renderWindow->GetEventPending() ) renderWindow->SetAbortRender( 1 );
}


AbortRenderCommand::AbortRenderCommand()
 : vtkCommand()
{
}


AbortRenderCommand::~AbortRenderCommand()
{
}


}
