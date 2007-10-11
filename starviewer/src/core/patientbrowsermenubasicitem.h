/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWSERMENUBASICITEM_H
#define UDGPATIENTBROWSERMENUBASICITEM_H

#include <QLabel>

namespace udg {

class Series;

/**
Widget per utilitzar com a item de menú. Pot representar diversos objectes i te la funcionalitat de canviar l'aparença al situar-se el mouse sobre l'element.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuBasicItem : public QLabel
{
Q_OBJECT
public:

    PatientBrowserMenuBasicItem( QWidget *parent = 0 );

    ~PatientBrowserMenuBasicItem(){}

    /// Posem la serie a representar
    void setSerie( Series * serie );

    /// Obtenim la serie que es representa
    Series *  getSerie();

signals:
    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void isActive(Series*);

    /// Aquest senyal s'emetrà quan el mouse surti del widget
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una serie de l'item
    void selectedSerie( Series * );

protected:
    /// Sobrecàrrega del mètode que tracta tots els events
    bool event( QEvent * event);

private:
    /// Serie que representa l'item
    Series * m_serie;
};

}

#endif
