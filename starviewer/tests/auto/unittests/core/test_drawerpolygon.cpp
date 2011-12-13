#include "autotest.h"
#include "drawerpolygon.h"

#include "drawerpolygontesthelper.h"

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
};

Q_DECLARE_METATYPE(DrawerPolygon*)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkPropAssembly>)

void test_DrawerPolygon::getAsVtkProp_ShouldReturnPropLikeExpected_data()
{
    QTest::addColumn<DrawerPolygon*>("drawerPolygon");
    QTest::addColumn< vtkSmartPointer<vtkPropAssembly> >("expectedProp");

    {
        DrawerPolygon *drawerPolygon = new DrawerPolygon(this);
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(0);
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
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
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
        drawerPolygon->addVertix(1.0, 1.0, 1.0);
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
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
        drawerPolygon->addVertix(1.0, 1.0, 1.0);
        drawerPolygon->addVertix(4.0, 3.0, 3.0);
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
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
        drawerPolygon->addVertix(1.0, 1.0, 1.0);
        drawerPolygon->addVertix(4.0, 3.0, 3.0);
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
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
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
        drawerPolygon->addVertix(1.0, 1.0, 0.0);
        drawerPolygon->addVertix(3.0, 4.0, 0.0);
        drawerPolygon->addVertix(4.0, 3.0, 0.0);
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
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(0.0, 0.0, 0.0);
        drawerPolygon->addVertix(1.0, 1.0, 0.0);
        drawerPolygon->addVertix(3.0, 4.0, 0.0);
        drawerPolygon->addVertix(4.0, 3.0, 0.0);
        drawerPolygon->setFilled(true);
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
        polyData->SetPolys(cellArray);
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        mapper->SetInput(polyData);
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
        drawerPolygon->addVertix(-1.0, 8.0,  8.0);
        drawerPolygon->addVertix( 9.0, 8.0,  9.0);
        drawerPolygon->addVertix( 7.0, 8.0,  2.0);
        drawerPolygon->addVertix( 6.0, 8.0,  1.0);
        drawerPolygon->addVertix( 9.0, 8.0, -8.0);
        drawerPolygon->addVertix( 2.0, 8.0,  1.0);
        drawerPolygon->addVertix( 0.0, 8.0, -6.0);
        drawerPolygon->addVertix( 3.0, 8.0, -8.0);
        drawerPolygon->addVertix(-4.0, 8.0, -4.0);
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
        mapper->SetInput(polyData);
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

DECLARE_TEST(test_DrawerPolygon)

#include "test_drawerpolygon.moc"
