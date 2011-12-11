#ifndef DRAWERPOLYGONTESTHELPER
#define DRAWERPOLYGONTESTHELPER

#include <vtkSmartPointer.h>

class QString;

class vtkActor2D;
class vtkCellArray;
class vtkCoordinate;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkProperty2D;

namespace testing {

class DrawerPolygonTestHelper {

public:

    /// Crea un backgroundActor amb els valors per defecte de DrawerPolygon.
    static vtkSmartPointer<vtkActor2D> createDefaultBackgroundActor();
    /// Crea un foregroundActor amb els valors per defecte de DrawerPolygon.
    static vtkSmartPointer<vtkActor2D> createDefaultForegroundActor();
    /// Compara el mapper d'un DrawerPolygon amb el que hauria de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool compareMappers(vtkPolyDataMapper2D *actualMapper, vtkPolyDataMapper2D *expectedMapper, QString &firstDifference);
    /// Compara la coordenada de transferomació d'un DrawerPolygon amb la que hauria de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool compareTransformCoordinate(vtkCoordinate *actualCoordinate, vtkCoordinate *expectedCoordinate, QString &firstDifference);
    /// Compara el polyData d'un DrawerPolygon amb el que hauria de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool comparePolyData(vtkPolyData *actualPolyData, vtkPolyData *expectedPolyData, QString &firstDifference);
    /// Compara els punts d'un DrawerPolygon amb els que haurien de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool comparePoints(vtkPoints *actualPoints, vtkPoints *expectedPoints, QString &firstDifference);
    /// Compara les cel·les d'un DrawerPolygon amb les que haurien de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool compareCells(const QString &type, vtkCellArray *actualCells, vtkCellArray *expectedCells, QString &firstDifference);
    /// Compara la propietat d'un DrawerPolygon amb la que hauria de ser i retorna si són iguals.
    /// Si no ho són, la primera diferència és descrita a firstDifference.
    static bool compareProperties(vtkProperty2D *actualProperty, vtkProperty2D *expectedProperty, QString &firstDifference);

};

}

#endif // DRAWERPOLYGONTESTHELPER
