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
    void setRange( double minimum, double maximum );

private slots:

    void setMinimum( double minimum );
    void setMaximum( double maximum );
    void keepRange( bool keep );
    void setMinimum( int minimum );

private:

    double m_range;

};


} // namespace udg


#endif // QGRAPHICTRANSFERFUNCTIONEDITOR_H
