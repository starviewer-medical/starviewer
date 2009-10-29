/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWSERMENULIST_H
#define UDGPATIENTBROWSERMENULIST_H

#include <QWidget>
#include <QKeyEvent>

class QVBoxLayout;

namespace udg {

class PatientBrowserMenuBasicItem;

/**
    Widget que representa el contingut del menú en un llistat d'ítems
    Aquests ítems s'organitzaran en files i columnes dins d'un grup al que li donarem un "caption"

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuList : public QWidget {
Q_OBJECT
public:
    PatientBrowserMenuList( QWidget *parent = 0 );
    ~PatientBrowserMenuList();

    /// Afegeix un grup d'ítems amb le caption donat
    void addItemsGroup( const QString &caption, const QList< QPair<QString,QString> > &itemsList );

    /// Marquem com a seleccionat l'ítem que tingui l'identificador donat
    void setSelectedItem( const QString &identifier );

    /// Tractament dels events
    bool event( QEvent * event );

signals:
    /// Senyal que s'emet quan el mouse entra en el widget i ens notifica el seu identificador
    void isActive(QString);

    /// Senyal que s'emet quan no hi ha cap item activat
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una ítem i ens notifica el seu identificador
    void selectedItem(QString);

    /// Signal que s'emet quan es rep l'event de tancar el menu
    void close();

private:
    /// Creem un widget amb la informació bàsica d'un ítem
    PatientBrowserMenuBasicItem *createBasicItem( const QString &label, const QString &identifier );

    /// Llista dels items de les series que formen la llista
    QList<PatientBrowserMenuBasicItem *> m_itemsList;

    /// Layout on colocarem els elements
    QVBoxLayout *m_verticalLayout;
};

}

#endif
