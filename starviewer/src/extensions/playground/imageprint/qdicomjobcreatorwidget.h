#ifndef UDGQDICOMJOBCREATORWIDGET_H
#define UDGQDICOMJOBCREATORWIDGET_H

#include "ui_qdicomjobcreatorwidgetbase.h"
#include "qprintjobcreatorwidget.h"

/**
 * Implementació de QPrintJobCreatorWidget per impressores DICOM en DCMTK.
 */

namespace udg {

class QDicomJobCreatorWidget : public QPrintJobCreatorWidget , private::Ui::QDicomJobCreatorWidgetBase
{
Q_OBJECT

public:
    
    QDicomJobCreatorWidget();
     ~QDicomJobCreatorWidget();
	PrintJob * getPrintJob();

private:
    void createConnections();
    void createActions();

private:
    PrintJob * m_printJob;
};
} 

#endif
