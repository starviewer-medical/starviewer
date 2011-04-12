#ifndef UDGQPRINTJOBCREATORWIDGET_H
#define UDGQPRINTJOBCREATORWIDGET_H

#include "QWidget"

/**
 * Interfície pels Widgets encarregats de configurar els paràmetres del PrintJob
 */

namespace udg {

class PrintJob;

class QPrintJobCreatorWidget : public QWidget{
Q_OBJECT

public:
	virtual PrintJob * getPrintJob() =0;
};
}

#endif
