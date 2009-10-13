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

public slots:
    virtual void addPrinter() =0;
    virtual void modifyPrinter() =0;
    virtual void deletePrinter() =0; 
};
}

#endif
