
/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef UDGSTUDYLISTVIEW_H
#define UDGSTUDYLISTVIEW_H

#include "qstudylistviewbase.h"
#include <qstring.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qevent.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include "study.h"
#include "series.h"
#include "pacsparameters.h"
#include "studylist.h"

namespace udg {

/** Aquesta classe és un widget millorar i modificat del listview, que permet mostrar estudis i sèries d'una manera organitzada i fàcilment
@author marc
*/
class QStudyListView : public QStudyListViewBase
{
Q_OBJECT
public:



    QStudyListView(QWidget *parent = 0, const char *name = 0);

    QString formatName(const std::string);
    QString formatAge(const std::string);
    QString formatDate(const std::string);
    QString formatHour(const std::string);
    
    void insertSeries(Series *);
    void showStudyList(StudyList *);
    void removeStudy(QString StudyUID);
 
    void setSortColumn(int);
    
    QString getSelectedStudyPacsAETitle();
    QString getSelectedStudyUID();
    QString getSelectedSeriesUID();
    ~QStudyListView();
    QPopupMenu *m_popupMenu;

signals :
    void click(QString,QString);
    void retrieve();
    void delStudy();
    void view();
    void addSeries(Series *serie);
    void clearIconView();
    void selectedSeriesList(QString);
        
public slots:
    void expand(QListViewItem *,const QPoint &,int);
    void popupMenuShow(QListViewItem *,const QPoint&,int);
    void selectedSeriesIcon(int index);
    
    void clicked(QListViewItem *);
    void clear();
    void retrieveStudy();
    void deleteStudy();
    void viewStudy();


private :
    QPixmap m_openFolder;
    QPixmap m_closeFolder;
    QPixmap m_iconSeries;
    QString m_oldStudyUID;
    std::string m_oldPacsAETitle,m_OldInstitution;
    void insertStudy(Study *);
    void createPopupMenu(QString);
    void setSeriesToIconView(QListViewItem *item);

};

};

#endif
