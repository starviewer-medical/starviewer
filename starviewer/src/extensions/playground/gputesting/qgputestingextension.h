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

};


}


#endif
