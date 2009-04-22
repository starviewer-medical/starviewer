/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQSTROKESEGMENTATION_H
#define UDGQSTROKESEGMENTATION_H

#include "ui_qstrokesegmentationextensionbase.h"

namespace udg {

// FWD declarations
class Volume;
class StrokeSegmentationMethod;
class ToolManager;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QStrokeSegmentationExtension : public QWidget , private ::Ui::QStrokeSegmentationExtensionBase {
Q_OBJECT
public:
    QStrokeSegmentationExtension( QWidget *parent = 0 );
    ~QStrokeSegmentationExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );

public slots:
    /// Executa l'algorisme de segmentació
    void applyMethod();

private:
    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    void initializeTools();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();

private slots:
    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition(double x, double y, double z);

    /// actualitza el valor llindar baix
    void setLowerValue( int x );

     /// actualitza el valor llindar alt
    void setUpperValue( int x );

    /// Canvia la opacitat de la màscara
    void setOpacity(int op);

    /// Calcula el volum de la màscara
    double calculateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentació
    Volume *m_maskVolume;

    /// Mètode de la segmentació
    StrokeSegmentationMethod *m_segMethod;

    bool m_isSeed, m_isMask;
    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;

    /// Accions
    QAction *m_rotateClockWiseAction;

    /// Tool manager
    ToolManager *m_toolManager;

    /// Posició de la seed
    double m_seedPosition[3];
};

} // end namespace udg

#endif
