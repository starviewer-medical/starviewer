#include "autotest.h"
#include "drawerpolygon.h"

#include "drawerpolygontesthelper.h"
#include "glutessellator.h"

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPropAssembly.h>
#include <vtkPropCollection.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>

using namespace udg;
using namespace testing;

class test_DrawerPolygon : public QObject {
Q_OBJECT

private slots:
    void getAsVtkProp_ShouldReturnPropLikeExpected_data();
    void getAsVtkProp_ShouldReturnPropLikeExpected();

    void isModified_ShouldReturnExpectedValue_data();
    void isModified_ShouldReturnExpectedValue();

    void get2DPlaneIndices_ReturnsExpectedValues_data();
    void get2DPlaneIndices_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DrawerPolygon*)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkPropAssembly>)
Q_DECLARE_METATYPE(double*)

void test_DrawerPolygon::getAsVtkProp_ShouldReturnPropLikeExpected_data()
{
    QTest::addColumn<DrawerPolygon*>("drawerPolygon");
    QTest::addColumn< vtkSmartPointer<vtkPropAssembly> >("expectedProp");

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("0 points") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(1);
        points->InsertPoint(0, 0.0, 0.0, 0.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(1);
        cellArray->InsertCellPoint(0);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("1 point") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->addVertex(Vector3(1.0, 1.0, 1.0));
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(3);
        points->InsertPoint(0, 0.0, 0.0, 0.0);
        points->InsertPoint(1, 1.0, 1.0, 1.0);
        points->InsertPoint(2, 0.0, 0.0, 0.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(3);
        cellArray->InsertCellPoint(0);
        cellArray->InsertCellPoint(1);
        cellArray->InsertCellPoint(2);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("2 points") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->addVertex(Vector3(1.0, 1.0, 1.0));
        drawerPolygon->addVertex(Vector3(4.0, 3.0, 3.0));
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(4);
        points->InsertPoint(0, 0.0, 0.0, 0.0);
        points->InsertPoint(1, 1.0, 1.0, 1.0);
        points->InsertPoint(2, 4.0, 3.0, 3.0);
        points->InsertPoint(3, 0.0, 0.0, 0.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(4);
        cellArray->InsertCellPoint(0);
        cellArray->InsertCellPoint(1);
        cellArray->InsertCellPoint(2);
        cellArray->InsertCellPoint(3);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("triangle (3 points)") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->addVertex(Vector3(1.0, 1.0, 1.0));
        drawerPolygon->addVertex(Vector3(4.0, 3.0, 3.0));
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(4);
        points->InsertPoint(0, 0.0, 0.0, 0.0);
        points->InsertPoint(1, 1.0, 1.0, 1.0);
        points->InsertPoint(2, 4.0, 3.0, 3.0);
        points->InsertPoint(3, 0.0, 0.0, 0.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(4);
        cellArray->InsertCellPoint(0);
        cellArray->InsertCellPoint(1);
        cellArray->InsertCellPoint(2);
        cellArray->InsertCellPoint(3);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("triangle (4 points, first == last)") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->addVertex(Vector3(1.0, 1.0, 0.0));
        drawerPolygon->addVertex(Vector3(3.0, 4.0, 0.0));
        drawerPolygon->addVertex(Vector3(4.0, 3.0, 0.0));
        drawerPolygon->setFilled(false);
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(5);
        points->InsertPoint(0, 0.0, 0.0, 0.0);
        points->InsertPoint(1, 1.0, 1.0, 0.0);
        points->InsertPoint(2, 3.0, 4.0, 0.0);
        points->InsertPoint(3, 4.0, 3.0, 0.0);
        points->InsertPoint(4, 0.0, 0.0, 0.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(5);
        cellArray->InsertCellPoint(0);
        cellArray->InsertCellPoint(1);
        cellArray->InsertCellPoint(2);
        cellArray->InsertCellPoint(3);
        cellArray->InsertCellPoint(4);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("quadrilateral, unfilled") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->addVertex(Vector3(1.0, 1.0, 0.0));
        drawerPolygon->addVertex(Vector3(3.0, 4.0, 0.0));
        drawerPolygon->addVertex(Vector3(4.0, 3.0, 0.0));
        drawerPolygon->setFilled(true);
        // Per saber els punts finals hem de passar per GluTessellator. No poden ser hardcoded perquè poden canviar segons la implementació de GLU.
        QList<Vector3> vertices;
        vertices << Vector3(0.0, 0.0, 0.0) << Vector3(1.0, 1.0, 0.0) << Vector3(3.0, 4.0, 0.0) << Vector3(4.0, 3.0, 0.0);
        GluTessellator tessellator;
        tessellator.tessellate(vertices);
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vertices = tessellator.getVertices();
        points->SetNumberOfPoints(vertices.size());
        for (int i = 0; i < vertices.size(); i++)
        {
            points->SetPoint(i, vertices[i].data());
        }
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        const QList<GluTessellator::Triangle> &triangles = tessellator.getTriangles();
        for (int i = 0; i < triangles.size(); i++)
        {
            cellArray->InsertNextCell(3);
            for (int j = 0; j < 3; j++)
            {
                cellArray->InsertCellPoint(triangles[i].indices[j]);
            }
        }
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetPolys(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("quadrilateral, filled") << drawerPolygon << propAssembly;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(-1.0, 8.0,  8.0));
        drawerPolygon->addVertex(Vector3( 9.0, 8.0,  9.0));
        drawerPolygon->addVertex(Vector3( 7.0, 8.0,  2.0));
        drawerPolygon->addVertex(Vector3( 6.0, 8.0,  1.0));
        drawerPolygon->addVertex(Vector3( 9.0, 8.0, -8.0));
        drawerPolygon->addVertex(Vector3( 2.0, 8.0,  1.0));
        drawerPolygon->addVertex(Vector3( 0.0, 8.0, -6.0));
        drawerPolygon->addVertex(Vector3( 3.0, 8.0, -8.0));
        drawerPolygon->addVertex(Vector3(-4.0, 8.0, -4.0));
        drawerPolygon->setFilled(false);
        drawerPolygon->setCoordinateSystem(DrawerPrimitive::DisplayCoordinateSystem);
        drawerPolygon->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);
        drawerPolygon->setLineWidth(8.0);
        drawerPolygon->setOpacity(0.969);
        drawerPolygon->setVisibility(false);
        drawerPolygon->setColor(QColor(153, 67, 150));
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(10);
        points->InsertPoint(0, -1.0, 8.0,  8.0);
        points->InsertPoint(1,  9.0, 8.0,  9.0);
        points->InsertPoint(2,  7.0, 8.0,  2.0);
        points->InsertPoint(3,  6.0, 8.0,  1.0);
        points->InsertPoint(4,  9.0, 8.0, -8.0);
        points->InsertPoint(5,  2.0, 8.0,  1.0);
        points->InsertPoint(6,  0.0, 8.0, -6.0);
        points->InsertPoint(7,  3.0, 8.0, -8.0);
        points->InsertPoint(8, -4.0, 8.0, -4.0);
        points->InsertPoint(9, -1.0, 8.0,  8.0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        cellArray->InsertNextCell(10);
        cellArray->InsertCellPoint(0);
        cellArray->InsertCellPoint(1);
        cellArray->InsertCellPoint(2);
        cellArray->InsertCellPoint(3);
        cellArray->InsertCellPoint(4);
        cellArray->InsertCellPoint(5);
        cellArray->InsertCellPoint(6);
        cellArray->InsertCellPoint(7);
        cellArray->InsertCellPoint(8);
        cellArray->InsertCellPoint(9);
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToDisplay();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInputData(polyData);
        mapper->SetTransformCoordinate(coordinate);
        vtkSmartPointer<vtkActor2D> backgroundActor = DrawerPolygonTestHelper::createDefaultBackgroundActor();
        backgroundActor->GetProperty()->SetLineStipplePattern(0xFF00);
        backgroundActor->GetProperty()->SetLineWidth(10.0);
        backgroundActor->GetProperty()->SetOpacity(0.969);
        backgroundActor->SetVisibility(false);
        backgroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkActor2D> foregroundActor = DrawerPolygonTestHelper::createDefaultForegroundActor();
        foregroundActor->GetProperty()->SetLineStipplePattern(0xFF00);
        foregroundActor->GetProperty()->SetLineWidth(8.0);
        foregroundActor->GetProperty()->SetOpacity(0.969);
        foregroundActor->SetVisibility(false);
        QColor color(153, 67, 150);
        foregroundActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        foregroundActor->SetMapper(mapper);
        vtkSmartPointer<vtkPropAssembly> propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
        propAssembly->AddPart(backgroundActor);
        propAssembly->AddPart(foregroundActor);
        QTest::newRow("random") << drawerPolygon << propAssembly;
    }
}

void test_DrawerPolygon::getAsVtkProp_ShouldReturnPropLikeExpected()
{
    QFETCH(DrawerPolygon*, drawerPolygon);
    QFETCH(vtkSmartPointer<vtkPropAssembly>, expectedProp);

    vtkPropAssembly *prop = vtkPropAssembly::SafeDownCast(drawerPolygon->getAsVtkProp());
    vtkPropCollection *parts = prop->GetParts();
    QCOMPARE(parts->GetNumberOfItems(), 2);
    parts->InitTraversal();
    vtkActor2D *backgroundActor = vtkActor2D::SafeDownCast(parts->GetNextProp());
    vtkActor2D *foregroundActor = vtkActor2D::SafeDownCast(parts->GetNextProp());
    vtkPropCollection *expectedParts = expectedProp->GetParts();
    expectedParts->InitTraversal();
    vtkActor2D *expectedBackgroundActor = vtkActor2D::SafeDownCast(expectedParts->GetNextProp());
    vtkActor2D *expectedForegroundActor = vtkActor2D::SafeDownCast(expectedParts->GetNextProp());
    QCOMPARE(backgroundActor->GetVisibility(), expectedBackgroundActor->GetVisibility());
    QCOMPARE(foregroundActor->GetVisibility(), expectedForegroundActor->GetVisibility());
    QVERIFY(backgroundActor->GetMapper() == foregroundActor->GetMapper());
    vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::SafeDownCast(foregroundActor->GetMapper());
    vtkPolyDataMapper2D *expectedMapper = vtkPolyDataMapper2D::SafeDownCast(expectedForegroundActor->GetMapper());
    QString firstDifference;
    QVERIFY2(DrawerPolygonTestHelper::compareMappers(mapper, expectedMapper, firstDifference), qPrintable(firstDifference));
    QVERIFY2(DrawerPolygonTestHelper::compareProperties(backgroundActor->GetProperty(), expectedBackgroundActor->GetProperty(), firstDifference),
             qPrintable(firstDifference));
    QVERIFY2(DrawerPolygonTestHelper::compareProperties(foregroundActor->GetProperty(), expectedForegroundActor->GetProperty(), firstDifference),
             qPrintable(firstDifference));
}

void test_DrawerPolygon::isModified_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<DrawerPolygon*>("drawerPolygon");
    QTest::addColumn<bool>("modified");

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        QTest::newRow("brand new") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        QTest::newRow("after addVertix") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->removeVertices();
        QTest::newRow("after removeVertices") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(1.0, 2.0, 3.0));
        drawerPolygon->getAsVtkProp();
        drawerPolygon->setVertex(0, Vector3(0.0, 0.0, 0.0));
        QTest::newRow("after setVertix in range") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->setVertex(0, Vector3(0.0, 0.0, 0.0));
        QTest::newRow("after setVertix out of range") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->getAsVtkProp();
        drawerPolygon->getVertex(0);
        QTest::newRow("after getVertix in range (on unmodified)") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->getVertex(0);
        QTest::newRow("after getVertix in range (on modified)") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->getVertex(0);
        QTest::newRow("after getVertix out of range (on unmodified)") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->removeVertices();
        drawerPolygon->getVertex(0);
        QTest::newRow("after getVertix out of range (on modified)") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->getAsVtkProp();
        QTest::newRow("after getAsVtkProp") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->getNumberOfPoints();
        QTest::newRow("after getNumberOfPoints (on unmodified)") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->getNumberOfPoints();
        QTest::newRow("after getNumberOfPoints (on modified)") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        Vector3 point, closestPoint;
        drawerPolygon->getDistanceToPoint(point, closestPoint);
        QTest::newRow("after getDistanceToPoint (on unmodified)") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        Vector3 point, closestPoint;
        drawerPolygon->getDistanceToPoint(point, closestPoint);
        QTest::newRow("after getDistanceToPoint (on modified)") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        double bounds[6];
        drawerPolygon->getBounds(bounds);
        QTest::newRow("after getBounds (on unmodified)") << drawerPolygon << false;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        double bounds[6];
        drawerPolygon->getBounds(bounds);
        QTest::newRow("after getBounds (on modified)") << drawerPolygon << true;
    }

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        drawerPolygon->addVertex(Vector3(0.0, 0.0, 0.0));
        drawerPolygon->update();
        QTest::newRow("after update") << drawerPolygon << false;
    }
}

