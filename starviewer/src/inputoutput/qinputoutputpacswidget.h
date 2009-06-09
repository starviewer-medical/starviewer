/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTPACSWIDGET_H
#define UDGQINPUTOUTPUCPACSWIDGET_H

#include "ui_qinputoutputpacswidgetbase.h"

#include <QMenu>
#include <QHash>

#include "qexecuteoperationthread.h"

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class DicomMask;
class Patient;
class StatsWatcher;
class Status;
class PacsParameters;
class MultipleQueryStudy;
class PacsServer;
class Study;
class QOperationStateScreen;
class ProcessImageSingleton;

/** 
 * Widget en el que controla les operacions d'entrada/sortida del PACS
 */
class QInputOutputPacsWidget : public QWidget, private Ui::QInputOutputPacsWidgetBase
{
Q_OBJECT
public:
    /// Constructor de la classe
    QInputOutputPacsWidget(QWidget *parent = 0);
    ~QInputOutputPacsWidget();

    ///Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask, QList<PacsParameters> pacsToQuery);

    //TODO: per implementar
    void storeStudiesToPacs();

    ///Neteja els resultats de la última cerca
    void clear();

    ///Especifica quina interfície s'ha d'utilitzar per comunicar l'estat de les operacions
    void setQOperationStateScreen(QOperationStateScreen *qoperationStateScreen);

    /** Descarrega una estudi del pacs
     * @param indica si l'estudi s'ha de visualitzar
     * @param pacsIdToRetrieve indica l'id del Pacs del qual s'ha de descarregar l'estudi
     * @param maskStudyToRetrieve la màscara dels objectes a descarregar
     * @param studyToRetrieve L'estudi al qual pertanyen els objectes ad escarregar
     */
    void retrieve(bool view, QString pacsIdToRetrieve, DicomMask mask, Study *studyToRetrieve);

signals:
    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewRetrievedStudy(QString studyInstanceUID);

    ///Signal que s'emet per indica que un estudi ha estat descarregat
    void studyRetrieved(QString studyInstanceUID);

    ///Signal que s'emet per indicar que hi hagut algun canvi en l'estat de les operacions, (s'ha cancel·lat, ha acabat, s'ha creat un de nova, ha fallat)
    void operationStateChange();

    ///Indica que un estudi serà esborrat de la base de dades local, això es produeix perquè al intentar descarregar un estudi es comprovar que no hi ha suficient espai al disc i s'esborren els estudis que fa més temps que no han estat visualitzats
    void studyWillBeDeletedFromDatabase(QString studyInstanceUID);

private:
    /// Ha de ser global, sino l'objecte es destrueix i QT no té temps d'atendre els signals dels threads
    MultipleQueryStudy *m_multipleQueryStudy;

    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    ///Fa la consulta a diversos PACS
    //TODO aquest mètode quan s'hagi fet el refactoring de pacsparameters hauria de desapareixer no tindrà sentit
    Status queryMultiplePacs(DicomMask searchMask, QList<PacsParameters> listPacsToQuery, MultipleQueryStudy *multipleQueryStudy);

    /**Comprova que els paràmetres per la cerca siguin correctes, que no es tractir d'un consulta pesada i que ens hagin seleccionat
     * algun PACS per consultar 
     */
    bool AreValidQueryParameters(DicomMask *maskToQuery, QList<PacsParameters> pacsToQuery);

    ///Retorna l'ID del pacs al que pertany l'estudi passat per paràmetre, només té en compte els estudis que s'han consultat a la última query
    QString getPacsIDFromQueriedStudies(QString studyInstanceUID);

    /** Donat un AETitle busca les dades del PACS a la configuració i prepara un objecte PACSERVER, per poder
     * connectar al PACS
     */
    //TODO Aquest mètode ha de desapareixer quan s'hagi fet refactoring de PacsParameters i s'hagin tret els paràmetres que no li són propis
    PacsServer getPacsServerByPacsID(QString pacsID);

    ///Construeix la màscara de cerca per cercar les sèries d'un estudi
    DicomMask buildSeriesDicomMask(QString studyInstanceUID);

    ///Construeix la màscara de cerca per cercar les imatges d'una sèrie
    DicomMask buildImageDicomMask(QString studyInstanceUID, QString seriesInstanceUID);

private slots:
    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);

    ///Importa cap a la base de dades locals els estudis seleccionats
    void retrieveSelectedStudies(bool view = false);

    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view();

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al connectar amb el pacs
     * @param pacsID ID del pacs a la base de ades local
     */
    void errorConnectingPacs(QString pacsID);

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al fer una query d'estudis amb el pacs
     * @param id del PACS
     */
    void errorQueringStudiesPacs(QString PacsID);

    ///Ens Mostra un missatge indicant l'error produït a la QExecuteOperationThread, i com es pot solucionar
    void showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error);

private:
    QMenu m_contextMenuQStudyTreeWidget;
    QHash<QString, QString> m_hashPacsIDOfStudyInstanceUID;
    QExecuteOperationThread m_qexecuteOperationThread;
    QOperationStateScreen *m_qoperationStateScreen;
    ProcessImageSingleton *m_processImageSingleton;

    StatsWatcher *m_statsWatcher;
};

};// end namespace udg

#endif
