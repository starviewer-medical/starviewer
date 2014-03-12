#ifndef UDGREFERENCELINESTOOL_H
#define UDGREFERENCELINESTOOL_H

#include "tool.h"

// Forward declarations
class vtkMatrix4x4;

namespace udg {

class ReferenceLinesToolData;
class Q2DViewer;
class ImagePlane;
class DrawerPolygon;
class DrawerLine;

/**
    Tool per aplicar reference lines
  */
class ReferenceLinesTool : public Tool {
Q_OBJECT
public:
    enum { SingleImage, AllImages };

    ReferenceLinesTool(QViewer *viewer, QObject *parent = 0);
    ~ReferenceLinesTool();

    /// Re-implementa la funció del pare, afegint noves connexions
    /// @param data
    void setToolData(ToolData *data);

    void handleEvent(long unsigned eventID)
    {
        eventID = eventID;
    }; // Cal implementar-lo, ja que a Tool és virtual pur TODO potser seria millor deixar-ho implementat buit en el pare?

private slots:
    /// Actualitza les línies a projectar sobre la imatge segons les dades de la tool
    void updateProjectionLines();

    /// Actualitza el frame of reference corresponent a aquest visor i actualitza el de referència (dades compartides) si aquest visor és l'actiu
    void updateFrameOfReference();

    /// Actualitza el pla d'imatge a projectar quan el viewer és l'actiu. Es crida cada cop que al viewer es canvia de llesca
    void updateReferenceImagePlanesToProject();

    /// Actualitza les dades segons l'input actual
    void updateDataForCurrentInput();
    
    /// Fa que aquest sigui el visor de referència i actualitza la informació corresponent
    /// Es cridarà quan el viewer sigui l'actiu
    void setAsReferenceViewer();

private:
    /// Inicialitza la tool i el que s'ha de mostrar al viewer segons l'estat en el que es troba
    void initialize();
    
    /// Crea les primitives que es faran servir per
    /// mostrar les interseccions projectades
    void createPrimitives();

    /// Projecta la intersecció del pla de referència amb el localitzador, sobre el pla de localitzador
    /// tambe li indiquem quina es la linia a modificar
    void projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane, int drawerLineOffset = 0);
    
    /// Calcula la intersecció entre el pla localitzador i el pla de referència donats. boundsList indica sobre quins plans implícits volem realitzar les 
    /// interseccions (superior, inferior, central). Si hi ha intersecció, actualitza les corresponents
    /// línies segons l'offset indicat. Si hi ha intersecció retorna true, fals altrement
    bool computeIntersectionAndUpdateProjectionLines(ImagePlane *localizerPlane, ImagePlane *referencePlane, QList<int> boundsList, int lineOffset);
    
    /// Projects the given intersection points and updates the corresponding lines according to lineOffset
    void updateProjectionLinesFromIntersections(double firstIntersectionPoint[3], double secondIntersectionPoint[3], int lineOffset);
    
    /// Update the projected line according to lineOffset with the given points
    void updateProjectedLine(int lineOffset, double firstPoint[3], double secondPoint[3]);

    /// Resets the projected line according to lineOffset so all points are the same and nothing can be viewed
    void resetProjectedLine(int lineOffset);
    
    /// Projecta directament el pla donat, sobre el pla actual que s'està visualitzant al viewer
    /// Aquest mètode es fa servir per "debug"
    void projectPlane(ImagePlane *planeToProject);

    /// Ens crea una DrawerLine, ja sigui de les principals o de background
    DrawerLine* createNewLine(bool isBackgroundLine = false);

    /// Comprova que el nombre de DrawerLines a les corresponents llistes
    /// sigui l'adequat i actua en consequencia
    void checkAvailableLines();

    /// Crea o elimina línies de la llista segons la quantitat necessària total indicada. 
    /// El paràmetre areBackgroundLines indica si les línies a crear han de ser estil background o no
    void createAndRemoveLines(int neededLines, QList<DrawerLine*> &linesList, bool areBackgroundLines);

    /// Comprova si els dos plans donats compleixen la restricció de l'angle que hi ha d'haver entre els plans de referència i localitzador
    /// per poder aplicar reference lines
    bool meetAngleConstraint(ImagePlane *firstPlane, ImagePlane *secondPlane);

private:
    /// Nom del grup del drawer on agruparem les primitives del reference lines
    static const QString ReferenceLinesDrawerGroup;

    /// Rang d'angles en el que s'han de trobar els plans que intersecten per poder aplicar el reference lines
    static const double MinimumAngleConstraint;
    static const double MaximumAngleConstraint;
    
    /// Dades específiques de la tool
    ReferenceLinesToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ens guardem el frame of reference del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myFrameOfReferenceUID;

    /// Polígon que projectem. De moment mantenim aquest poligon per questions de debug, tot i que no es mostrara per pantalla
    /// TODO quan tinguem la tool prou madura podrem prescindir d'aquest poligon
    DrawerPolygon *m_projectedReferencePlane;

    /// En aquestes llistes mantindrem totes les linies a projectar
    QList<DrawerLine*> m_projectedIntersectionLines;
    QList<DrawerLine*> m_backgroundProjectedIntersectionLines;

    /// Aquesta variable serveix per controlar si volem mostrar el gruix de la llesca o si pel contrari
    /// amb la llesca tal qual ens conformem
    /// aquesta podria ser una variable usada en un ToolConfiguration
    bool m_showPlaneThickness;

    /// Ens indica quins plans volem projectar. Tindra els valors enumerats definits per....
    /// aquesta podria ser una variable usada en un ToolConfiguration
    int m_planesToProject;
};

}

#endif
