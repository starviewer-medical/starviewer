/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTHICKSLABWIDGET_H
#define UDGTHICKSLABWIDGET_H

#include "ui_thickslabwidgetbase.h"

// FWD declarations
class QAction;
class vtkCommand;

namespace udg {

class Volume;

/**
Aquesta classe és la interfície genèrica d'un Q2DViewer juntament amb el seu slider i spinbox corresponent. Conté el mètode per posar un input així com els mètodes per modificar el valor de l'slider i l'spinbox.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ThickSlabWidget : public QFrame, private Ui::ThickSlabWidgetBase {
Q_OBJECT
public:
    ThickSlabWidget(QWidget *parent = 0);

    ~ThickSlabWidget();

    /// Marca el widget com a seleccionat
    void setSelected( bool option );

    /// Retorna el visualitzador
    Q2DViewer * getViewer();

    /// Retorna cert si el model té fases, fals altrament.
    bool hasPhases();

public slots:
    void setInput( Volume *input );
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

signals:
    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void selected(ThickSlabWidget * viewer);

protected:
    /// Sobrecàrrega de l'event que s'emet quan el mouse fa un clic dins l'àmbit del widget
    void mousePressEvent ( QMouseEvent * event );

private:

    /// Crea les connexions entre signals i slots
    void createConnections();

private slots:
    /// Quan el visualitzador s'ha seleccionat, emet el senyal amb aquest widget
    void emitSelectedViewer();

private:
    /// El volum principal
    Volume *m_mainVolume;
};

};

#endif
