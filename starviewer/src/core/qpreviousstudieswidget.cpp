/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qpreviousstudieswidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QIcon>
#include <QPushButton>
#include <QMovie>
#include <QFont>
#include <QMessageBox>

#include "logging.h"
#include "study.h"
#include "../inputoutput/previousstudiesmanager.h"

namespace udg {

QPreviousStudiesWidget::QPreviousStudiesWidget( Study * inputStudy, QWidget *parent )
   : QFrame(parent)
{
    setWindowFlags( Qt::Popup );
    QVBoxLayout * verticalLayout = new QVBoxLayout( this );

    m_lookingForStudiesWidget = new QWidget(this);
    m_previousStudiesTree = new QTreeWidget(this);
    m_previousStudiesManager = new PreviousStudiesManager();

    initializeLookinForStudiesWidget();
    initializeTree();

    verticalLayout->addWidget( m_lookingForStudiesWidget );
    verticalLayout->addWidget( m_previousStudiesTree );

    createConnections();


    m_lookingForStudiesWidget->setVisible( true );
    m_previousStudiesTree->setVisible( false );

    m_previousStudiesManager->queryPreviousStudies( inputStudy );

}

QPreviousStudiesWidget::~QPreviousStudiesWidget()
{

}

void QPreviousStudiesWidget::createConnections()
{
    connect( m_previousStudiesManager, SIGNAL(queryPreviousStudiesFinished(QList<Study*>,QHash<QString,QString>)) , this , SLOT( insertStudiesToTree( QList<Study*>,QHash<QString,QString>) ) );
}

void QPreviousStudiesWidget::initializeTree()
{

    // Inicialitzem la capçalera
    QStringList labels;
    labels << "" << "" <<  "" << tr("Date") << tr("Hour") << tr("Modality") << tr("Description");
    m_previousStudiesTree->setHeaderLabels( labels );

    // Fem 7 columnes perquè la primera l'amagarem
    m_previousStudiesTree->setColumnCount( 7 );

    m_previousStudiesTree->setColumnHidden( 0 , true );

    m_previousStudiesTree->setAlternatingRowColors( true );

    // El farem visible quan rebem la llista d'estudis previs
    m_previousStudiesTree->setVisible( false );

}

void QPreviousStudiesWidget::initializeLookinForStudiesWidget()
{

    QHBoxLayout * horizontalLayout = new QHBoxLayout( m_lookingForStudiesWidget );


    QLabel * downloadigAnimation = new QLabel();
    QMovie *operationAnimation = new QMovie();
    operationAnimation->setFileName(":/images/loader.gif");
    downloadigAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    horizontalLayout->addWidget( downloadigAnimation );
    horizontalLayout->addWidget( new QLabel( tr("Looking for previous studies...") ) );

}

void QPreviousStudiesWidget::insertStudyToTree(Study * study)
{
    QTreeWidgetItem * item = new QTreeWidgetItem();

    //Afegim l'item al widget
    m_previousStudiesTree->addTopLevelItem( item );

    item->setFlags( Qt::ItemIsEnabled );

    // Definim la mida de lletra que utilitzerem per l'item
    QFont font;
    font.setPointSize( 12 );

    for ( int i = 0 ; i < m_previousStudiesTree->columnCount() ; i++ )
        item->setFont( i , font );

    item->setText( 3 , study->getDateAsString() );
    item->setText( 4 , study->getTimeAsString() );
    item->setText( 5 , study->getModalitiesAsSingleString() );
    item->setText( 6 , study->getDescription() );



//    QLabel * downloadigAnimation = new QLabel();
//    QMovie *operationAnimation = new QMovie();
//    //operationAnimation->setFileName(":/images/loader.gif");
//    operationAnimation->setScaledSize( QSize(12,12) );
//    downloadigAnimation->setMovie(operationAnimation);
//    operationAnimation->start();
//
//    m_previousStudiesTree->setItemWidget( item , 1 , downloadigAnimation );

    QIcon dowloadIcon( QString(":/images/down.png") );
    QPushButton * downloadButton = new QPushButton( dowloadIcon , QString("") );
    downloadButton->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum);
    downloadButton->setMaximumSize( 20, 13 );
    downloadButton->setMinimumSize( 20, 13 );

    QFont buttonFont;
    buttonFont.setPointSize( 9 );
    downloadButton->setFont( buttonFont );

    m_previousStudiesTree->setItemWidget( item , 2 , downloadButton );

}

void QPreviousStudiesWidget::updateWidthTree()
{
    int fixedSize = 0;
    for ( int i = 1 ; i < m_previousStudiesTree->columnCount() ; i++ )
    {
        m_previousStudiesTree->resizeColumnToContents( i );
        fixedSize += m_previousStudiesTree->columnWidth(i);
    }
    m_previousStudiesTree->setFixedWidth( fixedSize + 20 );
}

void QPreviousStudiesWidget::insertStudiesToTree(  QList<Study*> studiesList , QHash<QString, QString> hashPacsIDOfStudyInstanceUID )
{
    foreach( Study *study, studiesList )
    {
        DEBUG_LOG("eeeee");
        insertStudyToTree( study );
    }

    m_previousStudiesTree->sortByColumn( 3 , Qt::AscendingOrder );

    m_lookingForStudiesWidget->setVisible( false );
    m_previousStudiesTree->setVisible( true );

    updateWidthTree();
}

void QPreviousStudiesWidget::clicked()
{
    QMessageBox::warning(this, tr("Export to DICOM") , tr("This action is not allowed because the selected viewer is empty.") );
}

}
