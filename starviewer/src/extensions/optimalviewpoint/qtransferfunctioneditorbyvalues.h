/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQTRANSFERFUNCTIONEDITORBYVALUES_H
#define UDGQTRANSFERFUNCTIONEDITORBYVALUES_H

#include "ui_qtransferfunctioneditorbyvaluesbase.h"

namespace udg {

class QTransferFunctionIntervalEditor;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <jspinola@gmail.com>
*/
class QTransferFunctionEditorByValues
    : public QWidget, public Ui::QTransferFunctionEditorByValuesBase
{

    Q_OBJECT

public:

    QTransferFunctionEditorByValues( QWidget * parent = 0 );

    virtual ~QTransferFunctionEditorByValues();

    const QGradientStops & getTransferFunction() const;
    void setTransferFunction( const QGradientStops & transferFunction );

public slots:

    void addInterval();
    void removeInterval();

protected:

    QWidget * m_intervalEditorsWidget;
    QVBoxLayout * m_intervalEditorsLayout;

private slots:

    void markAsChanged();

private:

    QTransferFunctionIntervalEditor * addIntervalAndReturnIt();

    unsigned short m_numberOfIntervals;
    mutable QGradientStops m_transferFunction;
    mutable bool m_changed;

};

}

#endif
