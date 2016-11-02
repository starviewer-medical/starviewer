/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "roitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "image.h"
#include "mathtools.h"
#include "areameasurecomputer.h"
#include "voxel.h"
#include "roidata.h"
#include "roidataprinter.h"
#include "petctfusionroidataprinter.h"
#include "petroidataprinter.h"
#include "nmroidataprinter.h"
#include "nmctfusionroidataprinter.h"
#include "sliceorientedvolumepixeldata.h"

#include <QApplication>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

namespace udg {

ROITool::ROITool(QViewer *viewer, QObject *parent)
 : MeasurementTool(viewer, parent), m_roiPolygon(0)
{
    m_toolName = "ROITool";
    m_hasSharedData = false;
}

ROITool::~ROITool()
{
}

MeasureComputer* ROITool::getMeasureComputer()
{
    return new AreaMeasureComputer(m_roiPolygon);
}

QMap<int, ROIData> ROITool::computeROIData()
{
    Q_ASSERT(m_roiPolygon);

    auto pixelData = m_2DViewer->getCurrentPixelData();

    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputData(m_roiPolygon->getVtkPolyData());
    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->SetMatrix(pixelData.getWorldToDataMatrix());
    transformFilter->SetTransform(transform);
    transformFilter->Update();
    auto *pixelDataOrientedRoiPolyData = transformFilter->GetOutput();
    // Pixel data oriented because it's not really slice oriented: still needs to permute axes

    // To compute the voxels inside the polygon we'll use the sweep line algorithm approach
    // tracing a set of lines within the bounds of the drawn polygon. Upon the resulting intersections between polygon segments and sweep lines
    // we will be able to compute which points of the line are inside of the polygon and get the corresponding voxel values.
    // The sweep lines will be horizontal and swept down in vertical direction
    double bounds[6];
    pixelDataOrientedRoiPolyData->GetBounds(bounds);

    int xIndex, yIndex, zIndex;
    pixelData.getOrthogonalPlane().getXYZIndexes(xIndex, yIndex, zIndex);

    // Building up the initial sweep line
    // We'll have to add some extra space to the x/y bounds just to help the sweep line algorithm work better
    // when the vertices and segments are just on the bound lines
    Vector3 spacing = pixelData.getSpacing();   // This is really slice oriented
    double xMargin = spacing.x * 1.1;
    double yMargin = spacing.y * 1.1;
    
    // First point of the sweep line, will be at the minimum x, y, z bounds of the polygon
    Point3D sweepLineBeginPoint;
    sweepLineBeginPoint[xIndex] = bounds[xIndex * 2] - xMargin;
    sweepLineBeginPoint[yIndex] = bounds[yIndex * 2] - yMargin;
    sweepLineBeginPoint[zIndex] = bounds[zIndex * 2];
    
    // Second point of the sweep line, will be the same as the first but with the maximum x bounds of the polygon so it will trace an horizontal line
    Point3D sweepLineEndPoint;
    sweepLineEndPoint[xIndex] = bounds[xIndex * 2 + 1] + xMargin;
    sweepLineEndPoint[yIndex] = bounds[yIndex * 2] - yMargin;
    sweepLineEndPoint[zIndex] = bounds[zIndex * 2];

    // The ending height of the sweep line will be at the maximum y bounds of the polygon
    double verticalLimit = bounds[yIndex * 2 + 1] + yMargin;

    // Build segments
    QList<Line3D> segments;
    auto *points = pixelDataOrientedRoiPolyData->GetPoints();
    for (int i = 0; i < points->GetNumberOfPoints() - 1; i++)
    {
        segments.append(Line3D(points->GetPoint(i), points->GetPoint(i+1)));
    }
    segments.append(Line3D(points->GetPoint(points->GetNumberOfPoints() - 1), points->GetPoint(0)));

    // Compute the ROI data corresponding for each input
    QMap<int, ROIData> roiDataMap;
    for (int i = 0; i < m_2DViewer->getNumberOfInputs(); ++i)
    {
        // Compute the voxel values inside of the polygon if the input is visible and the images are monochrome
        if (m_2DViewer->isInputVisible(i) && !m_2DViewer->getInput(i)->getImage(0)->getPhotometricInterpretation().isColor())
        {
            ROIData roiData = computeVoxelValues(segments, sweepLineBeginPoint, sweepLineEndPoint, verticalLimit, i);
            
            // Set additional information of the ROI data
            roiData.setUnits(m_2DViewer->getInput(i)->getPixelUnits());
            roiData.setModality(m_2DViewer->getInput(i)->getModality());

            roiDataMap.insert(i, roiData);
        }
    }

    return roiDataMap;
}

ROIData ROITool::computeVoxelValues(const QList<Line3D> &polygonSegments, Point3D sweepLineBeginPoint, Point3D sweepLineEndPoint, double sweepLineEnd, int inputNumber)
{
    // We get the pixel data to obtain voxels values from
    auto pixelData = m_2DViewer->getCurrentPixelDataFromInput(inputNumber);
    
    OrthogonalPlane currentView = pixelData.getOrthogonalPlane();
    int yIndex = currentView.getYIndex();

    double verticalSpacingIncrement = pixelData.getSpacing().y; // slice oriented

    // ROI voxel data to be obtained from the sweep line
    ROIData roiData;
    while (sweepLineBeginPoint.at(yIndex) <= sweepLineEnd)
    {
        // We get the intersections bewteen ROI segments and current sweep line
        QList<double*> intersectionList = getIntersectionPoints(polygonSegments, Line3D(sweepLineBeginPoint, sweepLineEndPoint), currentView);

        // Adding the voxels from the current intersections of the current sweep line to the voxel values list
        addVoxelsFromIntersections(intersectionList, currentView, pixelData, roiData);
        
        // Shift the sweep line the corresponding space in vertical direction
        sweepLineBeginPoint[yIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[yIndex] += verticalSpacingIncrement;
    }

    return roiData;
}

QList<int> ROITool::getIndexOfSegmentsCrossingAtHeight(const QList<Line3D> &segments, double height, int heightIndex)
{
    QList<int> intersectedSegmentsIndexList;

    for (int i = 0; i < segments.count(); ++i)
    {
        if ((height <= segments.at(i).getFirstPoint().at(heightIndex) && height >= segments.at(i).getSecondPoint().at(heightIndex))
        || (height >= segments.at(i).getFirstPoint().at(heightIndex) && height <= segments.at(i).getSecondPoint().at(heightIndex)))
        {
            intersectedSegmentsIndexList << i;
        }
    }

    return intersectedSegmentsIndexList;
}

QList<double*> ROITool::getIntersectionPoints(const QList<Line3D> &polygonSegments, const Line3D &sweepLine, const OrthogonalPlane &view)
{
    QList<double*> intersectionPoints;
    int sortIndex = view.getXIndex();
    int heightIndex = view.getYIndex();
    
    QList<int> indexListOfSegmentsToIntersect = getIndexOfSegmentsCrossingAtHeight(polygonSegments, sweepLine.getFirstPoint().at(heightIndex), heightIndex);
    foreach (int segmentIndex, indexListOfSegmentsToIntersect)
    {
        int intersectionState;
        double *foundPoint = MathTools::infiniteLinesIntersection(polygonSegments.at(segmentIndex).getFirstPoint().getAsDoubleArray(),
                                                                    polygonSegments.at(segmentIndex).getSecondPoint().getAsDoubleArray(),
                                                                    sweepLine.getFirstPoint().getAsDoubleArray(), sweepLine.getSecondPoint().getAsDoubleArray(),
                                                                    intersectionState);
        if (intersectionState == MathTools::LinesIntersect)
        {
            // Must sort intersections horizontally in order to be able to get voxels inside polygon correctly
            bool found = false;
            int i = 0;
            while (!found && i < intersectionPoints.count())
            {
                if (foundPoint[sortIndex] > intersectionPoints.at(i)[sortIndex])
                {
                    intersectionPoints.insert(i, foundPoint);
                    found = true;
                }
                else
                {
                    ++i;
                }
            }
            // If not found, it means it is the greatest point and must be placed at the end
            if (!found)
            {
                intersectionPoints << foundPoint;
            }
        }
    }

    return intersectionPoints;
}

void ROITool::addVoxelsFromIntersections(const QList<double*> &intersectionPoints, const OrthogonalPlane &view, SliceOrientedVolumePixelData &pixelData, ROIData &roiData)
{
    if (MathTools::isEven(intersectionPoints.count()))
    {
        int scanDirectionIndex = view.getXIndex();
        double scanDirectionIncrement = pixelData.getSpacing().x;   // slice oriented

        int limit = intersectionPoints.count() / 2;
        for (int i = 0; i < limit; ++i)
        {
            double *firstIntersection = intersectionPoints.at(i * 2);
            double *secondIntersection = intersectionPoints.at(i * 2 + 1);
            // First we check which will be the direction of the scan line
            Point3D currentScanLinePoint;
            double scanLineEnd;
            if (firstIntersection[scanDirectionIndex] <= secondIntersection[scanDirectionIndex])
            {
                for (int i = 0; i < 3; ++i)
                {
                    currentScanLinePoint[i] = firstIntersection[i];
                }
                scanLineEnd = secondIntersection[scanDirectionIndex];
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    currentScanLinePoint[i] = secondIntersection[i];
                }
                scanLineEnd = firstIntersection[scanDirectionIndex];
            }
            // Then we scan and get the voxels along the line
            while (currentScanLinePoint.at(scanDirectionIndex) <= scanLineEnd)
            {
                // currentScanLinePoint is pixel data oriented
                double pixelDataOrientedPoint[4] = { currentScanLinePoint[0], currentScanLinePoint[1], currentScanLinePoint[2], 1.0 };
                double worldPoint[4];
                pixelData.getDataToWorldMatrix()->MultiplyPoint(pixelDataOrientedPoint, worldPoint);
                Vector3 voxelCoordinate(worldPoint);
                roiData.addVoxel(pixelData.getVoxelValue(voxelCoordinate));
                currentScanLinePoint[scanDirectionIndex] += scanDirectionIncrement;
            }
        }
    }
    else
    {
        DEBUG_LOG(QString("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!: %1").arg(intersectionPoints.count()));
    }
}

void ROITool::printData()
{
    QString annotation = getAnnotation();

    DrawerText *text = new DrawerText;
    text->setText(annotation);

    setTextPosition(text);

    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void ROITool::setTextPosition(DrawerText *text)
{
    double bounds[6];
    m_roiPolygon->getBounds(bounds);

    double attachmentPoint[3];
    attachmentPoint[0] = (bounds[1] + bounds[0]) / 2.0;
    attachmentPoint[1] = (bounds[3] + bounds[2]) / 2.0;
    attachmentPoint[2] = (bounds[5] + bounds[4]) / 2.0;

    text->setAttachmentPoint(attachmentPoint);
}

QString ROITool::getAnnotation()
{
    Q_ASSERT(m_roiPolygon);

    // Calculem les dades estadístiques
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QMap<int, ROIData> roiDataMap = computeROIData();
    QApplication::restoreOverrideCursor();

    QString annotation;
    AbstractROIDataPrinter *roiDataPrinter = getROIDataPrinter(roiDataMap);
    if (roiDataPrinter)
    {
        annotation = roiDataPrinter->getString();
    }
    delete roiDataPrinter;
    m_2DViewer->restoreRenderingQuality();

    return annotation;
}

AbstractROIDataPrinter* ROITool::getROIDataPrinter(const QMap<int, ROIData> &roiDataMap)
{
    AbstractROIDataPrinter *roiDataPrinter = 0;

    switch (roiDataMap.count())
    {
        case 1:
            if (roiDataMap.value(0).getModality() == "PT")
            {
                roiDataPrinter = new PETROIDataPrinter(roiDataMap, getMeasurementString(), m_2DViewer);
            }
            else if (roiDataMap.value(0).getModality() == "NM")
            {
                roiDataPrinter = new NMROIDataPrinter(roiDataMap, getMeasurementString(), m_2DViewer);
            }
            break;

        case 2:
            {
                QStringList modalities;
                modalities << roiDataMap.value(0).getModality() << roiDataMap.value(1).getModality();
                if (modalities.contains("CT") && modalities.contains("PT"))
                {
                    roiDataPrinter = new PETCTFusionROIDataPrinter(roiDataMap, getMeasurementString(), m_2DViewer);
                }
                else if (modalities.contains("CT") && modalities.contains("NM"))
                {
                    roiDataPrinter = new NMCTFusionROIDataPrinter(roiDataMap, getMeasurementString(), m_2DViewer);
                }
            }
            break;
    }

    if (!roiDataPrinter)
    {
        roiDataPrinter = new ROIDataPrinter(roiDataMap, getMeasurementString(), m_2DViewer);
    }
    
    return roiDataPrinter;
}

}
