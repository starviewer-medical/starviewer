
#ifndef UDGQDICOMPRINTEXTENSION_H
#define UDGQDICOMPRINTEXTENSION_H

#include "ui_qdicomprintextensionbase.h"
#include "dicomprint.h"

namespace udg {

class ImagePrintFactory;
class QPrintJobCreatorWidget;
class QPrinterConfigurationWidget;
class Image;
class DicomPrintPage;
class DicomPrintJob;
class Volume;
class ToolManager;

class QDicomPrintExtension : public QWidget , private::Ui::QDicomPrintExtensionBase {
Q_OBJECT

public:
    QDicomPrintExtension( QWidget *parent = 0 );
    ~QDicomPrintExtension();

public slots:

    void configurationPrinter();

    ///Li assigna el volum que s'ha d'imprimir
    void setInput( Volume *input );
    /// Actualitzem tots els elements que estan lligats al Volume que estem visualitzant.
    void updateInput( Volume *input );

private slots:

    //Actualitza la informació que mostrem de la impressora 
    void selectedDicomPrinterChanged(int indexOfSelectedDicomPrinter);
    ///Omple el combobox amb les impressores i mostra com a seleccionada la que esta marca com a impressora per defecte
    void  fillSelectedDicomPrinterComboBox();
    ///activa/desactiva el fram que conté poder escollir el interval i des de quina imatge fins quina imprimir en funció del mode de selecció d'imatges escollit
    void imageSelectionModeChanged();
    ///Slot que s'executa quan canviem de valor a través del Slider l'interval d'imatges a imprimir
    void m_intervalImagesSliderValueChanged(int value);
    ///Slot que s'executa quan canviem de valor a través del Slider a partir de quina imatge imprimir
    void m_fromImageSliderValueChanged(int value);
    ///Slot que s'executa quan canviem de valor a través del Slider fins quina imatge imprimir
    void m_toImageSliderValueChanged(int value);
    ///Slot que s'exectua quan el LineEdit per especificar el valor del interval d'imatges a imprimir és editat, per traspassar el seu valor al Slider
    void m_intervalImagesLineEditTextEdited(const QString &text);
    ///Slot que s'exectua quan el LineEdit per especificar a partir de quina imatges s'haa imprimir és editat, per traspassar el seu valor al Slider
    void m_fromImageLineEditTextEdited(const QString &text);
    ///Slot que s'exectua quan el LineEdit per especificar fins quina imatge s'ha d'imprimir és editat, per traspassar el seu valor al Slider
    void m_toImageLineEditTextEdited(const QString &text);
    ///Actualitza label indicant el número de pàgines dicom print que s'imprimiran
    void updateNumberOfDicomPrintPagesToPrint();

    ///Envia a imprimir les imatges seleccionades a la impressora seleccionada
    void print();

private:
    ImagePrintFactory		* m_factory;
    QPrinterConfigurationWidget *m_printerConfigurationWidgetProof;

    ///Crea les connexions
    void createConnections();

    ///Crea inputValidators pels lineEdit de la selecció d'imatges
    void configureInputValidator();

    /// Inicialitza les tools que volem tenir activades al viewer
    void initializeViewerTools();

    ///Configura els controls de selecció d'imatges en funció dels nombre d'imatges
    void updateSelectionImagesValue();

    ///Retorna el DicomPrintJob que s'ha d'enviar a imprimir en funció de la impressora i imatges seleccionades
    DicomPrintJob getDicomPrintJobToPrint();

    ///Retorna una llista de pàgines per imprimir
    QList<DicomPrintPage> getDicomPrintPageListToPrint();

    ///Retorna les imatges s'han d'enviar a imprimir en funció de lo definit a la selecció d'imatges
    QList<Image*> getSelectedImagesToPrint();

    ///Retorna la impressora seleccionada
    DicomPrinter getSelectedDicomPrinter();

    ///Indica amb la selecció actua el número de pàgines a imprimir, sinó l'ha pogut calcu·lar correctament retorna 0
    int getNumberOfPagesToPrint();

    ///Retorna el número d'imatges que caben en la pàgina en funció del layout escollit
    int getNumberOfImagesPerFilm(QString filmLayout);

    ///ens retorna un DicomPrintPage amb els paràmetres d'impressió omplerts a partir d'una impressora. No afegeix les imatges ni número de pàgina
    DicomPrintPage fillDicomPrintPagePrintSettings(DicomPrinter dicomPrinter);

	///Mostra per pantalla els errors que s'han produït alhora d'imprimir
	/*Degut a només podem tenir una pàgina per FilmSession degut a limitacions de dcmtk fa que haguem d'imprimir pàgina per pàgina 
	  per això ens podem trobar que la primera pàgina s'imprimeixi bé, i les restants no, per això passem el paràmetre printedSomePage per indica que només
      algunes de les pàgines han fallat*/
	void showDicomPrintError(DicomPrint::DicomPrintError error, bool printedSomePage);
    
    /// Comprova si se suporta el format de la sèrie i actualitza la interfície segons convingui.
    void updateVolumeSupport();

private:
    /// Gestor de tools pel viewer
    ToolManager *m_toolManager;
};

} // end namespace udg.

#endif
