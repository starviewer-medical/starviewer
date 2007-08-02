/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientitemmodel.h"
#include "patientitem.h"
#include "patient.h"
#include "study.h"
#include "series.h"

#include <QAction>
#include "logging.h"
//Proves
#include <QHashIterator>

namespace udg {

PatientItemModel::PatientItemModel( Patient * patient, QObject *parent )
 : QAbstractItemModel ( parent )
{
    DEBUG_LOG( QString("Constructor PatientItemModel") );
    m_modelPatient = createModel( patient );
//     setupModelData(patient, rootItem);
}

PatientItemModel::~PatientItemModel()
{
    DEBUG_LOG( QString("Destructor PatientItemModel") );
}

QVariant PatientItemModel::data(const QModelIndex &index, int role) const
{
    DEBUG_LOG( QString("Inici mètode data") );

//     if (!index.isValid())
//          return QVariant();
// 
//      if (role != Qt::DisplayRole)
//          return QVariant();
// 
//      PatientItem *item = static_cast<PatientItem*>(index.internalPointer());
// 
//      return item->data(index.column());
    return m_modelPatient->data(index,role);
}


Qt::ItemFlags PatientItemModel::flags(const QModelIndex &index) const
{
    DEBUG_LOG( QString("Inici mètode flags") );
}

QVariant PatientItemModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    DEBUG_LOG( QString("Inici mètode headerData") );

    return ( QStringList() << tr("Tipus") << tr("Data") << tr("Modalitat") << tr("Descripcio") );

}

QModelIndex PatientItemModel::index(int row, int column, const QModelIndex &parent ) const
{
    PatientItem *patientItem;

     if (!parent.isValid())
         patientItem = m_rootItem;
     else
         patientItem = static_cast<PatientItem*>(parent.internalPointer());

     PatientItem *childItem = patientItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();

    DEBUG_LOG( QString("Inici mètode index") );
}

QModelIndex PatientItemModel::parent(const QModelIndex &index) const
{
    DEBUG_LOG( QString("Inici mètode parent") );

    if (!index.isValid())
         return QModelIndex();

     PatientItem *childItem = static_cast<PatientItem*>(index.internalPointer());
     PatientItem *patientItem = childItem->parent();

     if (patientItem == m_rootItem)
         return QModelIndex();

     return createIndex(patientItem->row(), 0, patientItem);
}

int PatientItemModel::rowCount(const QModelIndex &parent) const
{   
    DEBUG_LOG( QString("Inici mètode rowCount") );
    return m_modelPatient->rowCount();
}   

int PatientItemModel::columnCount(const QModelIndex &parent) const
{
    DEBUG_LOG( QString("Inici mètode columnCount") );

    return m_modelPatient->columnCount();

    if (parent.isValid())
    {
        DEBUG_LOG( QString("Retorna") + (static_cast<PatientItem*>(parent.internalPointer())->columnCount()) );
         return static_cast<PatientItem*>(parent.internalPointer())->columnCount();
    }
    else{
         DEBUG_LOG( QString("no index. Retorna %1") .arg(m_rootItem->columnCount()) );
         return m_rootItem->columnCount();
    }
}

QStandardItemModel * PatientItemModel::createModel( Patient * patient )
{

// 1.- Creació del model:
// 2.- Obtenim el pacient
// 3.- Per cada study fer:
//    a) Obtenir les dades generals de l'study
//    b) Afegir la fila que derivi de pacient amb totes les dades generals de l'estudi
//    c) Per cada serie de l'estudi fer:
//         i) Obtenir les dades generals de la serie 
//        ii) Afegir una fila que derivi de study amb totes les dades generals de la serie

    DEBUG_LOG( QString("------Inici creacio del model----") );

    // Dades del pacient
    QStandardItemModel * modelPatient = new QStandardItemModel();
    QStandardItem * itemPatient;

    // Dades de l'estudi
    QStandardItem * itemStudy;
    QStandardItem * itemStudyDate;
    QStandardItem * itemStudyModality;
    QStandardItem * itemStudyDescription;
//     QList<QStandardItem*> listStudyColumns;
    Study * study;
    QList< Study* > studies = patient->getStudies();
    int numberStudy;

    // Dades de la serie
    QStandardItem * itemSerie;
    QStandardItem * itemSerieDate;
    QStandardItem * itemSerieModality;
    QStandardItem * itemSerieDescription;
//     QList<QStandardItem*> listSerieColumns;
    Series * serie;
    QList< Series* > series;
    int numberSerie;

    // Creació de l'item del pacient
    itemPatient = new QStandardItem();
    itemPatient->setText( "Patient" );
    modelPatient->setItem( 0,0, itemPatient );
    
    for( numberStudy = 0; numberStudy < patient->getNumberOfStudies(); numberStudy++ )
    {
        // Creació de l'estudi
        study = studies.value( numberStudy );
        series = study->getSeries();

        itemStudy = new QStandardItem();
        itemStudy->setText( "Study" );
        itemStudyDate = new QStandardItem();
        itemStudyDate->setText( study->getDate().toString( "dd/MM/yyyy") );
        itemStudyModality = new QStandardItem();
        itemStudyModality->setText( /*study->getModality()*/"study_modality" );
        itemStudyDescription = new QStandardItem();
        itemStudyDescription->setText( study->getDescription() );

        QList<QStandardItem*> listStudyColumns;
        listStudyColumns << itemStudy << itemStudyDate << itemStudyModality << itemStudyDescription;
        itemPatient->insertRow( numberStudy, listStudyColumns );

        for( numberSerie = 0; numberSerie < study->getNumberOfSeries(); numberSerie++ )
        {
            // Creacio de la serie
            serie = series.value( numberSerie );
            itemSerie = new QStandardItem();
            itemSerie->setText( "Serie" );
            itemSerieDate = new QStandardItem();
            itemSerieDate->setText( "" );
            itemSerieModality = new QStandardItem();
            itemSerieModality->setText( serie->getModality() );
            itemSerieDescription = new QStandardItem();
            itemSerieDescription->setText( serie->getDescription() );

            QList<QStandardItem*> listSerieColumns;
            listSerieColumns << itemSerie << itemSerieDate << itemSerieModality << itemSerieDescription;

            itemStudy->insertRow( numberSerie, listSerieColumns );
        }
    }

    modelPatient->setHorizontalHeaderLabels( QStringList() << tr("Tipus") << tr("Data") << tr("Modalitat") << tr("Descripcio"));

    return modelPatient;
}

void PatientItemModel::setupModelData(Patient * patient, PatientItem *parent)
{
    DEBUG_LOG( QString("Inici mètode setupModelData") );

    int i,j;
    PatientItem * item;
    Study * study;
    QList< Study* > studies = patient->getStudies();
    QList< Series* > series;

    for( i = 0; i < patient->getNumberOfStudies(); i++) // Afegim un node per cada estudi
    {
        study = studies.value( i );
        item = new PatientItem( study, m_rootItem );
        series = study->getSeries();

        for( j = 0; j < study->getNumberOfSeries(); j++) // Afegim un node per cada serie
        {
            item->appendChild( new PatientItem( series.value( j ), item ) );
        }

        m_rootItem->appendChild( item );
    }
}

// // QString PatientItemModel::toString()
// // {
// //     return rootItem->toString();
// // }
}
