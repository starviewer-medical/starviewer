#ifndef UDGQPRINTERCONFIGURATIONWIDGET_H
#define UDGQPRINTERCONFIGURATIONWIDGET_H

#include "QWidget"

/**
 * Interfície pels Widgets encarregats de manipular la impressora (Afegir, Elimnar...).
 */

namespace udg {

class Printer;

class QPrinterConfigurationWidget : public QWidget{
Q_OBJECT

public:
    virtual void addPrinter(Printer &_printer) =0;
    virtual void modifyPrinter(Printer &_printer) =0;
    virtual void deletePrinter(char * _reference) =0; 
};
}

#endif
