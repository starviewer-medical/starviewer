#include "loggingoutputwindow.h"

#include "logging.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>

namespace udg {

vtkStandardNewMacro(LoggingOutputWindow);

void LoggingOutputWindow::DisplayText(const char *text)
{
    INFO_LOG(text);
}

void LoggingOutputWindow::DisplayErrorText(const char *text)
{
    ERROR_LOG(text);
    this->InvokeEvent(vtkCommand::ErrorEvent, const_cast<char*>(text));
}

void LoggingOutputWindow::DisplayWarningText(const char *text)
{
    WARN_LOG(text);
    this->InvokeEvent(vtkCommand::WarningEvent, const_cast<char*>(text));
}

void LoggingOutputWindow::DisplayGenericWarningText(const char *text)
{
    WARN_LOG(text);
}

void LoggingOutputWindow::DisplayDebugText(const char *text)
{
    DEBUG_LOG(text);
}

LoggingOutputWindow::LoggingOutputWindow()
{
}

LoggingOutputWindow::~LoggingOutputWindow()
{
}

}
