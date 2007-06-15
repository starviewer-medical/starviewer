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

    /// Assigna la propietat isFirst.
    void setIsFirst( bool isFirst );
    /// Retorna la propietat isFirst.
    bool isFirst() const;
    /// Assigna la propietat isLast.
    void setIsLast( bool isLast );
    /// Retorna la propietat isLast.
    bool isLast() const;
    /// Retorna la propietat isInterval.
    bool isInterval() const;

    /// Assigna el començament de l'interval.
    void setStart( int start );
    /// Retorna el començament de l'interval.
    int start() const;
    /// Assigna el final de l'interval.
    void setEnd( int end );
    /// Retorna el final de l'interval.
    int end() const;
    /// Assigna el color RGBA de l'interval.
    void setColor( QColor color );
    /// Retorna el color RGBA de l'interval.
    const QColor & color() const;

public slots:

    /// Assigna la propietat isInterval.
    void setIsInterval( bool isInterval );
    /// Si previousEnd >= start --> start = previousEnd + 1.
    void setPreviousEnd( int previousEnd );
    /// Si nextStart <= end --> end = nextStart - 1.
    void setNextStart( int nextStart );

signals:

    /// S'emet quan canvia el començament de l'interval.
    void startChanged( int start );
    /// S'emet quan canvia el final de l'interval.
    void endChanged( int end );
    /// S'emet quan canvia el color RGBA de l'interval.
    void colorChanged( const QColor & color );

private:

    /// Fa els ajustos necessaris perquè l'interval sigui primer i últim alhora.
    void firstAndLast();

private slots:

    /// Fa els ajustes necessaris quan canvia la propietat isInterval.
    void isIntervalToggled( bool checked );
    /// Ajusta el final perquè sigui >= que el començament.
    void adjustWithNewStart( int start );
    /// Ajusta el començament perquè sigui <= que el final.
    void adjustWithNewEnd( int end );
    /// Obre el diàleg de selecció de color per a l'interval.
    void selectColor();

private:

    /// Propietats isFirst i isLast.
    bool m_isFirst, m_isLast;

};


}


#endif
