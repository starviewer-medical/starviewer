/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTITEM_H
#define UDGPATIENTITEM_H

#include <QList>
#include <QVariant>

namespace udg {

class Patient;
class Study;
class Series;

/**
Classe que representa un item de pacient, per tal de crear un model basat en els pacients

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientItem {

public:
    PatientItem( Patient *patient, PatientItem *parent = 0 );
    PatientItem( Study *study, PatientItem *parent = 0 );
    PatientItem( Series *series, PatientItem *parent = 0 );

    ~PatientItem();

    void appendChild(PatientItem *child);

    PatientItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    PatientItem *parent();

    ///Mètodes de proves
    QString toString();

private:

    QList<PatientItem*> childItems;
    QList<QVariant> itemData;
    PatientItem *parentItem;
};

}
#endif
