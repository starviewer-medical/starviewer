/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWSERMENUBASICITEM_H
#define UDGPATIENTBROWSERMENUBASICITEM_H

#include <ui_patientbrowsermenubasicitembase.h>
#include <series.h>
#include <QObject>

namespace udg {

/**
Widget per utilitzar com a item de menú. Pot representar diversos objectes i te la funcionalitat de canviar l'aparença al situar-se el mouse sobre l'element.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuBasicItem : public QFrame, public Ui::PatientBrowserMenuBasicItemBase
{
Q_OBJECT
public:

    PatientBrowserMenuBasicItem( QWidget *parent = 0 );

    ~PatientBrowserMenuBasicItem(){}

    /// Posem la serie a representar
    void setSerie( Series * serie );

    /// Obtenim la serie que es representa
    Series *  getSerie();

protected:

    /// Sobrecàrrega del mètode que tracta tots els events
    bool event( QEvent * event);

    /// Serie que representa l'item
    Series * m_serie;

signals:

    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void isActive( int, Series * );

    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una serie de l'item
    void selectedSerie( Series * );
};

}

#endif
