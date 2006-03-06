/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSERIESICONVIEW_H
#define UDGSERIESICONVIEW_H

#include "qseriesiconviewbase.h"
#include "series.h"
namespace udg {

/** Aquesta classe és un widget millorat del Icon View de QT, fet  modificat per poder mostrar les sèries d'un estudi. 
@author marc
*/

class QSeriesIconView : public QSeriesIconViewBase
{
Q_OBJECT

private :

    QString m_className; 

public:
    QSeriesIconView(QWidget *parent = 0, const char *name = 0);

    void insertSeries(Series *serie);
    void clear();
    
    ~QSeriesIconView();

public slots:
    void clearIconView();
    void addSeries(Series *serie);
    void clicked(QIconViewItem * item);
    void selectedSeriesList(QString key);
    void view(QIconViewItem *item);

signals :
    void selectedSeriesIcon(int index);
    void viewSeriesIcon();
};

};

#endif
