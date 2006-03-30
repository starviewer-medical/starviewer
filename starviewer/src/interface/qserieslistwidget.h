/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSERIESLISTWIDGET_H
#define UDGSERIESLISTWIDGET_H

// #include "qseriesiconviewbase.h"
#include "ui_qserieslistwidgetbase.h"
#include <QWidget>
#include "series.h"
namespace udg {

/** Aquesta classe és un widget millorat del ListWidget de QT, fet  modificat per poder mostrar les sèries d'un estudi. 
@author marc
*/

class QSeriesListWidget : public QWidget , private Ui::QSeriesListWidgetBase{
Q_OBJECT

public:
    QSeriesListWidget(QWidget *parent = 0 );

    void insertSeries(Series *serie);
    void clear();
    
    ~QSeriesListWidget();

public slots:
    void clearSeriesListWidget();
    void addSeries(Series *serie);

    void clicked(QListWidgetItem * item);
    void selectedSeriesList(QString key);
    void view(QListWidgetItem *item);

signals :
    void selectedSeriesIcon(QString);
    void viewSeriesIcon();
    
private :

    QString m_className; 
    void createConnections();
};

};

#endif
