#ifndef UDGQDICOMPRINTERCONFIGURATIONWIDGET_H
#define UDGQDICOMPRINTERCONFIGURATIONWIDGET_H

#include "ui_qdicomprinterconfigurationwidgetbase.h"
#include "qprinterconfigurationwidget.h"


/**
 * Implementació de la interfície QPrinterConfigurationWidget per impressió d'imatge DICOM en DCMTK.
 */

namespace udg {

class Printer;

class QDicomPrinterConfigurationWidget : public QPrinterConfigurationWidget , private::Ui::QDicomPrinterConfigurationWidgetBase
{
Q_OBJECT

public:
    QDicomPrinterConfigurationWidget();
    ~QDicomPrinterConfigurationWidget();
	
    void addPrinter(Printer &_printer);
    void modifyPrinter(Printer &_printer);
    void deletePrinter(char * _refernce);

public slots:
    void choosePrinter(const int &id);

private:
    void createConnections();
    void createActions();
};
} 

#endif
