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
	
public slots:
    void choosePrinter();
    void addPrinter();
    void updatePrinter();
    void deletePrinter();
    void testPrinter();
    void getAvailableParameters();

private:
    void createConnections();
    void createActions();
    void fillPrintersList();
    bool validatePrinterParameters();
    void clearPrinterParameters();
    
    int selectedPrinterId;
};
} 

#endif
