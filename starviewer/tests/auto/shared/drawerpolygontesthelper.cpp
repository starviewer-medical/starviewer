#include "drawerpolygontesthelper.h"

#include <QColor>
#include <QString>

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>

namespace testing {

vtkSmartPointer<vtkActor2D> DrawerPolygonTestHelper::createDefaultBackgroundActor()
{
    vtkSmartPointer<vtkActor2D> backgroundActor = vtkSmartPointer<vtkActor2D>::New();
    backgroundActor->SetVisibility(true);
    backgroundActor->GetProperty()->SetLineStipplePattern(0xFFFF);
    backgroundActor->GetProperty()->SetLineWidth(4.0);
    backgroundActor->GetProperty()->SetOpacity(1.0);
    backgroundActor->GetProperty()->SetColor(255.0, 255.0, 255.0);
    return backgroundActor;
}

vtkSmartPointer<vtkActor2D> DrawerPolygonTestHelper::createDefaultForegroundActor()
{
    vtkSmartPointer<vtkActor2D> foregroundActor = vtkSmartPointer<vtkActor2D>::New();
    foregroundActor->SetVisibility(true);
    foregroundActor->GetProperty()->SetLineStipplePattern(0xFFFF);
    foregroundActor->GetProperty()->SetLineWidth(2.0);
    foregroundActor->GetProperty()->SetOpacity(1.0);
    QColor color(255, 165, 0);
    foregroundActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
    return foregroundActor;
}

bool DrawerPolygonTestHelper::compareMappers(vtkPolyDataMapper2D *actualMapper, vtkPolyDataMapper2D *expectedMapper, QString &firstDifference)
{
    if (!compareTransformCoordinate(actualMapper->GetTransformCoordinate(), expectedMapper->GetTransformCoordinate(), firstDifference))
    {
        return false;
    }
    return comparePolyData(actualMapper->GetInput(), expectedMapper->GetInput(), firstDifference);
}

bool DrawerPolygonTestHelper::compareTransformCoordinate(vtkCoordinate *actualCoordinate, vtkCoordinate *expectedCoordinate, QString &firstDifference)
{
    if (actualCoordinate->GetCoordinateSystem() != expectedCoordinate->GetCoordinateSystem())
    {
        firstDifference = QString("Different transform coordinate system: actual: %1, expected: %2").arg(actualCoordinate->GetCoordinateSystemAsString())
                                                                                                    .arg(expectedCoordinate->GetCoordinateSystemAsString());
        return false;
    }
    return true;
}

bool DrawerPolygonTestHelper::comparePolyData(vtkPolyData *actualPolyData, vtkPolyData *expectedPolyData, QString &firstDifference)
{
    if (!comparePoints(actualPolyData->GetPoints(), expectedPolyData->GetPoints(), firstDifference))
    {
        return false;
    }
    if (!compareCells("line", actualPolyData->GetLines(), expectedPolyData->GetLines(), firstDifference))
    {
        return false;
    }
    return compareCells("poly", actualPolyData->GetPolys(), expectedPolyData->GetPolys(), firstDifference);
}

bool DrawerPolygonTestHelper::comparePoints(vtkPoints *actualPoints, vtkPoints *expectedPoints, QString &firstDifference)
{
    if (!actualPoints && !expectedPoints)
    {
        return true;
    }
    if (!actualPoints && expectedPoints)
    {
        firstDifference = "Different points: actual: null, expected: no null";
        return false;
    }
    if (actualPoints && !expectedPoints)
    {
        firstDifference = "Different points: actual: no null, expected: null";
        return false;
    }
    if (actualPoints->GetNumberOfPoints() != expectedPoints->GetNumberOfPoints())
    {
        firstDifference = QString("Different number of points: actual: %1, expected: %2").arg(actualPoints->GetNumberOfPoints())
                                                                                         .arg(expectedPoints->GetNumberOfPoints());
        return false;
    }
    int numberOfPoints = actualPoints->GetNumberOfPoints();
    for (int i = 0; i < numberOfPoints; i++)
    {
        double actualPoint[3];
        actualPoints->GetPoint(i, actualPoint);
        double expectedPoint[3];
        expectedPoints->GetPoint(i, expectedPoint);
        if (actualPoint[0] != expectedPoint[0] || actualPoint[1] != expectedPoint[1] || actualPoint[2] != expectedPoint[2])
        {
            firstDifference = QString("Different point #%1: actual: (%2, %3, %4), expected: (%5, %6, %7)").arg(i)
                                                                                                          .arg(actualPoint[0])
                                                                                                          .arg(actualPoint[1])
                                                                                                          .arg(actualPoint[2])
                                                                                                          .arg(expectedPoint[0])
                                                                                                          .arg(expectedPoint[1])
                                                                                                          .arg(expectedPoint[2]);
            return false;
        }
    }
    return true;
}

bool DrawerPolygonTestHelper::compareCells(const QString &type, vtkCellArray *actualCells, vtkCellArray *expectedCells, QString &firstDifference)
{
    if (actualCells->GetNumberOfCells() != expectedCells->GetNumberOfCells())
    {
        firstDifference = QString("Different number of %1s: actual: %2, expected: %3").arg(type)
                                                                                      .arg(actualCells->GetNumberOfCells())
                                                                                      .arg(expectedCells->GetNumberOfCells());
        return false;
    }
    actualCells->InitTraversal();
    expectedCells->InitTraversal();
    int numberOfCells = actualCells->GetNumberOfCells();
    for (int i = 0; i < numberOfCells; i++)
    {
        vtkIdType actualCellNumberOfPoints;
        vtkIdType *actualCellPoints;
        actualCells->GetNextCell(actualCellNumberOfPoints, actualCellPoints);
        vtkIdType expectedCellNumberOfPoints;
        vtkIdType *expectedCellPoints;
        expectedCells->GetNextCell(expectedCellNumberOfPoints, expectedCellPoints);
        if (actualCellNumberOfPoints != expectedCellNumberOfPoints)
        {
            firstDifference = QString("Different number of points in %1 #%2: actual: %3, expected: %4").arg(type).arg(i)
                                                                                                       .arg(actualCellNumberOfPoints)
                                                                                                       .arg(expectedCellNumberOfPoints);
            return false;
        }
        for (int j = 0; j < actualCellNumberOfPoints; j++)
        {
            if (actualCellPoints[j] != expectedCellPoints[j])
            {
                firstDifference = QString("Different point index #%1 in %2 #%3: actual: %4, expected: %5").arg(j).arg(type).arg(i)
                                                                                                          .arg(actualCellPoints[j])
                                                                                                          .arg(expectedCellPoints[j]);
                return false;
            }
        }
    }
    return true;
}

bool DrawerPolygonTestHelper::compareProperties(vtkProperty2D *actualProperty, vtkProperty2D *expectedProperty, QString &firstDifference)
{
    if (actualProperty->GetLineStipplePattern() != expectedProperty->GetLineStipplePattern())
    {
        firstDifference = QString("Different line stipple pattern: actual %1, expected: %2").arg(actualProperty->GetLineStipplePattern(), 4, 16)
                                                                                            .arg(expectedProperty->GetLineStipplePattern(), 4, 16);
        return false;
    }
    if (actualProperty->GetLineWidth() != expectedProperty->GetLineWidth())
    {
        firstDifference = QString("Different line width: actual %1, expected: %2").arg(actualProperty->GetLineWidth())
                                                                                  .arg(expectedProperty->GetLineWidth());
        return false;
    }
    if (actualProperty->GetOpacity() != expectedProperty->GetOpacity())
    {
        firstDifference = QString("Different opacity: actual %1, expected: %2").arg(actualProperty->GetOpacity()).arg(expectedProperty->GetOpacity());
        return false;
    }
    double *actualColor = actualProperty->GetColor();
    double *expectedColor = expectedProperty->GetColor();
    for (int i = 0; i < 3; i++)
    {
        if (actualColor[i] != expectedColor[i])
        {
            firstDifference = QString("Different color: actual (%1, %2, %3), expected: (%4, %5, %6)").arg(actualColor[0])
                                                                                                     .arg(actualColor[1])
                                                                                                     .arg(actualColor[2])
                                                                                                     .arg(expectedColor[0])
                                                                                                     .arg(expectedColor[1])
                                                                                                     .arg(expectedColor[2]);
            return false;
        }
    }
    return true;
}

}