/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREFERENCELINESTOOL_H
#define UDGREFERENCELINESTOOL_H

#include "tool.h"

// fwd declarations
class vtkMatrix4x4;

namespace udg {

class ReferenceLinesToolData;
class Q2DViewer;
class ImagePlane;
class DrawerPolygon;
class DrawerLine;

/**
Tool per aplicar reference lines

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ReferenceLinesTool : public Tool
{
Q_OBJECT
public:
    ReferenceLinesTool( QViewer *viewer, QObject *parent = 0 );

    ~ReferenceLinesTool();

    /**
     * Re-implementa la funció del pare, afegint noves connexions
     * @param data
     */
    void setToolData(ToolData * data);

    void handleEvent( long unsigned eventID ){}; // cal implementar-lo, ja que a Tool és virtual pur TODO potser seria millor deixar-ho implementat buit en el pare?

private slots:
    /// Actualitza les línies a projectar sobre la imatge segons les dades de la tool
    void updateProjectionLines();

    /// Actualitza el frame of reference de les dades a partir del volum donat
    void updateFrameOfReference();

    /// Actualitza el pla d'imatge a projectar. Es crida cada cop que al viewer es canvia de llesca
    void updateImagePlane();

    /// Aquest slot es crida per actualitzar les dades que marquen quin és el pla de referència
    /// Es cridarà quan el viewer sigui actiu o quan el viewer actiu canvïi d'input
    void refreshReferenceViewerData();

private:
    /// Crea les primitives que es faran servir per
    /// mostrar les interseccions projectades
    void createPrimitives();

    /// Projecta la intersecció del pla de referència amb el localitzador, sobre el pla de localitzador
    void projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane);

    /// Projecta directament el pla donat, sobre el pla actual que s'està visualitzant al viewer
    /// Aquest mètode es fa servir per "debug"
    void projectPlane(ImagePlane *planeToProject);

    /**
     * Donats quatre punts d'un pla (referència) i el pla localitzador, ens dóna els punts d'intersecció
     * @param tlhc top left hand corner ( origen )
     * @param trhc top right hand corner
     * @param brhc bottom right hand corner
     * @param blhc bottom left hand corner
     * @param localizerPlane pla localitzador que volem fer intersectar
     * @param firstIntersectionPoint[] primer punt d'intersecció trobat ( si n'hi ha )
     * @param secondIntersectionPoint[] segon punt d'intersecció trobat ( si n'hi ha )
     * @return el nombre d'interseccions trobades
     */
    int getIntersections( QVector<double> tlhc, QVector<double> trhc, QVector<double> brhc, QVector<double> blhc, ImagePlane *localizerPlane, double firstIntersectionPoint[3], double secondIntersectionPoint[3] );

private:
    /// Dades específiques de la tool
    ReferenceLinesToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ens guardem el frame of reference del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myFrameOfReferenceUID;

    /// Polígon que projectem. De moment mantenim aquest poligon per questions de debug, tot i que no es mostrara per pantalla
    /// TODO quan tinguem la tool prou madura podrem prescindir d'aquest poligon
    DrawerPolygon *m_projectedReferencePlane;

    /// Intersecció que projectem
    DrawerLine *m_lowerProjectedIntersection, *m_backgroundLowerProjectedIntersection, *m_upperProjectedIntersection, *m_backgroundUpperProjectedIntersection;
};

}

#endif
