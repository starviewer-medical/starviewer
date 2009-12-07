/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPREVIOUSSTUDIESWIDGET_H
#define UDGQPREVIOUSSTUDIESWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>

namespace udg {

class Study;
class PreviousStudiesManager;
class QueryScreen;

class QPreviousStudiesWidget : public QFrame
{
Q_OBJECT
public:
    QPreviousStudiesWidget(Study * inputStudy, QWidget * parent = 0 );
    ~QPreviousStudiesWidget();

signals:
    /// S'emet només quan no hi ha altres estudis ja descarregant-se.
    void downloadingStudies();

    /// S'emet quan s'han acabat de descarregar tots els estudis demanats.
    void studiesDownloaded();

private:
    /// Creació de connexions
    void createConnections();
    /// Inicialització del QTreeWidget on es visualitzaran els estudis previs
    void initializeTree();
    /// Inicialització del widget mostrat quan s'estan cercant estudis previs.
    void initializeLookinForStudiesWidget();
    /// Insereix un estudi a l'arbre i genera el contenidor associat a aquest estudi.
    void insertStudyToTree(Study * study , QString pacsID  );
    /// Actualitza l'amplada del QTreeWidget per aconseguir que l'scroll horitzontal no apareixi i tota la info sigui visible.
    void updateWidthTree();

    /// Metodes encarregats de controlar quan s'han d'emetre els signals \sa downloadingStudies i \sa studiesDownloaded;
    void increaseNumberOfDownladingStudies();
    void decreaseNumberOfDownladingStudies();

private slots:
    /// Insereix els estudis a l'arbre.
    void insertStudiesToTree(  QList<Study*> studiesList , QHash<QString, QString> hashPacsIDOfStudyInstanceUID );
    /// Invoca la descarrega i visualització de l'estudi identificat amb l'uid proporcionat.
    void viewStudy( const QString & studyInstanceUID  );

    /// Actualitzen l'estat de l'objecte i la interfície de l'estudi en qüestió.
    void studyRetrieveStarted( QString studyInstanceUID );
    void studyRetrieveFinished( QString studyInstanceUID );
    void studyRetrieveFailed( QString studyInstanceUID );

private:

    /// Enumeració creada per tal de saber si els signals que emet QueryScreen pertanyen a alguna de les peticions d'aquesta classe
    enum Status { Initialized, Pending, Downloading, Finished, Failed};

    /// Contenidor d'objectes associats a l'estudi que serveix per facilitar la intercomunicació
    struct StudyInfo
    {
        Study * study;
        QString pacsID;
        QTreeWidgetItem * item;
        Status status;
        QLabel * statusIcon;
        QPushButton * downloadButton;
    };

    /// Estructura que s'encarrega de guardar els contenidors associats a cada Study
    QHash< QString, StudyInfo * > m_infomationPerStudy;
    /// Widget utilitzat per mostrar la llista dels estudis previs
    QTreeWidget * m_previousStudiesTree;
    /// Widget que apareix quan s'està fent la consulta dels possibles estudis previs.
    QWidget * m_lookingForStudiesWidget;
    /// Objecte encarregat de cercar estudis previs
    PreviousStudiesManager * m_previousStudiesManager;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper * m_signalMapper;
    /// Objecte utilitzat per invocar la descàrrega d'estudis.
    QueryScreen * m_queryScreen;
    /// Ens permet saber els estudis que s'estan descarregant.
    int m_numberOfDownloadingStudies;
};

}
#endif // UDGQPREVIOUSSTUDIESWIDGET_H
