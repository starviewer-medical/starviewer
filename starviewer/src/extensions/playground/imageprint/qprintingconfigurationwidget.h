#ifndef UDGQPRINTINGCONFIGURATIONWIDGET_H
#define UDGQPRINTINGCONFIGURATIONWIDGET_H

#include "QWidget"

/**
 * Interfície pels Widgets encarregats de configurar els paràmetres de la impressora per una impressió.
 */

namespace udg {

class Printer;

class QPrintingConfigurationWidget : public QWidget{
Q_OBJECT

public:
    virtual void	  setPrinter(Printer& _printer) =0;
    virtual Printer * getPrinter() =0;
};
}

#endif
