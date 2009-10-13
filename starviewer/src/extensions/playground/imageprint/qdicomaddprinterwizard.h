#ifndef UDGQDICOMADDPRINTERWIZARD_H
#define UDGQDICOMADDPRINTERWIZARD_H

#include "ui_qdicomaddprinterwizardbase.h"

namespace udg {

class DicomPrinter;

class QDicomAddPrinterWizard : public QWizard, private::Ui::QDicomAddPrinterWizardBase
{
Q_OBJECT

public:
    QDicomAddPrinterWizard();
    ~QDicomAddPrinterWizard();
	
public slots:

     void addPrinter(int codiPagina);

private:

    bool validateInputSettings();
    void getPrinterSettingsFromControls(DicomPrinter& printer);

};
};
#endif
