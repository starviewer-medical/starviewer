/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREFERENCELINESTOOL_H
#define UDGREFERENCELINESTOOL_H

#include "tool.h"

namespace udg {

class ReferenceLinesToolData;
class Q2DViewer;
class Volume;
class ImagePlane;
class DrawerPolygon;

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
    void updateFrameOfReference(Volume *volume);

    /// Actualitza el pla d'imatge a projectar. Es crida cada cop que al viewer es canvia de llesca
    void updateImagePlane();

    /// Aquest slot es crida per actualitzar les dades quan tenim un nou input
    void refreshInput();

private:
    /// Projecta el pla de referència sobre el pla de localitzador
    void projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane);

private:
    /// Dades específiques de la tool
    ReferenceLinesToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ens guardem el frame of reference del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myFrameOfReferenceUID;

    /// Polígon que projectem
    DrawerPolygon *m_projectedReferencePlane;
};

}

#endif
