/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTDICOMDIRWIDGET_H
#define UDGQINPUTOUTPUTDICOMDIRWIDGET_H

#include "ui_qinputoutputdicomdirwidgetbase.h"

#include <QMenu>

#include "dicomdirimporter.h"
#include "dicomdirreader.h"

class QString;

namespace udg {

/** Widget en el que controla les operacions d'entrada/sortida d'un dicomdir
*/

class DicomMask;
class Patient;
class StatsWatcher;

class QInputOutputDicomdirWidget : public QWidget, private Ui::QInputOutputDicomdirWidgetBase
{
Q_OBJECT

public:
    /// Constructor de la classe
    QInputOutputDicomdirWidget(QWidget *parent = 0);

    ///Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask);

    ~QInputOutputDicomdirWidget();

signals:

    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewPatients(QList<Patient*>);

    ///Signal que s'emet per indica que un estudi ha estat descarregat
    void studyRetrieved();

public slots:

    ///Obre un dicomdir
    void openDicomdir();

    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view();

private:

    QMenu m_contextMenuQStudyTreeWidget;

    DICOMDIRReader m_readDicomdir; // conté la informació del dicomdir obert en aquests instants
    StatsWatcher *m_statsWatcher;

    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    ///Carrega l'amplada de les columnes de QStudyTreeView guardada al QSettings
    void setQStudyTreeWidgetColumnsWidth();

    ///Guarda al QSettings l'amplada de les columnes del QStudyTreeView, perquè quan es torni a carregar el widget es motri l'amplada de les columnes igual a quan es va tancar
    void saveQStudyTreeWidgetColumnsWidth();

    ///Mostrar l'error que s'ha produït al importar el dicomdir
    void showDICOMDIRImporterError(QString studyInstanceUID, DICOMDIRImporter::DICOMDIRImporterError error);

private slots:

    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);


    ///Importa cap a la base de dades locals els estudis seleccionats
    void retrieveSelectedStudies();
};

};// end namespace udg

#endif
