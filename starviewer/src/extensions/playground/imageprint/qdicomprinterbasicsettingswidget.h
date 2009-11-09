
#ifndef UDGQDICOMPRINTERBASICSETTINGSWIDGETBASE_H
#define UDGQDICOMPRINTERBASICSETTINGSWIDGETBASE_H

#include "ui_qdicomprinterbasicsettingswidgetbase.h"

#include "dicomprinter.h"

namespace udg {

///Classe que mostra i permet especificar els paràmetres bàsics de configuració d'una impressora dicom;
class QDicomPrinterBasicSettingsWidget : public QWidget , private::Ui::QDicomPrinterBasicSettingsWidgetBase {
Q_OBJECT

public:
    QDicomPrinterBasicSettingsWidget(QWidget *parent = 0);

    ///Retorna/Mostra els basic printer settings
    void getDicomPrinterBasicSettings(DicomPrinter &dicomPrinter);
    void setDicomPrinterBasicSettings(DicomPrinter &dicomPrinter);

    ///Neteja els controls
    void clear();

private :

    ///Emplena DicomPrinter amb  les dades del GroupBox Film Settings
    void getFilmSettings(DicomPrinter &dicomPrinter);

    ///Emplena DicomPrinter amb les dades del GroupBox Print Settings
    void getPrintSettings(DicomPrinter &dicomPrinter);

    ///Mostra els paràmetres de Film Settings
    void setFilmSettings(DicomPrinter &dicomPrinter);

    ///Mostra els paràmetres del Print Settings
    void setPrintSettings(DicomPrinter &dicomPrinter);
};
} // end namespace udg.

#endif
