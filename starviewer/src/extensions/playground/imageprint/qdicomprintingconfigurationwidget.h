#ifndef UDGQDICOMPRINTINGCONFIGURATIONWIDGET_H
#define UDGQDICOMPRINTINGCONFIGURATIONWIDGET_H

#include "ui_qdicomprintingconfigurationwidgetbase.h"
#include "qprintingconfigurationwidget.h"

/**
 * Implementació de QPrintingConfigurationWidget per impressores DICOM en DCMTK.
 */

namespace udg {

class Printer;

class QDicomPrintingConfigurationWidget : public QPrintingConfigurationWidget , private::Ui::QDicomPrintingConfigurationWidgetBase
{
Q_OBJECT

public:
    QDicomPrintingConfigurationWidget();
    ~QDicomPrintingConfigurationWidget();
	
    void	  setPrinter(Printer& _printer);
    Printer *  getPrinter();

private:
    void createConnections();
    void createActions();
};
} 

#endif
