#ifndef QGRAPHICTRANSFERFUNCTIONEDITOR_H
#define QGRAPHICTRANSFERFUNCTIONEDITOR_H


#include "ui_qgraphictransferfunctioneditorbase.h"


namespace udg {


class QGraphicTransferFunctionEditor : public QWidget, private ::Ui::QGraphicTransferFunctionEditorBase {

    Q_OBJECT

public:

    QGraphicTransferFunctionEditor( QWidget *parent = 0 );
    ~QGraphicTransferFunctionEditor();

    void setTransferFunction( const TransferFunction &transferFunction );
    const TransferFunction& transferFunction() const;

public slots:

    /// Assigna el rang de valors de la funció de transferència.
    void setRange( int minimum, int maximum );

private slots:

    /// Assigna el rang de valors de la funció de transferència.
    void setBasicRange( double minimum, double maximum );
    void setMinimum( int minimum );
    void setMaximum( int maximum );
    void setScroll( int scroll );

};


} // namespace udg


#endif // QGRAPHICTRANSFERFUNCTIONEDITOR_H
