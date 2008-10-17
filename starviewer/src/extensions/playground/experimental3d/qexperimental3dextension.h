#ifndef UDGQEXPERIMENTAL3DEXTENSION_H
#define UDGQEXPERIMENTAL3DEXTENSION_H


#include "ui_qexperimental3dextensionbase.h"


namespace udg {


class Experimental3DVolume;
class Volume;


/**
 * Aquesta extensió pretén ser com OptimalViewpoint però ben feta.
 */
class QExperimental3DExtension : public QWidget, private ::Ui::QExperimental3DExtensionBase {

    Q_OBJECT

public:

    QExperimental3DExtension( QWidget *parent = 0 );
    ~QExperimental3DExtension();

    /// Assigna el volum d'entrada.
    void setInput( Volume *input );

private:

    /// Crea les connexions de signals i slots.
    void createConnections();

private slots:

    /// Obre un diàleg per triar el color de fons.
    void chooseBackgroundColor();
    /// Habilita/inhabilita la il·luminació especular.
    void enableSpecularLighting( bool on );
    /// Obre un diàleg per carregar una funció de transferència.
    void loadTransferFunction();
    /// Obre un diàleg per desar una funció de transferència.
    void saveTransferFunction();
    /// Fa la visualització amb les opcions seleccionades.
    void doVisualization();

private:

    Experimental3DVolume *m_volume; // el destruirà el visor

};


}


#endif
