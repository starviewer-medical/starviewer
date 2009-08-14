#ifndef UDGPRINTERHANDLER_H
#define UDGPRINTERHANDLER_H

#include <QList>

/**
* Interfície pels manipuladors d'impressores (Afegir, Esborrar...).
*/

namespace udg{

    class Printer;

class PrinterHandler
{
 public:
	virtual void addPrinter(Printer &_printer)=0;
    virtual void updatePrinter(Printer &_printer)=0;
    virtual void removePrinter(Printer &_printer)=0;
    virtual void removePrinter(QString &_reference)=0;
    virtual Printer * getPrinter(QString &_reference)=0;
};
}; 
#endif