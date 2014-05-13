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

#include "polylinetemporalroitool.h"
#include "polylinetemporalroitooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawerpolygon.h"
#include "drawer.h"
#include "mathtools.h"
// Vtk
#include <vtkPoints.h>
#include <vtkLine.h>

namespace udg {

PolylineTemporalROITool::PolylineTemporalROITool(QViewer *viewer, QObject *parent)
 : PolylineROITool(viewer, parent)
{
    m_toolName = "PolylineTemporalROITool";
    m_hasSharedData = false;
    m_hasPersistentData = true;

    m_myData = new PolylineTemporalROIToolData;

    connect(this, SIGNAL(finished()), this, SLOT(start()));
}

PolylineTemporalROITool::~PolylineTemporalROITool()
{
}

ToolData *PolylineTemporalROITool::getToolData() const
{
    return m_myData;
}

void PolylineTemporalROITool::setToolData(ToolData *data)
{
    // Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if (m_myData != data)
    {
        // Creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<PolylineTemporalROIToolData*>(data);
    }
}

void PolylineTemporalROITool::start()
{
    DEBUG_LOG("Start PolylineTemporalROI");
    double bounds[6];
    m_roiPolygon->getBounds(bounds);
    this->computeTemporalMean();
}

double PolylineTemporalROITool::computeTemporalMean()
{
    if (!m_myData->temporalImageHasBeenDefined())
    {
        return 0.0;
    }

    TemporalImageType::RegionType region = m_myData->getTemporalImage()->GetLargestPossibleRegion();
    // std::cout<<"Region="<<region<<std::endl;
    TemporalImageType::SizeType size = region.GetSize();
    // std::cout<<"Size="<<size<<std::endl;
    int temporalSize = size[0];
    QVector<double> mean (temporalSize);
    QVector<double> aux (temporalSize);
    int i, j;
    for (i = 0; i < temporalSize; i++)
    {
        mean[i] = 0.0;
    }

    int subId;
    int initialPosition;
    int endPosition;
    double intersectPoint[3];
    double *firstIntersection;
    double *secondIntersection;
    double pcoords[3];
    double t;
    double p0[3];
    double p1[3];
    int numberOfVoxels = 0;
    QList<double*> intersectionList;
    QList<int> indexList;
    vtkPoints *auxPoints;
    double rayP1[3];
    double rayP2[3];
    double verticalLimit;
    int currentView = m_2DViewer->getView();

    // El nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_roiPolygon->getNumberOfPoints() - 1;

    // Taula de punters a vtkLine per a representar cadascun dels segments del polígon
    QVector<vtkLine*> segments;

    // Creem els diferents segments
    for (i = 0; i < numberOfSegments; i++)
    {
        vtkLine *line = vtkLine::New();
        line->GetPointIds()->SetNumberOfIds(2);
        line->GetPoints()->SetNumberOfPoints(2);

        const double *p1 = m_roiPolygon->getVertix(i);
        const double *p2 = m_roiPolygon->getVertix(i + 1);

        line->GetPoints()->InsertPoint(0, p1);
        line->GetPoints()->InsertPoint(1, p2);

        segments << line;
    }

    double bounds[6];
    m_roiPolygon->getBounds(bounds);
    double *spacing = m_2DViewer->getMainInput()->getSpacing();

    int rayPointIndex;
    int intersectionIndex;
    switch (currentView)
    {
        case OrthogonalPlane::XYPlane:
            // xmin
            rayP1[0] = bounds[0];
            // y
            rayP1[1] = bounds[2];
            // z
            rayP1[2] = bounds[4];
            // xmax
            rayP2[0] = bounds[1];
            // y
            rayP2[1] = bounds[2];
            // z
            rayP2[2] = bounds[4];

            rayPointIndex = 1;
            intersectionIndex = 0;
            verticalLimit = bounds[3];
            break;

        case OrthogonalPlane::YZPlane:
            // xmin
            rayP1[0] = bounds[0];
            // ymin
            rayP1[1] = bounds[2];
            // zmin
            rayP1[2] = bounds[4];
            // xmin
            rayP2[0] = bounds[0];
            // ymin
            rayP2[1] = bounds[2];
            // zmax
            rayP2[2] = bounds[5];

            rayPointIndex = 1;
            intersectionIndex = 2;
            verticalLimit = bounds[3];
            break;

        case OrthogonalPlane::XZPlane:
            // xmin
            rayP1[0] = bounds[0];
            // ymin
            rayP1[1] = bounds[2];
            // zmin
            rayP1[2] = bounds[4];
            // xmax
            rayP2[0] = bounds[1];
            // ymin
            rayP2[1] = bounds[2];
            // zmin
            rayP2[2] = bounds[4];

            rayPointIndex = 2;
            intersectionIndex = 0;
            verticalLimit = bounds[5];
            break;
    }

    while (rayP1[rayPointIndex] <= verticalLimit)
    {
        intersectionList.clear();
        indexList.clear();
        for (i = 0; i < numberOfSegments; i++)
        {
            auxPoints = segments[i]->GetPoints();
            auxPoints->GetPoint(0, p0);
            auxPoints->GetPoint(1, p1);
            if ((rayP1[rayPointIndex] <= p0[rayPointIndex] && rayP1[rayPointIndex] >= p1[rayPointIndex])
                || (rayP1[rayPointIndex] >= p0[rayPointIndex] && rayP1[rayPointIndex] <= p1[rayPointIndex]))
            {
                indexList << i;
            }
        }
        // Obtenim les interseccions entre tots els segments de la ROI i el raig actual
        foreach (int segment, indexList)
        {
            if (segments[segment]->IntersectWithLine(rayP1, rayP2, 0.0001, t, intersectPoint, pcoords, subId) > 0)
            {
                double *findedPoint = new double[3];
                findedPoint[0] = intersectPoint[0];
                findedPoint[1] = intersectPoint[1];
                findedPoint[2] = intersectPoint[2];
                intersectionList.append (findedPoint);
            }
        }

        if (MathTools::isEven(intersectionList.count()))
        {
            int limit = intersectionList.count() / 2;
            for (i = 0; i < limit; i++)
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at(initialPosition);
                secondIntersection = intersectionList.at(endPosition);

                // Tractem els dos sentits de les interseccions

                // D'esquerra cap a dreta
                if (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])
                {
                    while (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])
                    {
                        aux = this->getGraySerie(firstIntersection, temporalSize);
                        for (j = 0; j < temporalSize; j++)
                        {
                            mean[j] += aux[j];
                        }
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] += spacing[0];
                    }
                }
                // De dreta cap a esquerra
                else
                {
                    while (firstIntersection[intersectionIndex] >= secondIntersection[intersectionIndex])
                    {
                        aux = this->getGraySerie(firstIntersection, temporalSize);
                        for (j = 0; j < temporalSize; j++)
                        {
                            mean[j] += aux[j];
                        }
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] -= spacing[0];
                    }
                }
            }
        }
        else
        {
            DEBUG_LOG("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI Ã‰S IMPARELL!!");
        }

        // Fem el següent pas en la coordenada que escombrem
        rayP1[rayPointIndex] += spacing[1];
        rayP2[rayPointIndex] += spacing[1];

        while (!intersectionList.isEmpty())
        {
            double *intersection = intersectionList.takeFirst();
            delete[] intersection;
        }
    }

    // std::cout<<"Num of voxels:"<<numberOfVoxels<<std::endl;

    for (j = 0; j < temporalSize; j++)
    {
        mean[j] /= numberOfVoxels;
        // std::cout<<"i: "<<j<<": "<<mean[j]<<std::endl;
    }

    this->m_myData->setMeanVector(mean);

    // Destruim els diferents segments que hem creat per simular la roi
    for (i = 0; i < numberOfSegments; i++)
    {
        segments[i]->Delete();
    }

    return 0.0;
}

QVector<double> PolylineTemporalROITool::getGraySerie(double *coords, int size)
{
    QVector<double> v (size);
    double *origin = m_2DViewer->getMainInput()->getOrigin();
    double *spacing = m_2DViewer->getMainInput()->getSpacing();
    itk::Index<4> index;
    int xIndex, yIndex, zIndex;
    int i;

    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);
    index[xIndex + 1] = (int)((coords[xIndex] - origin[xIndex])/spacing[xIndex]);
    index[yIndex + 1] = (int)((coords[yIndex] - origin[yIndex])/spacing[yIndex]);
    index[zIndex + 1] = m_2DViewer->getCurrentSlice();

    for (i = 0; i < size; i++)
    {
        index[0] = i;
        v[i] = m_myData->getTemporalImage()->GetPixel(index);
    }
    return v;
}

}
