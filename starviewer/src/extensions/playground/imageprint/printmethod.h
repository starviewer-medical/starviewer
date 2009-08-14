#ifndef UDGPRINTMETHOD_H
#define UDGPRINTMETHOD_H

/**
* Interfície pel mètodes d'impressió
*/

namespace udg{

    class Printer;
    class PrintJob;

class PrintMethod
{
 public:
    virtual void setPrinter(Printer &_printer)=0;
    virtual void setPrintJob(PrintJob &_printJob)=0;
    virtual bool    sentToPrint()=0;
};
}; 
#endif