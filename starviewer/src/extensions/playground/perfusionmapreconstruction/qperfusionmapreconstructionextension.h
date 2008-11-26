/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPERFUSIONMAPRECONSTRUCTION_H
#define UDGQPERFUSIONMAPRECONSTRUCTION_H

#include "ui_qperfusionmapreconstructionextensionbase.h"

#include <itkImage.h>
#include <volume.h>

#include <QString>
#include <QVector>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkActor;
class vtkUnsignedCharArray;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class ToolManager;
class Drawer;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QPerfusionMapReconstructionExtension : public QWidget , private Ui::QPerfusionMapReconstructionExtensionBase {
Q_OBJECT
public:
    QPerfusionMapReconstructionExtension( QWidget *parent = 0 );
    ~QPerfusionMapReconstructionExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

private:
    typedef itk::Image<bool, 3> BoolImageType;
    typedef itk::Image<double, 3> DoubleImageType;
    typedef itk::Image<double, 4> DoubleTemporalImageType;
    static const double TE = 25.0;
    static const double TR = 1.5;
    static const double PI = 3.14159265358979323846;

    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();
    bool findProbableSeries( );

    void createColorMap( );
    void createColorMap2( );

private slots:

    void computePerfusionMap( );
    void computeDeltaR( );
    void computeMoments( );
    //Xapussa: posem directament el tipus dels voxels del Volume
    void computeMomentsVoxel( QVector<double> v, double &m0, double &m1, double &m2);
    void findAIF( );
    void fftAIF( );
    void getOmega( );
    void computePerfusion( );
    void deconvolve( QVector<double> tissue, QVector<double>& residuefunc);
    void changeMap( int value );

     /// gestiona els events del m_2DView
    void vsiEventHandler( unsigned long id );

    ///connecta amb el canvi de window level
    void createColorMap( double window, double level );
    void createColorMap(DoubleImageType::Pointer image, Q2DViewer* viewer);
    void createColorMap(Volume::ItkImageType::Pointer image, Q2DViewer* viewer);
    void getPerfusionColormapTable( vtkUnsignedCharArray * table );

    ///aplica un filtre al mapa
    void applyFilterMapImage( );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler();

    /// desactiva el booleà que ens diu si està el botó esquerra apretat
    void setLeftButtonOff( );

    /// Visualitza el menú que permet escollir la sèrie del DSC
    void contextMenuDSCRelease();
    void contextMenuSEPreRelease();
    void contextMenuSEPostRelease();
    void contextMenuEvent(QContextMenuEvent *event);
    void setSeries(Series *series);


private:
    enum VSIImage{ DSC , SEPre, SEPost };
    int m_imageVSItype;

    /// El volum principal
    Volume *m_mainVolume;
    Volume *m_DSCVolume;
    Volume *m_SEPreVolume;
    Volume *m_SEPostVolume;
    Volume* m_map0Volume;
    Volume* m_map1Volume;
    Volume* m_map2Volume;

    bool m_isLeftButtonPressed;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_mapMax, m_mapMin;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_voxelInformationAction;
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// Tool manager
    ToolManager *m_toolManager;

    BoolImageType::Pointer checkImage;

    DoubleTemporalImageType::Pointer deltaRImage;
    //DoubleImageType::Pointer deltaRImage;

    DoubleImageType::Pointer m0Image;
    DoubleImageType::Pointer m1Image;
    DoubleImageType::Pointer m2Image;
    DoubleImageType::Pointer cbfImage;
    DoubleImageType::Pointer cbvImage;
    DoubleImageType::Pointer mttImage;

    QVector<double> aif;
    double m_m0aif;
    QVector<double> fftaifreal;
    QVector<double> fftaifimag;
    QVector<double> omega;

    double reg_fact, reg_exp;

    Drawer* m_drawer;

};

} // end namespace udg

#endif
