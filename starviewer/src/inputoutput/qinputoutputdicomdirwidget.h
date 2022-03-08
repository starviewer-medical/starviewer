/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/


#ifndef UDGQINPUTOUTPUTDICOMDIRWIDGET_H
#define UDGQINPUTOUTPUTDICOMDIRWIDGET_H

#include "ui_qinputoutputdicomdirwidgetbase.h"

#include "dicomdirimporter.h"
#include "dicomdirreader.h"

#include <QMenu>

// Fordward declarations
class QString;

namespace udg {

// Fordward declarations
class DicomMask;
class Patient;
class StatsWatcher;

/**
    Widget en el que controla les operacions d'entrada/sortida d'un dicomdir
  */
class QInputOutputDicomdirWidget : public QWidget, private Ui::QInputOutputDicomdirWidgetBase {
Q_OBJECT

public:
    /// Constructor de la classe
    QInputOutputDicomdirWidget(QWidget *parent = 0);
    ~QInputOutputDicomdirWidget();

    /// Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask);

    /// Neteja els resultats de la última cerca
    void clear();

public slots:
    /// Mostra un diàleg per escollir un arxiu de DICOMDIR i carregar-lo.
    /// Retorna fals si no s'ha escollit cap arxiu o l'arxiu escollit no és vàlid, cert altrament.
    bool openDicomdir();

signals:
    /// Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    /// Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewPatients(QList<Patient*>);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    /// Mostrar l'error que s'ha produït al importar el dicomdir
    void showDICOMDIRImporterError(QString studyInstanceUID, DICOMDIRImporter::DICOMDIRImporterError error);

private slots:
    /// Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void requestedSeriesOfStudy(Study *studyRequestedSeries);

    /// Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void requestedImagesOfSeries(Series *seriesRequestedImages);

    /// Importa cap a la base de dades locals els estudis seleccionats
    void retrieveSelectedStudies();

    /// Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view();

private:
    QMenu m_contextMenuQStudyTreeWidget;

    /// Conté la informació del dicomdir obert en aquests instants
    DICOMDIRReader m_readDicomdir;
    StatsWatcher *m_statsWatcher;
};

};// end namespace udg

#endif