void test_DrawerPolygon::isModified_ShouldReturnExpectedValue()
{
    QFETCH(DrawerPolygon*, drawerPolygon);
    QFETCH(bool, modified);

    QCOMPARE(drawerPolygon->isModified(), modified);
}

void test_DrawerPolygon::get2DPlaneIndices_ReturnsExpectedValues_data()
{
    QTest::addColumn<DrawerPolygon*>("polygon");
    QTest::addColumn<int>("expectedXIndex");
    QTest::addColumn<int>("expectedYIndex");

    DrawerPolygon *polygon = 0;
    Vector3 p1{0.0, 0.0, 0.0};
    Vector3 p2{1.0, 1.0, 0.0};
    Vector3 p3{3.0, 4.0, 0.0};
    Vector3 p4{4.0, 3.0, 0.0};

    polygon = new DrawerPolygon(this);
    polygon->addVertex(p1);
    polygon->addVertex(p2);
    polygon->addVertex(p3);
    polygon->addVertex(p4);
    QTest::newRow("Axial plane polygon") << polygon << 0 << 1;

    polygon = new DrawerPolygon(this);
    qSwap(p2[0], p2[2]);
    qSwap(p3[0], p3[2]);
    qSwap(p4[0], p4[2]);
    polygon->addVertex(p1);
    polygon->addVertex(p2);
    polygon->addVertex(p3);
    polygon->addVertex(p4);
    QTest::newRow("Sagital plane polygon") << polygon << 1 << 2;

    polygon = new DrawerPolygon(this);
    qSwap(p2[0], p2[1]);
    qSwap(p3[0], p3[1]);
    qSwap(p4[0], p4[1]);
    polygon->addVertex(p1);
    polygon->addVertex(p2);
    polygon->addVertex(p3);
    polygon->addVertex(p4);
    QTest::newRow("Coronal plane polygon") << polygon << 0 << 2;

    polygon = new DrawerPolygon(this);
    p1[1] = 1.0;
    p2[1] = 2.0;
    p3[1] = 3.0;
    p4[1] = 4.0;
    polygon->addVertex(p1);
    polygon->addVertex(p2);
    polygon->addVertex(p3);
    polygon->addVertex(p4);
    QTest::newRow("Undetermined plane polygon") << polygon << -1 << -1;
}

void test_DrawerPolygon::get2DPlaneIndices_ReturnsExpectedValues()
{
    QFETCH(DrawerPolygon*, polygon);
    QFETCH(int, expectedXIndex);
    QFETCH(int, expectedYIndex);

    int x, y;
    polygon->get2DPlaneIndices(x, y);
    QCOMPARE(x, expectedXIndex);
    QCOMPARE(y, expectedYIndex);
}

DECLARE_TEST(test_DrawerPolygon)

#include "test_drawerpolygon.moc"
