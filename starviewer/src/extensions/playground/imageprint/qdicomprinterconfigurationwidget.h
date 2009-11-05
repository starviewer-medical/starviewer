#ifndef UDGQDICOMPRINTERCONFIGURATIONWIDGET_H
#define UDGQDICOMPRINTERCONFIGURATIONWIDGET_H

#include "ui_qdicomprinterconfigurationwidgetbase.h"
#include "qprinterconfigurationwidget.h"

namespace udg {

class DicomPrinter;
class QDicomAddPrinterWidget;

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
    // Mostra un nou Widget en forma de Pop-up per poder introduir els paràmetres de la impressora.
    void addPrinter();
    // Modifica una impressora Dicom ja existen amb els paràmetres introduits per l'usuari.
    bool modifyPrinter();
    // Esborra una impressora.
    void deletePrinter();
    // Comprova la connexió amb una impressora entrada al sistema.
    void testPrinter();    
    // Mostra/Amaga per pantalla la informació avançada de la impressora.
    void showAdvancedSettings();
    // Després d'afegir una impressora actualitza la llista d'impressores i et mostra la informació de la última. S'executa amb el Signal newPrinterAddedSignal
    void showNewPrinterAdded();
    ///Slot que s'executa al prèmer el botó cancel
    void cancel();
    ///Slot que s'executa al prèmer el botó accept
    void accept();

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
    // Es mostra la informació relacionada amb la impressió per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setPrintSettingsToControls(DicomPrinter& printer);
     // Es guarda la informaciórelacionada amb la impressió de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getPrintSettingsFromControls(DicomPrinter& printer);
    // Es mostra la informació relacionada amb la placa a imprimir per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setFilmSettingsToControls(DicomPrinter& printer);
    // Es guarda la informació relacionada amb la placa a imprimir de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getFilmSettingsFromControls(DicomPrinter& printer);
    // Es mostra la informació avançada de la impressió per pantalla (/p printer és la impressora que conté la informació a mostrar)
    void setAdvancedSettingsToControls(DicomPrinter& printer);
    // Es guarda la informació avançada de la impressió de l'interfície a un objecte DicomPrinter (/p printer és la impressora on es guarden les dades).
    void getAdvancedSettingsFromControls(DicomPrinter& printer);
    
    ///Mostra les opcions de configuració avançada i redimensiona la finestra
    void showAdvancedConfigurationOptions(bool show);

    /**Retorna la impressora seleccionada, si no tenim cap impressora seleccionada retorna l'objecte amb les propietats buides.
      * Aquest mètode retorna la informació de la impressora com està guardada, si seleccionem una impressora, en modifiquem les dades a través 
      * d'aquesta d'interfície i cridem aquest mètode retornarà les dades tal com les teniem guardades al settings, no retornarà les dades modificada a la 
      * interfície si no s'han aplicat els canvis i guardats al settings*/
    DicomPrinter getSelectedDicomPrinter();

    int m_selectedPrinterId;
    QDicomAddPrinterWidget * m_addPrinterWidget;
};
};
#endif
