#ifndef UDGDICOMPRINTFACTORY_H
#define UDGDICOMPRINTFACTORY_H

#include "imageprintfactory.h"

/**
 * Implementació del Factory ImagePrint per la impressió mitjançant DCMTK.
 * S'ha aplicat el patró Abstract Factory.
 */

namespace udg 
{
class Printer;
class PrintMethod;
class PrinterManager;
class QPrinterConfigurationWidget;
class QPrintJobCreatorWidget;

class DicomPrintFactory : public ImagePrintFactory
{
public:
     
    DicomPrintFactory();
    ~DicomPrintFactory();

    /// Retorna un objecte Printer (DicomPrinter)
    Printer* getPrinter();

    /// Retorna un objecte PrintMethod (DicomPrintMethod)
    PrintMethod* getPrintMethod();

    /// Retorna un objecte PrinterManager (DicomPrinterManager)
    PrinterManager* getPrinterManager();

    /// Retorna un objecte QPrinterConfigurationWidget (QDicomPrinterConfigurationWidget)
    QPrinterConfigurationWidget* getPrinterConfigurationWidget();

    /// Retorna un QPrintJobCreatorWidget (QDicomJobCreatorWidget)
    QPrintJobCreatorWidget* getPrintJobCreatorWidget();

private:

    Printer* m_printer;

    PrintMethod* m_printMethod;

    PrinterManager* m_printerManager;

    QPrinterConfigurationWidget* m_qPrinterconfigurationWidget;

    QPrintJobCreatorWidget* m_qPrintJobCreatorWidget;
};
}

#endif
