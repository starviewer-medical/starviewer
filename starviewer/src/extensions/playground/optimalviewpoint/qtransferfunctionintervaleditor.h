/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQTRANSFERFUNCTIONINTERVALEDITOR_H
#define UDGQTRANSFERFUNCTIONINTERVALEDITOR_H


#include "ui_qtransferfunctionintervaleditorbase.h"


namespace udg {


/**
 * Aquesta classe representa un interval de l'editor de funcions de
 * transferència per valors (QTransferFunctionEditorByValues).
 *
 * Pot comportar-se de dues maneres segons un checkbox: si està marcat es
 * comporta com un interval i altrament es comporta com un punt. Quan es
 * comporta com a punt el que val és el punt inicial (start), i en cas que es
 * comporti com a interval hi ha el punt inicial (start) i el final (end).
 * Sempre es compleix que start <= end, i tant l'inici com el final sempre estan
 * entre 0 i 255 (ambdós inclosos).
 *
 * Tant si és un punt com un interval, sempre té associat un únic valor RGBA. El
 * color es pot triar amb un diàleg de selecció de color i l'opacitat amb el
 * mateix diàleg o directament amb un spinbox.
 *
 * Un interval té dues propietats importants que poden estar activades o
 * desactivades. Són isFirst i isLast. La primera diu que l'interval és el
 * primer, i fa que start = 0. La darrera diu que l'interval és l'últim i fa que
 * end = 0.
 *
 * Hi ha signals i slots que permeten comunicar-se amb altres instàncies de la
 * mateixa classe per evitar solapaments d'intervals.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
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
