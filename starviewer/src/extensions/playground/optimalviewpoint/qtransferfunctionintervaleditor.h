/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQTRANSFERFUNCTIONINTERVALEDITOR_H
#define UDGQTRANSFERFUNCTIONINTERVALEDITOR_H

#include "ui_qtransferfunctionintervaleditorbase.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <jspinola@gmail.com>
*/
class QTransferFunctionIntervalEditor
    : public QWidget, public Ui::QTransferFunctionIntervalEditorBase
{

    Q_OBJECT

public:

    QTransferFunctionIntervalEditor( QWidget * parent = 0 );

    virtual ~QTransferFunctionIntervalEditor();

    void setIsFirst( bool isFirst );
    void setIsLast( bool isLast );
    bool isInterval() const;
    int start() const;
    int end() const;
    QColor color() const;
    void setStart( int start );
    void setEnd( int end );
    void setColor( QColor color );
    QGradientStops getInterval() const;

public slots:

    void setIsInterval( bool isInterval );
    void setPreviousEnd( int previousEnd );
    void setNextStart( int nextStart );

private slots:

    void isIntervalToggled( bool checked );
    void adjustWithNewStart( int start );
    void adjustWithNewEnd( int end );
    void selectColor();

signals:

    void startChanged( int start );
    void endChanged( int end );

private:

    void firstAndLast();

    bool m_isFirst, m_isLast;

};

}

#endif
