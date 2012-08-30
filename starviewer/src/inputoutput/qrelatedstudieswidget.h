#ifndef UDGQRELATEDSTUDIESWIDGET_H
#define UDGQRELATEDSTUDIESWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include <QHash>

class QTreeWidgetItem;

namespace udg {

class Study;
class RelatedStudiesManager;
class QueryScreen;
class Patient;
class QTreeWidgetWithSeparatorLine;

class QRelatedStudiesWidget : public QFrame {
Q_OBJECT
public:
    QRelatedStudiesWidget(QWidget *parent = 0);
    ~QRelatedStudiesWidget();

    /// Mètode per activar la cerca d'estudis relacionats. Es mostraran al widget els estudis del pacient anteriors a la data
    /// de l'estudi proporcionat. Es farà la consulta als PACS que estan seleccionats a la llista de nodes de la QueryScreen
    void searchPreviousStudiesOf(Study *study);

    /// Mètode per buscar els estudis relacionat amb el pacient.
    void searchStudiesOf(Patient *patient);

    /// Actualitza la llista marcant aquells estudis que ja estan en memòria perquè sigui fàcil identificar-los i no es puguin tornar a descarregar.
    void updateList();

public slots:
    virtual void setVisible(bool visible);

signals:
    /// S'emet només quan no hi ha altres estudis ja descarregant-se.
    void downloadingStudies();

    /// S'emet quan s'han acabat de descarregar tots els estudis demanats.
    void studiesDownloaded();

private:
    /// Creació de connexions
    void createConnections();
    /// Inicialització del QTreeWidget on es visualitzaran els estudis relacionats
    void initializeTree();
    /// Inicialització del widget mostrat quan s'estan cercant estudis relacionats.
    void initializeLookingForStudiesWidget();
    /// Insereix un estudi a l'arbre i genera el contenidor associat a aquest estudi.
    void insertStudyToTree(Study *study);
    /// Actualitza l'amplada del QTreeWidget per aconseguir que l'scroll horitzontal no apareixi i tota la info sigui visible.
    void updateWidgetWidth();
    /// Actualitza l'alçada del QTreeWidget per aconseguir que es vegi el màxim de contingut possible.
    /// Nota: Perquè funcioni correctament el QRelatedStudiesWidget ha de ser visible.
    void updateWidgetHeight();

    /// Metodes encarregats de controlar quan s'han d'emetre els signals \sa downloadingStudies i \sa studiesDownloaded;
    void increaseNumberOfDownladingStudies();
    void decreaseNumberOfDownladingStudies();

    void initializeSearch();

    /// Ressalta el QTreeWidgetItem posant el text que conté a negreta
    void highlightQTreeWidgetItem(QTreeWidgetItem *item);

    /// Treu del llistat de modalitats les modalitats que no són d'imatge
    QStringList removeNonImageModalities(const QStringList &studiesModalities);
    
    /// Indica si s'ha de ressaltar l'estudi en el QTreeWidget
    bool hasToHighlightStudy(Study *study);

private slots:
    /// Insereix els estudis a l'arbre.
    void insertStudiesToTree(QList<Study*> studiesList);

    /// Invoca la descàrrega i càrrega de l'estudi identificat amb l'uid proporcionat.
    void retrieveAndLoadStudy(const QString &studyInstanceUID);

    /// Actualitzen l'estat de l'objecte i la interfície de l'estudi en qüestió.
    void studyRetrieveStarted(QString studyInstanceUID);
    void studyRetrieveFinished(QString studyInstanceUID);
    void studyRetrieveFailed(QString studyInstanceUID);
    void studyRetrieveCancelled(QString studyInstanceUID);

private:
    /// Enumeració creada per tal de saber si els signals que emet QueryScreen pertanyen a alguna de les peticions d'aquesta classe
    enum Status { Initialized, Pending, Downloading, Finished, Failed, Cancelled };
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
    /// Widget utilitzat per mostrar la llista dels estudis relacionats
    QTreeWidgetWithSeparatorLine *m_relatedStudiesTree;
    /// Widget que apareix quan s'està fent la consulta dels possibles estudis relacionats.
    QWidget *m_lookingForStudiesWidget;
    /// Label per mostrar que no hi ha estudis relacionats.
    QLabel *m_noRelatedStudiesLabel;
    /// Objecte encarregat de cercar estudis relacionats
    RelatedStudiesManager *m_relatedStudiesManager;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper *m_signalMapper;
    /// Objecte utilitzat per invocar la descàrrega d'estudis.
    QueryScreen *m_queryScreen;
    /// Ens permet saber els estudis que s'estan descarregant.
    int m_numberOfDownloadingStudies;
    /// Pacient associat a la última cerca feta.
    Patient *m_patient;

    ///Llistat amb les modalitats dels estudis que hem de ressalar
    QStringList m_modalitiesOfStudiesToHighlight;
};

}
#endif // UDGQRELATEDSTUDIESWIDGET_H
