/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTITEMMODEL_H
#define UDGPATIENTITEMMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QVariant>

namespace udg {

class PatientItem;
class Patient;

/**
Classe que representa un model del patró Model/View Programming de les Qt's,
per tal de representar en forma d'arbre les dades d'un pacient.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class PatientItemModel : public QAbstractItemModel {
Q_OBJECT
public:

    PatientItemModel( Patient * patient, QObject *parent = 0 );

    ~PatientItemModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QStandardItemModel * getModel( ){ return m_modelPatient; };
    /// Mètodes de proves
//     QString toString();

private:
    /// Construeix l'arbre a partir del pacient
    void setupModelData(Patient * patient, PatientItem *parent);

    QStandardItemModel * createModel( Patient * patient );

    QStandardItemModel * m_modelPatient;

    PatientItem *m_rootItem;
};

};

#endif
