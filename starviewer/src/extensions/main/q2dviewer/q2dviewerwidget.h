/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERWIDGET_H
#define UDGQ2DVIEWERWIDGET_H

#include "ui_q2dviewerwidgetbase.h"

class QAction;
class vtkCommand;

namespace udg {

class Volume;

/**
Aquesta classe és la interfície genèrica d'un Q2DViewer juntament amb el seu slider i spinbox corresponent. Conté el mètode per posar un input així com els mètodes per modificar el valor de l'slider i l'spinbox.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerWidget : public QFrame, private Ui::Q2DViewerWidgetBase {
Q_OBJECT
public:
    Q2DViewerWidget(QWidget *parent = 0);

    ~Q2DViewerWidget();

    void setSelected( bool option );

    Q2DViewer * getViewer();

protected:
    /// Sobrecàrrega de l'event que s'emet quan el mouse fa un clic dins l'àmbit del widget
    void mousePressEvent ( QMouseEvent * event );

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// Crea les connexions entre signals i slots
    void createConnections();

signals:
    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void selected(Q2DViewerWidget * viewer);

public slots:
    void setInput( Volume *input );
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

private slots:
    /// Quan el visualitzador s'ha seleccionat, emet el senyal amb aquest widget
    void emitSelectedViewer();
};

};

#endif
