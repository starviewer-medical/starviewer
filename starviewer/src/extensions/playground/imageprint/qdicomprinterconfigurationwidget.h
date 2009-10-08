#ifndef UDGQDICOMPRINTERCONFIGURATIONWIDGET_H
#define UDGQDICOMPRINTERCONFIGURATIONWIDGET_H

#include "ui_qdicomprinterconfigurationwidgetbase.h"
#include "qprinterconfigurationwidget.h"

namespace udg {

class DicomPrinter;

/**
 * Implementació de la interfície QPrinterConfigurationWidget per impressió d'imatge DICOM en DCMTK.
 */

class QDicomPrinterConfigurationWidget : public QPrinterConfigurationWidget , private::Ui::QDicomPrinterConfigurationWidgetBase
{
Q_OBJECT

public:
    QDicomPrinterConfigurationWidget();
    ~QDicomPrinterConfigurationWidget();
	
public slots:
    // Acualiza la informació del Printer cada vegada que es selecciona al QTree una impressora.
    void printerSelectionChanged();
    // Afegeix una nova impressora Dicom a partir dels paràmetres introduits per l'usuari.
    void addPrinter();
    // Modifica una impressora Dicom ja existen amb els paràmetres introduits per l'usuari.
    void updatePrinter();
    // Esborra una impressora.
    void deletePrinter();
    // Comprova la connexió amb una impressora entrada al sistema.
    void testPrinter();
    // Mostra per pantalla els diferents valors que poden agafar cadascun dels paràmetres d'una impressora.
    void getAvailableParameters();

private:
    void createConnections();
    // Actualitza la llista d'impressores entrades al sistema.
    void refreshPrinterList();
    // Comprova la validesa dels pàrametres d'entrada.
    bool validatePrinterSettings();
    // Neteja els valors de la interífice.
    void clearPrinterSettings();
    // Es mostra la informació bàsica de la impressora per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setPrinterSettingsToControls(DicomPrinter& printer);
    // Es guarda la informació bàsica de la impressora de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getPrinterSettingsFromControls(DicomPrinter& printer);
    // Es mostra la informació avançada de la impressora per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setDefaultPrinterSettingsToControls(DicomPrinter& printer);
     // Es guarda la informació avançada de la impressora de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getDefaultPrinterSettingsFromControls(DicomPrinter& printer);
    // Es mostra la informació del Job de la impressora per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setDefaultJobSettingsToControls(DicomPrinter& printer);
    // Es guarda la informació del Job de la impressora de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getDefaultJobSettingsFromControls(DicomPrinter& printer);
    
    int m_selectedPrinterId;
    bool m_semafor;
};
};
#endif
