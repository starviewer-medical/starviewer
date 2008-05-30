/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQTHICKSLABWIDGET_H
#define UDGQTHICKSLABWIDGET_H

#include "ui_qthickslabwidgetbase.h"

namespace udg {

class Q2DViewer;

/**
Widget per controlar el Thick Slab d'un Q2DViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QThickSlabWidget : public QWidget, private Ui::QThickSlabWidgetBase
{
Q_OBJECT
public:
    QThickSlabWidget(QWidget *parent = 0);

    ~QThickSlabWidget();

    void setSlabThickness( int thickness );
    void setProjectionMode( int mode ); // TODO int o QString?

    /**
     * Enllacem aquest controlador amb un Q2DViewer. De moment només està previst que
     * s'enllaci amb un sol visor. Tenir més d'un visor linkat pot no tenir massa sentit ja que
     * els thickness seran diferents i tampoc es veu cap utilitat de tenir més d'un thickslab
     * "sincronitzat" alhora
     * @param viewer Visualitzador al que linkem el control de thick slab
     */
    void link( Q2DViewer *viewer );

signals:
    void thicknessChanged( int thickness );
    void projectionModeChanged( int mode ); // TODO enviar int o QString?

protected slots:
    /**
     * Aquest slot està connectat als canvis en el combo i segons el valor
     * escollit aplica sobre el visor el tipus de projecció adequat
     * @param comboItem
     */
    void applyProjectionMode( int comboItem );

    /**
     * Actualiza el valor màxim de thickness que es mostra en el combo
     * d'acord amb les llesques que té el volum del visor actual
     */
    void updateMaximumThickness();

    /**
     * Actualitza el valor de thickness que mostra el label. Anirà connectat amb l'slider
     * @param value Valor de thickness
     */
    void updateThicknessLabel(int value);

    /**
     * Reseteja el viewer associat. Útil per quan ens canvien l'input, per exemple
     */
    void reset();

    ///aplica el thick slab segons el valor de l'slider.
    void applyThickSlab();

    /// Mètodes per controlar les connexions de l'slider amb l'actualització de l'slab
    void turnOnDelayedUpdate();
    void turnOffDelayedUpdate();
    void onSliderReleased();

    /**
     * Deshabilita la projecció, per tant es deixa
     * d'aplicar qualsevol projecció. Es cridarà quan
     * es fagi un reset de la vista
     */
    void disableProjection();

protected:
    /// Visualitzador linkat al widget
    Q2DViewer *m_currentViewer;

};

}

#endif
