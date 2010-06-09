/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCURVEDMPREXTENSION_H
#define UDGCURVEDMPREXTENSION_H

#include "ui_curvedmprextensionbase.h"

// Qt's
#include <QPointer>

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
class DrawerPolyline;

/**
    Aquesta extensió integra les eines per poder realitzar un MPR Curvilini
*/
class CurvedMPRExtension : public QWidget , private ::Ui::CurvedMPRExtensionBase {
Q_OBJECT
public:
    CurvedMPRExtension( QWidget *parent = 0 );
    ~CurvedMPRExtension();

public:
    /// Assigna el volum amb el que s'aplica l'MPR Curvilini
    void setInput( Volume *input );

private:
    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

    /// Inicia el procés de creació del reslicedVolume que caldrà visualitzar al segon viewer
    /// quan es modifica el número d'imatges que es vol que tingui la reconstrucció
    void updateResliceWithLastPointsPath();

    /// Crea un nou volum de reconstrucció i el mostra al visor corresponent
    void updateReslice( QPointer<DrawerPolyline> polyline, bool calculatePointsPath );

    /// Porta a terme l'MPR Curvilini retornant un nou volum al que se li ha assignat la reconstrucció calculada
    /// L'MPR Curvilini es calcula sobre el volum del visor principal
    Volume* doCurvedReslice( QPointer<DrawerPolyline> polyline, bool calculatePointsPath );

    /// Retorna una llista amb tots els punts que hi ha sobre la polyline indicada per
    /// l'usuari i que cal tenir en compte al fer la reconstrucció
    QList<double *> getPointsPath( QPointer<DrawerPolyline> polyline );

    /// Guarda punt calculat per la última línia dibuixada per l'usuari així
    /// si modifica el número d'imatges a reconstruir no cal tornar-los a calcular tots
    void addPointLastPath( double *point, int xIndex, int yIndex, int zIndex );

    /// Guarda la direcció de desplaçament del punt pertanyent a la polyline dibuixada per l'usuari
    /// Serà necessari per dibuixar línies de referències del gruix
    void addInfoPointLastPolyline( double *point, double *directorVector, int xIndex, int yIndex, int zIndex );

    // Guardem per cada segment quin és l'index de l'últim punt a desplaçar
    // i en quina direcció s'hauran de desplaçar els punts anteriors
    // En aquest cas aquesta direcció serà el vector director de la recta perpendicular al segment
    // creat entre el punt anterior i l'actual
    // vectorDirector(A,B) <-> vectorDirectorPerpendicular(-B,A)
    void addInfoPointsSegmentMovement( double *directorVector, int idxLastPointToMove, int xIndex, int yIndex, int zIndex );

    /// Es crida si ens han indicat que es vol reconstruir un volum amb més d'una imatge.
    /// Desplaça els punts de la línia indicada per l'usuari per equivaldre als punts del 
    /// pla de projecció que representarà la primera imatge del nou volum.  
    QList<double *> getLastPointsPathForFirstImage();

    /// Mostra les polylines que delimitaran el gruix indicat per l'usuari
    void showThicknessPolylines();

    /// S'inicialitzen i s'emplenen les dades VTK que han de formar el volum de la reconstrucció.
    void initAndFillImageDataVTK( const QList<double *> &pointsPath, vtkImageData *imageDataVTK );

private slots:
    /// Cada cop que es canvia l'input del viewer principal cal actualitzar el volum de treball
    void updateMainVolume( Volume *volume );

    /// Cada cop que l'usuari modifiqui el gruix indicat per fer el MIP, es modifica torna a fer
    /// la reconstrucció al visor corresponent, tenint en compte la última línia indicada per l'usuari
    void changeThicknessReconstruction();

    /// Inicia el procés de creació del reslicedVolume que caldrà visualitzar al segon viewer
    /// Es crida quan l'usuari indica la línia sobre la que caldrà projectar
    void updateReslice( QPointer<DrawerPolyline> polyline );

    /// Cada cop que es canvia el viewer seleccionat s'habiliten les tools en aquest visor
    /// i es deshabiliten de l'altre
    void changeSelectedViewer( Q2DViewerWidget *selectedViewer );
    
    /// Cada cop que es creï de nou l'eina de traçat de línia la connectarà amb 
    /// l'slot corresponent per obtenir la línia dibuixada
    void updateLinePathToolConnection( bool enabled );

    /// Mètodes per controlar les connexions de l'slider per indicar el número d'imatges
    /// amb la generació de la reconstrucció
    void turnOnDelayedUpdate();
    void turnOffDelayedUpdate();
    void onSliderReleased();

    /// Actualitza el valor de número d'imatges que mostra el label del slider
    void updateNumberOfImagesLabel( int value );

private:
    /// El volum al que se li practica l'MPR Curvilini
    Volume *m_mainVolume;

    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;

    /// Punts que formaven la última polyline dibuixada per l'usuari
    struct InfoPointLastPolyline
    {
        double *point;
        double *directorVector;
    };
    QList< InfoPointLastPolyline > m_lastPolylinePoints;

    /// Punts calculats que formen la última línia dibuixada per l'usuari
    QList<double *> m_lastPointsPath;

    /// Per guardar index últim punt de cada tram i el vector director que s'haurà de seguir per desplaçar els punts anteriors
    /// quan s'hagi de reconstruir més d'una imatge 
    struct InfoPointsSegmentMovement
    {
        int idxLastPointToMove;
        double *directorVector;
    };
    QList< InfoPointsSegmentMovement > m_pointsSegmentMovement;

    /// Número d'imatges que ha de contenir el volum de la reconstrucció
    int m_numImages;

    /// Distància entre la primera i la úlltima imatge que composaran la reconstrucció
    int m_maxThickness;

    /// Polilínia que ens marca el límit superior del thickness indicat per l'usuari.
    QPointer<DrawerPolyline> m_upPolylineThickness;

    /// Polilínia que ens marca el límit inferior del thickness indicat per l'usuari.
    QPointer<DrawerPolyline> m_downPolylineThickness;
};

};  //  end  namespace udg

#endif
