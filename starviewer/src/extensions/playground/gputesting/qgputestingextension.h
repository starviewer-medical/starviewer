#ifndef UDGQGPUTESTINGEXTENSION_H
#define UDGQGPUTESTINGEXTENSION_H


#include "ui_qgputestingextensionbase.h"


namespace udg {


class Volume;


/**
 * Extensió per fer proves amb GPU.
 */
class QGpuTestingExtension : public QWidget, private ::Ui::QGpuTestingExtensionBase {

    Q_OBJECT

public:

    QGpuTestingExtension( QWidget *parent = 0 );
    ~QGpuTestingExtension();

    /// Assigna el volum d'entrada.
    void setInput( Volume *input );

private:

    /// Crea les connexions de signals i slots.
    void createConnections();

private slots:

    /// Obre un diàleg per triar el color de fons.
    void chooseBackgroundColor();
    /// Obre un diàleg per carregar una funció de transferència.
    void loadTransferFunction();
    /// Obre un diàleg per desar una funció de transferència.
    void saveTransferFunction();
    /// Fa la visualització amb les opcions seleccionades.
    /// Fa la visualització.
    void doVisualization();

    /// Posa els paràmetres de la càmera actuals als spinboxs corresponents.
    void getCamera();
    /// Assigna els paràmetres de la càmera d'acord amb els spinboxs corresponents.
    void setCamera();
    /// Carrega els paràmetres de la càmera des d'un fitxer.
    void loadCamera();
    /// Desa els paràmetres de la càmera a un fitxer.
    void saveCamera();
    /// Estableix el nombre de punts de vista.

};


}


#endif
