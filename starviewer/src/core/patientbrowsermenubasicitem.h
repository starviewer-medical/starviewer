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

/**
Widget per utilitzar com a item de menú. Pot representar diversos objectes i te la funcionalitat de canviar l'aparença al situar-se el mouse sobre l'element.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuBasicItem : public QLabel {
Q_OBJECT
public:
    PatientBrowserMenuBasicItem( QWidget *parent = 0 );
    ~PatientBrowserMenuBasicItem(){}

    void setIdentifier( const QString &identifier );
    QString getIdentifier() const;

    /// La posem en negreta per marcar que es la que s'esta visualitzant
    void setFontBold();

signals:
    /// Aquest senyal s'emetrà quan el mouse entri al widget i ens notifica el seu identificador
    void isActive(QString);

    /// Aquest senyal s'emetrà quan el mouse surti del widget
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull un ítem i ens notifica el seu identificador
    void selectedItem(QString);

protected:
    /// Sobrecàrrega del mètode que tracta tots els events
    bool event( QEvent * event);

private:
    /// Identificador de l'ítem
    QString m_identifier;
};

}

#endif
