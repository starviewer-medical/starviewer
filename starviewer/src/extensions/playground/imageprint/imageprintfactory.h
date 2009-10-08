#ifndef UDGIMAGEPRINTFACTORY_H
#define UDGIMAGEPRINTFACTORY_H

/**
 * Factory d'objectes que són utilitzats a la impressió d'imatges (Dicom, Paper...).
 * S'ha aplicat el patró Abstract Factory.
 */

namespace udg {

    class Printer;
    class PrintMethod;
    class PrinterManager;
    class QPrinterConfigurationWidget;
    class QPrintingConfigurationWidget;
    class QPrintJobCreatorWidget;

class ImagePrintFactory 
{	
public:
    /// Retorna la impressora.
    virtual Printer * getPrinter()=0;
    
    /// Retorna el mètode d'impressió
    virtual PrintMethod * getPrintMethod()=0;
    
     /// Retorna el manipulador de la impressora (Afegir, Eliminar...).
    virtual PrinterManager  * getPrinterManager()=0;
    
    /// Retorna el Widget encarregat de manipular la impressora (Afegir, Elimnar...)
    virtual QPrinterConfigurationWidget * getPrinterConfigurationWidget()=0;
    
    /// Retorna el Widget encarregat de configurar els paràmetres de la impressora per una impressió.
    virtual QPrintingConfigurationWidget* getPrintingConfigurationWidget()=0;
    
    /// Retorna el widget encarregat de configurar el PrintJob per una impressió.
    virtual QPrintJobCreatorWidget * getPrintJobCreatorWidget() =0;
};
} 
#endif
