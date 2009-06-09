/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTDICOMDIRWIDGET_H
#define UDGQINPUTOUTPUTDICOMDIRWIDGET_H

#include "ui_qinputoutputdicomdirwidgetbase.h"

#include "dicomdirimporter.h"
#include "dicomdirreader.h"

#include <QMenu>

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class DicomMask;
class Patient;
class StatsWatcher;

/** 
 * Widget en el que controla les operacions d'entrada/sortida d'un dicomdir
 */
class QInputOutputDicomdirWidget : public QWidget, private Ui::QInputOutputDicomdirWidgetBase
{
Q_OBJECT

public:
    /// Constructor de la classe
    QInputOutputDicomdirWidget(QWidget *parent = 0);
    ~QInputOutputDicomdirWidget();

    ///Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask);

    ///Neteja els resultats de la última cerca
    void clear();

public slots:
    ///Obre un dicomdir
    void openDicomdir();

signals:
    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewPatients(QList<Patient*>);

    ///Signal que s'emet per indica que un estudi ha estat descarregat
    void studyRetrieved(QString studyInstanceUID);

private:
    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    ///Mostrar l'error que s'ha produït al importar el dicomdir
    void showDICOMDIRImporterError(QString studyInstanceUID, DICOMDIRImporter::DICOMDIRImporterError error);

private slots:
    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);

    ///Importa cap a la base de dades locals els estudis seleccionats
    void retrieveSelectedStudies();

    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view();

private:
    QMenu m_contextMenuQStudyTreeWidget;

    DICOMDIRReader m_readDicomdir; // conté la informació del dicomdir obert en aquests instants
    StatsWatcher *m_statsWatcher;
};

};// end namespace udg

#endif
