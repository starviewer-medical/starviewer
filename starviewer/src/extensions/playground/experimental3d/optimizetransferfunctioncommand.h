#ifndef OPTIMIZETRANSFERFUNCTIONCOMMAND_H
#define OPTIMIZETRANSFERFUNCTIONCOMMAND_H

#include <vtkCommand.h>

namespace udg {

class QExperimental3DExtension;

class OptimizeTransferFunctionCommand : public vtkCommand {

public:

    static OptimizeTransferFunctionCommand* New();

    void setExtension(QExperimental3DExtension *extension);
    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData);

protected:

    OptimizeTransferFunctionCommand();

protected:

    QExperimental3DExtension *m_extension;

};

} // namespace udg

#endif // OPTIMIZETRANSFERFUNCTIONCOMMAND_H
