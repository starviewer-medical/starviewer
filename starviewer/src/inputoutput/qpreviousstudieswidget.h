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
class Patient;

class QPreviousStudiesWidget : public QFrame {
Q_OBJECT
public:
    QPreviousStudiesWidget(QWidget *parent = 0);
    ~QPreviousStudiesWidget();

    /// Mètode per activar la cerca d'estudis previs. Es mostraran al widget els estudis del pacient anteriors a la data
    /// de l'estudi proporcionat. Es farà la consulta als PACS que estan seleccionats a la llista de nodes de la QueryScreen
    void searchPreviousStudiesOf(Study *study);

    /// Mètode per buscar els estudis relacionat amb el pacient.
    void searchStudiesOf(Patient *patient);

    /// Actualitza la llista marcant aquells estudis que ja estan en memòria perquè sigui fàcil identificar-los i no es puguin tornar a descarregar.
    void updateList();

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
    void initializeLookingForStudiesWidget();
    /// Insereix un estudi a l'arbre i genera el contenidor associat a aquest estudi.
    void insertStudyToTree(Study *study);
    /// Actualitza l'amplada del QTreeWidget per aconseguir que l'scroll horitzontal no apareixi i tota la info sigui visible.
    void updateWidthTree();

    /// Metodes encarregats de controlar quan s'han d'emetre els signals \sa downloadingStudies i \sa studiesDownloaded;
    void increaseNumberOfDownladingStudies();
    void decreaseNumberOfDownladingStudies();

    void initializeSearch();

private slots:
    /// Insereix els estudis a l'arbre.
    void insertStudiesToTree(QList<Study*> studiesList, QHash<QString, QString> hashPacsIDOfStudyInstanceUID);

    /// Invoca la descàrrega i càrrega de l'estudi identificat amb l'uid proporcionat.
    void retrieveAndLoadStudy(const QString &studyInstanceUID);

    /// Actualitzen l'estat de l'objecte i la interfície de l'estudi en qüestió.
    void studyRetrieveStarted(QString studyInstanceUID);
    void studyRetrieveFinished(QString studyInstanceUID);
    void studyRetrieveFailed(QString studyInstanceUID);

private:
    /// Enumeració creada per tal de saber si els signals que emet QueryScreen pertanyen a alguna de les peticions d'aquesta classe
    enum Status { Initialized, Pending, Downloading, Finished, Failed };
    enum Columns { DownloadingStatus = 0, DownloadButton = 1, Modality = 2, Description = 3, Date = 4, Name = 5 };

    /// Contenidor d'objectes associats a l'estudi que serveix per facilitar la intercomunicació
    struct StudyInfo
    {
        Study *study;
        QTreeWidgetItem *item;
        Status status;
        QLabel *statusIcon;
        QPushButton *downloadButton;
    };

    /// Estructura que s'encarrega de guardar els contenidors associats a cada Study
    QHash<QString, StudyInfo*> m_infomationPerStudy;
    /// Widget utilitzat per mostrar la llista dels estudis previs
    QTreeWidget *m_previousStudiesTree;
    /// Widget que apareix quan s'està fent la consulta dels possibles estudis previs.
    QWidget *m_lookingForStudiesWidget;
    /// Label per mostrar que no hi ha estudis previs.
    QLabel *m_noPreviousStudiesLabel;
    /// Objecte encarregat de cercar estudis previs
    PreviousStudiesManager *m_previousStudiesManager;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper *m_signalMapper;
    /// Objecte utilitzat per invocar la descàrrega d'estudis.
    QueryScreen *m_queryScreen;
    /// Ens permet saber els estudis que s'estan descarregant.
    int m_numberOfDownloadingStudies;
    /// Pacient associat a la última cerca feta.
    Patient *m_patient;
};

}
#endif // UDGQPREVIOUSSTUDIESWIDGET_H
