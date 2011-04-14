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
