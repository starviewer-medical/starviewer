/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "interactivewidget.h"

#include "patientitemmodel.h"
#include "patientitem.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "logging.h"

#include <QtGui>
#include <QStringListModel>

namespace udg {

InteractiveWidget::InteractiveWidget( QWidget *parent)
{
    setupUi( this );
    setAlternatingRowColors( true );
    setMinimumHeight( 25 );
    setMaximumHeight( 25 );
//     buttonMenu = new RightButtonMenu();
//     buttonMenu->show();
}

InteractiveWidget::~InteractiveWidget()
{
}

void InteractiveWidget::enterEvent( QEvent * event )
{
    setMinimumHeight( 150 );
    setMaximumHeight( 150 );
}

void InteractiveWidget::leaveEvent( QEvent * event )
{
    setMinimumHeight( 25 );
    setMaximumHeight( 25 );
}

/*
void InteractiveWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        buttonMenu->show();
    }
}
*/

void InteractiveWidget::setVolume( Volume * volume)
{ 
    // Creació de l'estructura pacient->study->serie
    Patient *patient = new Patient();
    patient->setFullName( "Pacient de proves" );

    Study *study = new Study(patient);
    study->setInstanceUID( "1" );
    study->setDate( 1,1,1111);
    study->setDescription( "study_1_description" );
    patient->addStudy( study );
    Study *study_2 = new Study(patient);
    study_2->setInstanceUID( "2" );
    study_2->setDate( 2,2,2222);
    study_2->setDescription( "study_2_description" );
    patient->addStudy( study_2 );

    Series *series = new Series();
    series->setInstanceUID( "1_1" );
    series->setProtocolName( "serie_protocolName" );
    series->setDescription( "serie_description" );
    series->setModality( "serie_modality" );
    series->setVolume( volume );
    study->addSeries( series );
    Series *series_2 = new Series();
    series_2->setInstanceUID( "1_2" );
    series_2->setProtocolName( "serie_2_protocolName" );
    series_2->setDescription( "serie_2_description" );
    series_2->setModality( "serie_2_modality" );
    series_2->setVolume( volume );
    study->addSeries( series_2 );

    Series *series_3 = new Series();
    series_3->setInstanceUID( "2_1" );
    series_3->setProtocolName( "serie_3_protocolName" );
    series_3->setDescription( "serie_3_description" );
    series_3->setModality( "serie_3_modality" );
    series_3->setVolume( volume );
    study_2->addSeries( series_3 );
    Series *series_4 = new Series();
    series_4->setInstanceUID( "2_2" );
    series_4->setProtocolName( "serie_4_protocolName" );
    series_4->setDescription( "serie_4_description" );
    series_4->setModality( "serie_4_modality" );
    series_4->setVolume( volume );
    study_2->addSeries( series_4 );
    
//     DEBUG_LOG( QString("------Inici creacio del model----") );
// 
//     PatientItemModel * model = new PatientItemModel( patient );
//     DEBUG_LOG( QString( "------Model creat----" ) );
//     DEBUG_LOG( model->toString() );
// 
//     DEBUG_LOG( QString( "------Inici assignacio del model-----" ) );



//     QStandardItemModel * modelnumb = new QStandardItemModel();
// 
//     QIcon * icon = new QIcon("/home/ester/starviewer/src/main/images/axial.png");
//     QString text = "text";
//     QList<QStandardItem*> listPatient;
//     QList<QStandardItem*> listStudy;
//     QList<QStandardItem*> listSeries;

//     QStandardItem * root = new QStandardItem();
//     root->setText("root");
//     QStandardItem * item1 = new QStandardItem();
//     QStandardItem * item2 = new QStandardItem();
//     QStandardItem * item2_2 = new QStandardItem();
//     QStandardItem * item3 = new QStandardItem();
//     listPatient << item1;
//     listStudy << item2;
//     listSeries << item3;
//     item1->setText("patient");
//     item2->setText("study_1");
//     item2_2->setText("study_2");
//     item3->setText("serie");
//     modelnumb->setItem( 0,0, root );
//     root->setChild( 0, item1 );
// //     item1->setChild( 0, item2 );
// //     item1->setChild( 1, item2_2 );
// //     item2->setChild( 0, item3 );
// 
// 
//     QList<QStandardItem*> listStudyColumns;
//     QStandardItem * col1 = new QStandardItem();
//     col1->setText( study->getDate().toString( "dd/MM/yyyy") );
//     QStandardItem * col2 = new QStandardItem();
//     col2->setText( /*study->getModality()*/ "--" );
//     QStandardItem * col3 = new QStandardItem();
//     col3->setText( study->getDescription() );
// 
//     listStudyColumns << item2 << col1 << col2 << col3;
//     item1->insertRow( 0, listStudyColumns );
//     listStudyColumns.clear();
//     listStudyColumns << item2_2;
//     item1->insertRow( 1, listStudyColumns );
// 
//     listStudyColumns.clear();
//     listStudyColumns << item3;
//     item2->insertRow( 0, listStudyColumns );
//     
// 
//     modelnumb->setHorizontalHeaderLabels( QStringList() << tr("Tipus") << tr("Data") << tr("Modalitat") << tr("Descripcio"));

    PatientItemModel * patientModel = new PatientItemModel( patient );

    this->setModel( patientModel->getModel() );

//     buttonMenu->setPatient( patient );
//     buttonMenu->setPosition( QPoint( 200,200 ) );


//     QStandardItem * patientData = root->child( 1 );
//     QVariant variant = (patientData->data(Qt::DisplayRole));
//     DEBUG_LOG( QString (variant.typeName() ) );
//     if (variant.isValid())
//     {
//         DEBUG_LOG( QString( "Valid" ) );
//         DEBUG_LOG( variant.toString() );
//     }
//     QList<QVariant> listDataPatient = (patientData->data(Qt::DisplayRole)).toList();
//     int label = 0;
// 
//     while( label < listDataPatient.size() )
//     {
//         QString data = listDataPatient.value( label ).toString();
//         DEBUG_LOG( data );
//         label++;
//     }

}

void InteractiveWidget::setPatient(Patient * patient)
{
    PatientItemModel * patientModel = new PatientItemModel( patient );

    this->setModel( patientModel->getModel() );
}

/*
void InteractiveWidget::setModel( QStandardItemModel * model )
{

    int i,j,k,label;
    QStandardItem * patient;
    QStandardItem * patientData;
    QStandardItem * study;
    QStandardItem * serie;

    DEBUG_LOG( QString( "------Inici setModel-----" ) );

    ///Construim un widget per cada serie
    QStandardItem * root = model->item(0,0);

    int rowCount = root->rowCount();
    
    for ( i = 0; i < root->rowCount(); i++ )
    {
        patient = root->child( 0 );
        int row = patient->row();
        int column = patient->column();
        patientData = root->child( 1 );
        QList<QVariant> listDataPatient = (patientData->data(Qt::DisplayRole)).toList();
        label = 0;

        while( label < listDataPatient.size() )
        {
            QString data = listDataPatient.value( label ).toString();
            DEBUG_LOG( data );
            label++;
        }

        for ( j = 0; j < patient->rowCount(); j++ )
        {
            study = patient->child( j,0 );
            for ( k = 0; k < study->rowCount(); k++ )
            {
                serie = study->child( k,0 );
//                 QWidget newWidget =  createWidget( patient,study,serie);
            }
        }

    }

}
*/
}
