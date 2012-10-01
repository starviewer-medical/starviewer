#ifndef UDG_LOGGINGOUTPUTWINDOW_H
#define UDG_LOGGINGOUTPUTWINDOW_H

#include <vtkOutputWindow.h>

namespace udg {

/**
 * Subclasse de vtkOutputWindow que redirigeix els missatges d'error de VTK cap al log d'Starviewer.
 *
 * Encara que al nom hi digui "Window", no mostra cap finestra (i vtkOutputWindow per si sola tampoc).
 */
class LoggingOutputWindow : public vtkOutputWindow {

public:

    static LoggingOutputWindow* New();

    virtual void DisplayText(const char*);
    virtual void DisplayErrorText(const char*);
    virtual void DisplayWarningText(const char*);
    virtual void DisplayGenericWarningText(const char*);
    virtual void DisplayDebugText(const char*);

protected:
    
    LoggingOutputWindow();
    virtual ~LoggingOutputWindow();

private:

    LoggingOutputWindow(const LoggingOutputWindow&);    // No s'implementa.
    void operator=(const LoggingOutputWindow&);         // No s'implementa.

};

}

#endif
