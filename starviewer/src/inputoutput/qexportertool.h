#ifndef QEXPORTERTOOL_H
#define QEXPORTERTOOL_H

#include "ui_qexporterbase.h"
#include <QDialog>

class vtkImageData;

namespace udg {

class QViewer;
/**
    Classe encarregada de generar noves sèries a partir del que es captura per pantalla. Les series generades es poden guardar
    només a la base de dades local o poden ser enviades també al PACS seleccionat a través del widget.
    Actualment, les sèries que es generen són de tipus Secondary Capture.
*/
class QExporterTool : public QDialog , private Ui::QExporterBase
{
Q_OBJECT
public:
    /// @pre el viewer no pot ser null
    QExporterTool(  QViewer *viewer , QDialog *parent = 0 );

    ~QExporterTool();

private:
    /// Crear les connexions
    void createConnections();
    /// Inicialitza les opcions de captura segons el tipus de d'estudi. També inicialitza la llista de PACS.
    void initialize();
    /// Captura l'estat actual del visor passat al constructor @sa QExporterTool.
    vtkImageData * captureCurrentView();

private slots:
    /// Generar i guardar la nova sèrie a la bdd i enviar-la al PACS si és el cas.
    void generateAndStoreNewSeries();

    /// Slots utilitzats per actualitzar el label del número d'imatges que es generaran
    void currentImageRadioButtonClicked();
    void allImagesRadioButtonClicked();
    void imageOfCurrentPhaseRadioButtonClicked();
    void phasesOfCurrentImageRadioButtonClicked();

    /// S'utilitza per actualizar diferents elements segons quines destinacions tenim seleccionades
    void destinationsChanged( bool checked );

private:
    /// Visor passat en el constructor.
    QViewer * m_viewer;

};

}
#endif // UDGEXPORTERTOOL_H
