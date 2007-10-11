/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQLOGVIEWER_H
#define UDGQLOGVIEWER_H

#include "ui_qlogviewerbase.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QLogViewer : public QDialog, private Ui::QLogViewerBase
{
Q_OBJECT
public:
    QLogViewer(QWidget *parent = 0);

    ~QLogViewer();

public slots:
    /// Obre un diàleg per guardar el fitxer de log en una altre ubicació
    void saveLogFileAs();

    /// Actualitza les dades del fitxer de log que mostra
    void updateData();

private slots:
    /// Crea les connexions entre signals i slots
    void createConnections();

};

}

#endif
