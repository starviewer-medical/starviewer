#ifndef UDGQCINECONTROLLER_H
#define UDGQCINECONTROLLER_H

#include "ui_qcinecontrollerbase.h"

namespace udg {

class QViewerCINEController;
class QViewer;
/**
Widget per controlar el CINE en els QViewer
*/
class QCINEController : public QWidget, private Ui::QCINEControllerBase
{
Q_OBJECT
public:
    QCINEController(QWidget *parent = 0);

    ~QCINEController();

    void setQViewer(QViewer *viewer);

protected slots:
    void updateVelocityLabel(int value);
    void updateLoopStatus(bool enabled);

protected:
    QViewerCINEController *m_cineController;
};

}

#endif
