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

namespace udg {

class Patient;
class Study;
class Series;
class PatientBrowserMenuBasicItem;

/**
Widget per representar visualment en un menú les dades bàsiques de l'estructura Pacient -> Estudy -> Serie mitjançant widgets seleccionables (PatientBrowserMenuBasicItem).

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenuList : public QWidget {
Q_OBJECT
public:
    PatientBrowserMenuList( QWidget *parent = 0 );
    ~PatientBrowserMenuList();

    /// Posem el pacient al widget
    void setPatient( Patient *patient );

    /// Posem una serie com a seleccionada
    void setSelectedItem( const QString &identifier );

    /// Tractament dels events
    bool event( QEvent * event );

private:
    /// Creem un widget amb la informació bàsica d'un estudi
    QWidget * createStudyWidget( Study * study, QWidget * parent );

    /// Creem un widget amb la informació bàsica d'una sèrie
    PatientBrowserMenuBasicItem * createSerieWidget( Series * serie, QWidget * parent );

    /// Llista dels items de les series que formen la llista
    QList< PatientBrowserMenuBasicItem *> m_itemsList;

signals:
    /// Senyal que s'emet quan el mouse entra en el widget i ens notifica el seu identificador
    void isActive(QString);

    /// Senyal que s'emet quan no hi ha cap item activat
    void isNotActive();

    /// Aquest senyal s'emet quan s'escull una ítem i ens notifica el seu identificador
    void selectedItem(QString);

    /// Signal que s'emet quan es rep l'event de tancar el menu
    void close();

};

}

#endif
