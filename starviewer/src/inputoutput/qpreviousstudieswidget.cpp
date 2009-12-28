/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qpreviousstudieswidget.h"
#include <QVBoxLayout>
#include <QMovie>
#include <QTreeWidgetItem>

#include "logging.h"
#include "study.h"
#include "patient.h"

#include "previousstudiesmanager.h"
#include "queryscreen.h"
#include "singleton.h"


namespace udg {

QPreviousStudiesWidget::QPreviousStudiesWidget( QWidget *parent )
   : QFrame(parent)
{
    setWindowFlags( Qt::Popup );
    QVBoxLayout * verticalLayout = new QVBoxLayout( this );

    m_lookingForStudiesWidget = new QWidget(this);
    m_previousStudiesTree = new QTreeWidget(this);
    m_previousStudiesManager = new PreviousStudiesManager();
    m_signalMapper = new QSignalMapper(this);
    m_queryScreen = SingletonPointer<QueryScreen>::instance();
    m_numberOfDownloadingStudies = 0;

    m_noPreviousStudiesLabel = new QLabel(this);
    m_noPreviousStudiesLabel->setText( tr("No previous studies.") );

    initializeLookinForStudiesWidget();
    initializeTree();

    verticalLayout->addWidget( m_lookingForStudiesWidget );
    verticalLayout->addWidget( m_noPreviousStudiesLabel );
    verticalLayout->addWidget( m_previousStudiesTree );

    createConnections();


    m_lookingForStudiesWidget->setVisible( false );
    m_noPreviousStudiesLabel->setVisible( false );
    m_previousStudiesTree->setVisible( false );

}

QPreviousStudiesWidget::~QPreviousStudiesWidget()
{
    delete m_previousStudiesTree;
    foreach ( QString key, m_infomationPerStudy.keys() )
    {
        delete m_infomationPerStudy.take( key );
    }
    delete m_previousStudiesManager;
    delete m_lookingForStudiesWidget;
    delete m_signalMapper;
    delete m_noPreviousStudiesLabel;
}

void QPreviousStudiesWidget::searchPreviousStudiesOf(Study * study)
{
    Q_ASSERT( study );

    m_lookingForStudiesWidget->setVisible( true );
    m_noPreviousStudiesLabel->setVisible( false );
    m_previousStudiesTree->setVisible( false );

    int items = m_previousStudiesTree->topLevelItemCount();
    for ( int i = 0 ; i < items ; i++ )
    {
        delete m_previousStudiesTree->takeTopLevelItem(0);
    }
    foreach ( QString key, m_infomationPerStudy.keys() )
    {
        delete m_infomationPerStudy.take( key );
    }

    m_previousStudiesManager->queryPreviousStudies( study );
}

void QPreviousStudiesWidget::createConnections()
{
    connect( m_previousStudiesManager, SIGNAL(queryPreviousStudiesFinished(QList<Study*>,QHash<QString,QString>)) , this , SLOT( insertStudiesToTree( QList<Study*>,QHash<QString,QString>) ) );
    connect(m_signalMapper, SIGNAL( mapped(const QString &) ), this, SLOT( retrieveAndLoadStudy(const QString &) ) );
    connect(m_queryScreen, SIGNAL( studyRetrieveStarted(QString) ), this, SLOT( studyRetrieveStarted(QString) ) );
    connect(m_queryScreen, SIGNAL( studyRetrieveFinished(QString) ), this, SLOT( studyRetrieveFinished(QString) ) );
    connect(m_queryScreen, SIGNAL( studyRetrieveFailed(QString) ), this, SLOT( studyRetrieveFailed(QString) ) );
}

void QPreviousStudiesWidget::initializeTree()
{

    // Inicialitzem la capçalera
    QStringList labels;
    labels << "" << "" <<  "" << tr("Name") << tr("Date") << tr("Hour") << tr("Modality") << tr("Description");
    m_previousStudiesTree->setHeaderLabels( labels );

    // Fem 8 columnes perquè la primera l'amagarem
    m_previousStudiesTree->setColumnCount( 8 );
    m_previousStudiesTree->setColumnHidden( 0 , true );
    m_previousStudiesTree->setAlternatingRowColors( true );
    m_previousStudiesTree->setUniformRowHeights( true );

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

void QPreviousStudiesWidget::insertStudyToTree(Study * study, QString pacsID )
{
    QTreeWidgetItem * item = new QTreeWidgetItem();

    //Afegim l'item al widget
    m_previousStudiesTree->addTopLevelItem( item );

    item->setFlags( Qt::ItemIsEnabled );

    item->setText( 3 , study->getParentPatient()->getFullName() );
    item->setText( 4 , study->getDateAsString() );
    item->setText( 5 , study->getTimeAsString() );
    item->setText( 6 , study->getModalitiesAsSingleString() );
    item->setText( 7 , study->getDescription() );

    QLabel * status = new QLabel();

    m_previousStudiesTree->setItemWidget( item , 1 , status );

    QIcon dowloadIcon( QString(":/images/view.png") );
    QPushButton * downloadButton = new QPushButton( dowloadIcon , QString("") );

    connect(downloadButton, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping( downloadButton , study->getInstanceUID() );

    m_previousStudiesTree->setItemWidget( item , 2 , downloadButton );

    // Guardem informació relacionada amb l'estudi per facilitar la feina
    StudyInfo *relatedStudyInfo = new StudyInfo;
    relatedStudyInfo->item = item;
    relatedStudyInfo->pacsID = pacsID;
    relatedStudyInfo->study = study;
    relatedStudyInfo->downloadButton = downloadButton;
    relatedStudyInfo->statusIcon = status;
    relatedStudyInfo->status = Initialized;
    m_infomationPerStudy.insert( study->getInstanceUID() , relatedStudyInfo );

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
    if ( studiesList.size() > 0 )
    {

        foreach( Study *study, orderStudiesByDateTime( studiesList , true ) )
        {
            insertStudyToTree( study , hashPacsIDOfStudyInstanceUID[ study->getInstanceUID() ]);
        }

        m_previousStudiesTree->setVisible( true );

        updateWidthTree();

    }
    else
    {
        m_noPreviousStudiesLabel->setVisible( true );
    }

    m_lookingForStudiesWidget->setVisible( false );

}

void QPreviousStudiesWidget::retrieveAndLoadStudy( const QString & studyInstanceUID )
{
    StudyInfo * studyInfo = m_infomationPerStudy[ studyInstanceUID ];

    studyInfo->downloadButton->setEnabled( false );

    m_queryScreen->retrieveStudy( QInputOutputPacsWidget::Load, studyInfo->pacsID , studyInfo->study );

    studyInfo->status = Pending;

    QMovie *statusAnimation = new QMovie();
    studyInfo->statusIcon->setMovie(statusAnimation);
    statusAnimation->setFileName(":/images/loader.gif");
    statusAnimation->start();

    this->increaseNumberOfDownladingStudies();
}

void QPreviousStudiesWidget::studyRetrieveStarted( QString studyInstanceUID )
{
    StudyInfo * studyInfo = m_infomationPerStudy[ studyInstanceUID ];

    //Comprovem que el signal capturat de QueryScreen sigui nostre
    if ( studyInfo != NULL )
    {
        if ( studyInfo->status == Pending )
            studyInfo->status = Downloading;
    }

}

void QPreviousStudiesWidget::studyRetrieveFinished( QString studyInstanceUID )
{
    StudyInfo * studyInfo = m_infomationPerStudy[ studyInstanceUID ];

    //Comprovem que el signal capturat de QueryScreen sigui nostre
    if ( studyInfo != NULL )
    {
        if ( studyInfo->status == Downloading )
        {
            studyInfo->status = Finished;
            studyInfo->statusIcon->setPixmap( QPixmap(":/images/button_ok.png") );

            this->decreaseNumberOfDownladingStudies();
        }
    }

}

void QPreviousStudiesWidget::studyRetrieveFailed( QString studyInstanceUID )
{
    StudyInfo * studyInfo = m_infomationPerStudy[ studyInstanceUID ];

    //Comprovem que el signal capturat de QueryScreen sigui nostre
    if ( studyInfo != NULL )
    {
        if ( studyInfo->status == Downloading )
        {
            studyInfo->status = Failed;
            studyInfo->statusIcon->setPixmap( QPixmap(":/images/cancel.png") );
            studyInfo->downloadButton->setEnabled( true );

            this->decreaseNumberOfDownladingStudies();
        }
    }
}

void QPreviousStudiesWidget::increaseNumberOfDownladingStudies()
{
    m_numberOfDownloadingStudies++;
    if ( m_numberOfDownloadingStudies == 1 )
    {
        emit downloadingStudies();
    }
}

void QPreviousStudiesWidget::decreaseNumberOfDownladingStudies()
{
    m_numberOfDownloadingStudies--;
    if ( m_numberOfDownloadingStudies == 0 )
    {
        emit studiesDownloaded();
    }
}

QList<Study*> QPreviousStudiesWidget::orderStudiesByDateTime( QList<Study*> & inputList , bool descendingOrder )
{
    QMultiMap<long,Study*> output;
    long key;

    foreach( Study *study, inputList )
    {
        if ( descendingOrder )
        {
            key = -study->getDateTime().toTime_t();
        }
        else
        {
            key = study->getDateTime().toTime_t();
        }

        output.insert( key , study );
    }

    return output.values();
}

}
