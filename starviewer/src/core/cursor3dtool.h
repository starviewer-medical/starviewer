/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCURSOR3DTOOL_H
#define UDGCURSOR3DTOOL_H

#include "tool.h"

// fwd declarations
class vtkMatrix4x4;
class vtkSphereSource;
class vtkActor;
class vtkPolyDataMapper;

namespace udg {

class Cursor3DToolData;
class Q2DViewer;
class ImagePlane;

/**
Implementació de la tool del cursor 3D.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Cursor3DTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , CALCULATING };
    
    Cursor3DTool( QViewer *viewer, QObject *parent = 0 );

    ~Cursor3DTool();

    /**
     * Re-implementa la funció del pare, afegint noves connexions
     * @param data
     */
    void setToolData(ToolData * data);

    void handleEvent( long unsigned eventID );

private slots:
    /// Actualitza les línies a projectar sobre la imatge segons les dades de la tool
    void updateProjectedPoint();

    /// Actualitza el frame of reference de les dades a partir del volum donat
    void updateFrameOfReference();

    /// Aquest slot es crida per actualitzar les dades que marquen quin és el pla de referència
    /// Es cridarà quan el viewer sigui actiu o quan el viewer actiu canvïi d'input
    void refreshReferenceViewerData();

    /// Actualitza la posició de la tool
    void updatePosition();

private:
    /// Projecta el pla de referència sobre el pla de localitzador
    void projectPoint();

    /// Inicialitza la posició de la tool 
    void initializePosition();

    /// Desactiva la tool 
    void removePosition();

private:
    /// Dades específiques de la tool
    Cursor3DToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ens guardem el frame of reference del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myFrameOfReferenceUID;

    /// Ens guardem el l'instance UID del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myInstanceUID;

    /// Esfera a dibuixar per representar el punt
    vtkSphereSource * m_sphere;

    ///Actor que dibuixa el punt
    vtkActor *m_sphereActor;

    ///Mapejador del punt
    vtkPolyDataMapper *m_sphereMapper;

    /// Controlar l'estat de la tool
    int m_state;
};

}

#endif
