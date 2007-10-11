/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQLANDMARKREGISTRATIONEXTENSION_H
#define UDGQLANDMARKREGISTRATIONEXTENSION_H

#include <QWidget>
#include "ui_qlandmarkregistrationextensionbase.h"

#include "landmarkregistrator.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkActor;

namespace udg {

class Volume;
class ToolsActionFactory;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QLandmarkRegistrationExtension : public QWidget , private ::Ui::QLandmarkRegistrationExtensionBase
{
Q_OBJECT
public:
    QLandmarkRegistrationExtension(QWidget *parent = 0);

    ~QLandmarkRegistrationExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );

    /// Obtenim la ToolBar d'eines de l'extensió \TODO 'pujar' al pare com a mètode com a Extensions?
    QToolBar *getToolsToolBar() const { return m_toolsToolBar; };

    /// Omple la ToolBar amb les eines de l'extensió \TODO 'pujar' al pare com a mètode com a Extensions?
    void populateToolBar( QToolBar *toolbar );

public slots:

    /// Li assigna el volum secundari. Aquest mètode només és de conveniència i és temporal
    void setSecondInput( Volume *input );

private:

    /// El volum principal
    Volume *m_firstVolume;

    /// El volum on hi guardem el segon volum a registrar
    Volume *m_secondVolume;

    /// El volum on hi guardem el resultat del registre (imatge transformada)
    Volume *m_registeredVolume;

    /// Mètode de registre
    LandmarkRegistrator::Pointer m_registrationMethod;
    typedef itk::Point<double,3> PointType;
    itk::VectorContainer<int, PointType>::Pointer m_seedSet1;
    itk::VectorContainer<int, PointType>::Pointer m_seedSet2;
    //LandmarkRegistrator::LandmarkSetType  m_seedSet1;
    //LandmarkRegistrator::LandmarkSetType  m_seedSet2;

    typedef LandmarkRegistrator::TransformType LandmarkRegTransformType;

    LandmarkRegTransformType::Pointer landmarkRegTransform;

    ///Listes de les llavors
    std::list<std::vector<double> > m_seedList1;
    std::list<std::vector<double> > m_seedList2;

    ///Listes de les llavors
    std::vector<vtkActor*>  m_seedActorVector1;
    std::vector<vtkActor*>  m_seedActorVector2;
    std::vector<vtkActor*>  m_seedActorVectorReg;
    std::vector<int>  m_seedSliceVector1;
    std::vector<int>  m_seedSliceVector2;
    std::vector<int>  m_seedSliceVectorReg;
    int m_seedLastActor1;
    int m_seedLastActor2;
    int m_seedLastActorReg;

    vtkActor* m_selectedActor;
    bool m_movingSeed;
    int m_idSelectedSeed;
    std::list<std::vector<double> >::iterator m_itListSelected;

    /// La ToolBar de les eines de l'extensió \TODO 'pujar' al pare com a membre com a Extensions? [hauria de ser protected]
    QToolBar *m_toolsToolBar;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_seedAction;
    QAction *m_voxelInformationAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_rotateCounterClockWiseAction;
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// crea les accions \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea la ToolBar d'eines i altres si n'hi ha \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createToolBars();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();

    void setLandmarks();


private slots:
     /// gestiona els events del m_2DView
    void landmarkEventHandler( unsigned long id );
     /// gestiona els events del m_2DView_2
    void landmarkEventHandler2( unsigned long id );

    /// gestiona els events del moviment de ratolí
    void mouseMoveEventHandler( int idVolume );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler( int idVolume );

    /// gestiona els events del botó dret
    void rightButtonPressEventHandler( int idVolume );
    void rightButtonReleaseEventHandler( int idVolume );

    /// Aplica el mètode de registre
    void applyMethod();

    ///Posa una nova llavor
    void setNewSeedPosition( int idVolume );

    ///Reajustem la finestra per tal de veure un volum o l'altre
    void viewFirstVolume( );
    void viewSecondVolume( );

    ///Canvia transparència Overlay
    void setOpacity( int op );

    ///Activa les llavors
    void seed1Activated( int row, int aux);
     void seed2Activated( int row, int aux);

    ///Visualitza les llavors de la llesca
    void sliceChanged1( int s );
    void sliceChanged2( int s );

    ///Desa la transformada resultant del fitxer de registre
    void saveTransform(  );

    ///Carrega una transformada a partir del fitxer de registre
    void loadTransform(  );

    ///Reinicialitza l'extensió
    void restore(  );
};

}

#endif
