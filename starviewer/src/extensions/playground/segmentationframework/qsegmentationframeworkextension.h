/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQSEGMENTATIONFRAMEWORK_H
#define UDGQSEGMENTATIONFRAMEWORK_H

#include "ui_qsegmentationframeworkextensionbase.h"
#include "itkImage.h" 

class vtkSplineWidget;
class vtkImageData;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class ConnectedThreshold;
class IsolatedConnected;
class NeighborhoodConnected;
class ConfidenceConnected;
class VolumCalculator;
class ContournTool;

/**

	@author Grup de GrÀics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QSegmentationFrameworkExtension : public QWidget , private ::Ui::QSegmentationFrameworkExtensionBase {
Q_OBJECT
public:
    QSegmentationFrameworkExtension( QWidget *parent = 0 );

    ~QSegmentationFrameworkExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );
    
    

public slots:
    ///Cavni de Vistes
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

private:


    ///Vistes del volume
    enum ViewType{ Axial , Sagital , Coronal };
    void setView( ViewType view );
    

    ///Mètodes  
    ConnectedThreshold* m_method;
    IsolatedConnected* m_methodIC;
    NeighborhoodConnected* m_methodNC;
    ConfidenceConnected* m_methodCC;
   
    ///Càlcul de volums
    typedef itk::Image<signed int,3>     ImageType;
     VolumCalculator* m_volumCalculatorX;
     VolumCalculator* m_volumCalculatorY;
     VolumCalculator* m_volumCalculatorZ;

    /// Membres de classe
    float m_spacing[3];
    bool m_isSeed;
    bool m_outSeed;
    bool m_isMask;
    bool m_isCont;
    bool m_isFirst;
    int m_llescaactual;
    double m_novaArea;
    double m_nouVolum;
    int m_howManySeeds;
    bool m_isLeftButtonPressed;
    ViewType m_currentView;
    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    double m_originalSpline, m_newSpline;
   
     ///Tool Contorn
    ContournTool *m_contorn;
    vtkSplineWidget*  m_spline;
    vtkSplineWidget*  m_splineY;
    vtkSplineWidget*  m_splineZ;
    
    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_seedAction;
    QAction *m_voxelInformationAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_axialViewAction;
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
    QAction *m_contournAction;
    ToolsActionFactory *m_actionFactory;
    
    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;
    
    /// El volum principal
    Volume *m_mainVolume;
    
    /// El volum on hi guardem el resultat de la segmentaciÓ
    Volume *m_maskVolume;

    ///ACCIONS

    void createActions();
    void createToolBars();
    void createConnections();
    void Contorn(vtkImageData*);

private slots:

    void setSeedPosition();
    void applyCT();
    void applyIC();
    void applyNC();
    void applyCC();
    void clearAll();
    void clearSelected();
    void SelectOutSeed();
    void VolumeCalculator();
    void setAreaSlice(int);
    void opacityChanged(int);
    void calculateContorn();
    void setSplineLength(double);
    void setOriginalLength(double, int);
    void reset();
    

};

} // end namespace udg

#endif
