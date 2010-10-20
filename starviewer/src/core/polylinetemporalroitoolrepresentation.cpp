/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylinetemporalroitoolrepresentation.h"
#include "polylinetemporalroitooldata.h"

#include "logging.h"
//#include "series.h"
#include "drawer.h"
#include "drawerpolyline.h"
/*#include "drawertext.h"
#include "image.h"
#include "mathtools.h"*/
#include "q2dviewer.h"
/*#include "volume.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"*/
//vtk
#include <vtkLine.h>
#include <vtkPoints.h>
//#include <vtkImageData.h>

namespace udg {

PolylineTemporalROIToolRepresentation::PolylineTemporalROIToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
    m_myData = new PolylineTemporalROIToolData;
}

PolylineTemporalROIToolRepresentation::~PolylineTemporalROIToolRepresentation()
{
    delete m_polyline;
    //delete m_text;

    foreach (ToolHandler *handler, m_toolHandlers)
    {
        delete handler;
    }

    m_toolHandlers.clear();
    m_toolHandlersMap.clear();
}

ToolData *PolylineTemporalROIToolRepresentation::getToolData() const
{
    return m_myData;
}

void PolylineTemporalROIToolRepresentation::setToolData(ToolData *data)
{
    //Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if (m_myData != data)
    { 
        // creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<PolylineTemporalROIToolData *>(data);
    }
}

void PolylineTemporalROIToolRepresentation::setPolyline(DrawerPolyline *polyline)
{
    m_polyline = polyline;
    m_primitivesList << m_polyline;
}

void PolylineTemporalROIToolRepresentation::setParams(int view, int slice, double *origin, double *spacing)
{
    m_view = view;
    m_slice = slice;
    m_origin = origin;
    m_spacing = spacing;
    /*
    m_input = input;
    m_slabProjection = slabProjection;
    m_use = use;

    m_pixelSpacing = m_input->getImages().at(0)->getPixelSpacing();
    m_spacing = m_input->getSpacing();
    */
}

void PolylineTemporalROIToolRepresentation::calculate()
{
    DEBUG_LOG("Close Form! PTROI");
    m_polyline->addPoint(m_polyline->getPoint(0));
	m_polyline->update();

	double bounds[6];
	m_polyline->getBounds(bounds);

	if (!bounds)
    {
        DEBUG_LOG("Bounds no definits");
    }
    else
    {
        this->computeTemporalMean();
    }
    m_drawer->updateRenderer();
    //this->printData();

    /*if (m_toolHandlers.size() == 0) //already created
        this->createHandlers();*/
}

/*Volume::VoxelType PolylineTemporalROIToolRepresentation::getGrayValue(double *coords)
{
    double *origin = m_input->getOrigin();
    int index[3];

    switch(m_view)
    {
        case Q2DViewer::Axial:
            index[0] = (int)((coords[0] - origin[0])/m_spacing[0]);
            index[1] = (int)((coords[1] - origin[1])/m_spacing[1]);
            index[2] = m_slice;
            break;

        case Q2DViewer::Sagital:
            index[0] = m_slice;
            index[1] = (int)((coords[1] - origin[1])/m_spacing[1]);
            index[2] = (int)((coords[2] - origin[2])/m_spacing[2]);
            break;

        case Q2DViewer::Coronal:
            index[0] = (int)((coords[0] - origin[0])/m_spacing[0]);
            index[1] = m_slice;
            index[2] = (int)((coords[2] - origin[2])/m_spacing[2]);
            break;
    }

    if (m_use == 2)
        return *((Volume::VoxelType *)m_slabProjection->GetScalarPointer(index));
    else 
        return *(m_input->getScalarPointer(index));
}

void PolylineTemporalROIToolRepresentation::printData()
{
    double *bounds = m_polyline->getPolylineBounds();
    if (!bounds)
    {
        DEBUG_LOG("Bounds no definits");
    }
    else
    {
        double *intersection = new double[3];

        intersection[0] = (bounds[1]+bounds[0])/2.0;
        intersection[1] = (bounds[3]+bounds[2])/2.0;
        intersection[2] = (bounds[5]+bounds[4])/2.0;

        if (m_pixelSpacing[0] == 0.0 && m_pixelSpacing[1] == 0.0)
        {
            m_text->setText(tr("Area: %1 px2\nMean: %2\nSt.Dev.: %3").arg(m_polyline->computeArea(m_view, m_spacing), 0, 'f', 0).arg(this->computeGrayMean(), 0, 'f', 2).arg(this->computeStandardDeviation(), 0, 'f', 2));
        }
        else
        {
            m_text->setText(tr("Area: %1 mm2\nMean: %2\nSt.Dev.: %3").arg(m_polyline->computeArea(m_view)).arg(this->computeGrayMean(), 0, 'f', 2).arg(this->computeStandardDeviation(), 0, 'f', 2));
        }

        m_text->setAttatchmentPoint(intersection);
        m_text->update();
        m_drawer->drawWorkInProgress(m_text);
    }
}

void PolylineTemporalROIToolRepresentation::computeGrayValues()
{
    int i;
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

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_polyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    QVector<vtkLine*> segments;

    //creem els diferents segments
    for (i = 0; i < numberOfSegments; i++)
    {
        vtkLine *line = vtkLine::New();
        line->GetPointIds()->SetNumberOfIds(2);
        line->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_polyline->getPoint(i);
        double *p2 = m_polyline->getPoint(i+1);

        line->GetPoints()->InsertPoint(0, p1);
        line->GetPoints()->InsertPoint(1, p2);

        segments << line;
    }

    double *bounds = m_polyline->getPolylineBounds();

    int rayPointIndex;
    int intersectionIndex;
    switch(m_view)
    {
    case Q2DViewer::Axial:
        rayP1[0] = bounds[0];//xmin
        rayP1[1] = bounds[2];//y
        rayP1[2] = bounds[4];//z
        rayP2[0] = bounds[1];//xmax
        rayP2[1] = bounds[2];//y
        rayP2[2] = bounds[4];//z

        rayPointIndex = 1;
        intersectionIndex = 0;
        verticalLimit = bounds[3];
    break;

    case Q2DViewer::Sagital:
        rayP1[0] = bounds[0];//xmin
        rayP1[1] = bounds[2];//ymin
        rayP1[2] = bounds[4];//zmin
        rayP2[0] = bounds[0];//xmin
        rayP2[1] = bounds[2];//ymin
        rayP2[2] = bounds[5];//zmax

        rayPointIndex = 1;
        intersectionIndex = 2;
        verticalLimit = bounds[3];

    break;

    case Q2DViewer::Coronal:
        rayP1[0] = bounds[0];//xmin
        rayP1[1] = bounds[2];//ymin
        rayP1[2] = bounds[4];//zmin
        rayP2[0] = bounds[1];//xmax
        rayP2[1] = bounds[2];//ymin
        rayP2[2] = bounds[4];//zmin

        rayPointIndex = 2;
        intersectionIndex = 0;
        verticalLimit = bounds[5];
    break;
    }

    while(rayP1[rayPointIndex] <= verticalLimit)
    {
        intersectionList.clear();
        indexList.clear();
        for (i = 0; i < numberOfSegments; i++)
        {
            auxPoints = segments[i]->GetPoints();
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);
            if ((rayP1[rayPointIndex] <= p0[rayPointIndex] && rayP1[rayPointIndex] >= p1[rayPointIndex])
                || (rayP1[rayPointIndex] >= p0[rayPointIndex] && rayP1[rayPointIndex] <= p1[rayPointIndex]))
                indexList << i;

        }
        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
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

        if ((intersectionList.count() % 2)==0)
        {
            int limit = intersectionList.count()/2;
            for (i = 0; i < limit; i++)
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at(initialPosition);
                secondIntersection = intersectionList.at(endPosition);

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])//d'esquerra cap a dreta
                {
                    while (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])
                    {
                        grayValues << (double)getGrayValue(firstIntersection);
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] += m_spacing[0];
                    }
                }
                else //de dreta cap a esquerra
                {
                    while (firstIntersection[intersectionIndex] >= secondIntersection[intersectionIndex])
                    {
                        grayValues << (double)getGrayValue(firstIntersection);
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] -= m_spacing[0];
                    }
                }
            }
        }
        else
            DEBUG_LOG("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI Ã‰S IMPARELL!!");

        //fem el següent pas en la coordenada que escombrem
        rayP1[rayPointIndex] += m_spacing[1];
        rayP2[rayPointIndex] += m_spacing[1];
    }


    //destruim els diferents segments que hem creat per simular la roi
    for (i = 0; i < numberOfSegments; i++)
        segments[i]->Delete();


}

double PolylineTemporalROIToolRepresentation::computeGrayMean()
{
    computeGrayValues();

    double mean = 0.0;

    foreach (double value, grayValues)
        mean += value;

    //no es buida la llista pq la utilitzara computeStandardDeviation()

    return mean / grayValues.size();
}

double PolylineTemporalROIToolRepresentation::computeStandardDeviation()
{
    // no cal computeGrayValues(); ja ho ha fet computeGrayMean, que sempre es crida just abans

    double standardDeviation = 0.0;
    double mean = computeGrayMean();

    QList<double> deviations;

    foreach (double value, grayValues)
    {
        double individualDeviation = value - mean;
        deviations << (individualDeviation * individualDeviation);

    }

    grayValues.clear();

    foreach (double deviation, deviations)
        standardDeviation += deviation;

    standardDeviation /= deviations.size();

    return std::sqrt(standardDeviation);
}
*/

void PolylineTemporalROIToolRepresentation::createHandlers()
{
    /*HACK!!*/
    /*int zCoordinate;

    switch(m_view)
    {
    case Q2DViewer::Axial:
        zCoordinate = 2;
        break;
    case Q2DViewer::Sagital:
        zCoordinate = 0;
        break;
    case Q2DViewer::Coronal:
        zCoordinate = 1;
        break;
    }

    double depth = m_polyline->getPoint(0)[zCoordinate];

    /**//*

    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    for(int i = 0; i < m_polyline->getNumberOfPoints() - 1; i++)
    {
        ToolHandlerWithRepresentation *handler = new ToolHandlerWithRepresentation(m_drawer, m_polyline->getPoint(i));
        m_toolHandlers << handler;
        m_toolHandlersMap.insert(i, handler);
        connect(handler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
        connect(handler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
        connect(this, SIGNAL(hideToolHandlers()), handler, SLOT(hide()));
        connect(this, SIGNAL(showToolHandlers()), handler, SLOT(show()));

        connect(toolHandler, SIGNAL(moveAllPoints(double *)), handler, SLOT(move(double *)));

        handler->setParams(zCoordinate, depth);
    }

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << toolHandler;*/
}


QList<double *> PolylineTemporalROIToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    //const int key = m_toolHandlersMap.key(toolHandler);
    toolHandler = 0;

    QList<double *> list;
    /*list << m_polyline->getPoint(key);

    if (key == 0) //first point
        list << m_polyline->getPoint(m_polyline->getNumberOfPoints() - 1); //last point also
*/
    return list;
}

void PolylineTemporalROIToolRepresentation::moveAllPoints(double *movement)
{
    /*double *point;

    for(int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        point = m_polyline->getPoint(i);

        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    point = m_text->getAttatchmentPoint();

    point[0] += movement[0];
    point[1] += movement[1];
    point[2] += movement[2];

    this->refresh();*/
}

///!!!!

void PolylineTemporalROIToolRepresentation::convertInputImageToTemporalImage()
{
    //POTSER NO CAL I NOMÉS RECORREM L'INPUT LA REGIÓ QUE EN MARCA LA ROI
    //D'AQUESTA MANERA NO CALDRIA FER LA CÒPIA
/*
    m_temporalImageHasBeenDefined = true;

    TemporalImageType::RegionType regiont;
    TemporalImageType::IndexType startt;
    startt[0]=0;
    startt[1]=0;
    startt[2]=0;
    startt[3]=0;

    TemporalImageType::SizeType sizet;
    sizet[0] = m_2DViewer->getInput()->getNumberOfPhases();  //les mostres temporals
    sizet[1] = m_2DViewer->getInput()->getItkData()->GetBufferedRegion().GetSize()[0];  //les X
    sizet[2] = m_2DViewer->getInput()->getItkData()->GetBufferedRegion().GetSize()[0];  //les Y
    sizet[3] = m_2DViewer->getInput()->getNumberOfSlicesPerPhase();  //les Z

    //Ho definim així perquè l'iterador passi per totes les mostres temporals
    regiont.SetSize(sizet);
    regiont.SetIndex(startt);

    m_temporalImage = TemporalImageType::New();
    m_temporalImage->SetRegions(regiont);
    m_temporalImage->Allocate();
*/
}

double PolylineTemporalROIToolRepresentation::computeTemporalMean()
{
    if (!m_myData->temporalImageHasBeenDefined())
    {
        return 0.0;
    }

    TemporalImageType::RegionType region = m_myData->getTemporalImage()->GetLargestPossibleRegion();
    //std::cout<<"Region="<<region<<std::endl;
    TemporalImageType::SizeType size = region.GetSize();
    //std::cout<<"Size="<<size<<std::endl;
    int temporalSize = size[0];
    QVector<double> mean (temporalSize);
    QVector<double> aux (temporalSize);
    int i,j;
    for(i=0;i<temporalSize;i++)
    {
        mean[i]=0.0;
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
    //int currentView = m_2DViewer->getView();

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_polyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    QVector<vtkLine*> segments;

    //creem els diferents segments
    for (i = 0; i < numberOfSegments; i++)
    {
        vtkLine *line = vtkLine::New();
        line->GetPointIds()->SetNumberOfIds(2);
        line->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_polyline->getPoint(i);
        double *p2 = m_polyline->getPoint(i+1);

        line->GetPoints()->InsertPoint(0, p1);
        line->GetPoints()->InsertPoint(1, p2);

        segments << line;
    }

    double bounds[6];
	m_polyline->getBounds(bounds);

    //double *spacing = m_2DViewer->getInput()->getSpacing();

    int rayPointIndex;
    int intersectionIndex;
    switch ( m_view /*currentView*/)
    {
        case Q2DViewer::Axial:
            rayP1[0] = bounds[0];//xmin
            rayP1[1] = bounds[2];//y
            rayP1[2] = bounds[4];//z
            rayP2[0] = bounds[1];//xmax
            rayP2[1] = bounds[2];//y
            rayP2[2] = bounds[4];//z
        
            rayPointIndex = 1;
            intersectionIndex = 0;
            verticalLimit = bounds[3];
            break;

        case Q2DViewer::Sagital:
            rayP1[0] = bounds[0];//xmin
            rayP1[1] = bounds[2];//ymin
            rayP1[2] = bounds[4];//zmin
            rayP2[0] = bounds[0];//xmin
            rayP2[1] = bounds[2];//ymin
            rayP2[2] = bounds[5];//zmax

            rayPointIndex = 1;
            intersectionIndex = 2;      
            verticalLimit = bounds[3];
            break;

        case Q2DViewer::Coronal:
            rayP1[0] = bounds[0];//xmin
            rayP1[1] = bounds[2];//ymin
            rayP1[2] = bounds[4];//zmin
            rayP2[0] = bounds[1];//xmax
            rayP2[1] = bounds[2];//ymin
            rayP2[2] = bounds[4];//zmin

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
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);  
            if ((rayP1[rayPointIndex] <= p0[rayPointIndex] && rayP1[rayPointIndex] >= p1[rayPointIndex]) 
                || (rayP1[rayPointIndex] >= p0[rayPointIndex] && rayP1[rayPointIndex] <= p1[rayPointIndex]))
            {
                indexList << i;
            }

        }
        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
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

        if ((intersectionList.count() % 2)==0)
        {
            int limit = intersectionList.count()/2;
            for (i = 0; i < limit; i++)
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at(initialPosition);
                secondIntersection = intersectionList.at(endPosition);

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])//d'esquerra cap a dreta
                {
                    while (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])
                    {
                        aux = this->getGraySerie(firstIntersection, temporalSize);
                        for(j=0;j<temporalSize;j++)
                        {
                            mean[j] += aux[j];
                        }
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] += m_spacing[0];
                    }
                }
                else //de dreta cap a esquerra
                {
                    while (firstIntersection[intersectionIndex] >= secondIntersection[intersectionIndex])
                    {
                        aux = this->getGraySerie(firstIntersection, temporalSize);
                        for(j=0;j<temporalSize;j++)
                        {
                            mean[j] += aux[j];
                        }
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] -= m_spacing[0];
                    }
                }
            }
        }
        else
            DEBUG_LOG("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI Ã‰S IMPARELL!!");

        //fem el següent pas en la coordenada que escombrem
        rayP1[rayPointIndex] += m_spacing[1];
        rayP2[rayPointIndex] += m_spacing[1];
    }

    //std::cout<<"Num of voxels:"<<numberOfVoxels<<std::endl;

    for (j = 0; j < temporalSize; j++)
    {
        mean[j] /= numberOfVoxels;
        //std::cout<<"i: "<<j<<": "<<mean[j]<<std::endl;
    }
    this->m_myData->setMeanVector(mean);

    //destruim els diferents segments que hem creat per simular la roi
    for (i = 0; i < numberOfSegments; i++)
    {
        segments[i]->Delete();
    }

    return 0.0;
}

QVector<double> PolylineTemporalROIToolRepresentation::getGraySerie(double *coords, int size)
{
    QVector<double> v (size);
    //double *origin = m_2DViewer->getInput()->getOrigin();
    //double *spacing = m_2DViewer->getInput()->getSpacing();
    itk::Index<4> index;
    int i;

    switch (m_view /*m_2DViewer->getView()*/)
    {
        case Q2DViewer::Axial:
            index[1] = (int)((coords[0] - m_origin[0]) / m_spacing[0]);
            index[2] = (int)((coords[1] - m_origin[1]) / m_spacing[1]);
            index[3] = m_slice;//m_2DViewer->getCurrentSlice();
            break;

        case Q2DViewer::Sagital:
            index[1] = m_slice;//m_2DViewer->getCurrentSlice();
            index[2] = (int)((coords[1] - m_origin[1]) / m_spacing[1]);
            index[3] = (int)((coords[2] - m_origin[2]) / m_spacing[2]);
            break;

        case Q2DViewer::Coronal:
            index[1] = (int)((coords[0] - m_origin[0]) / m_spacing[0]);
            index[2] = m_slice;//m_2DViewer->getCurrentSlice();
            index[3] = (int)((coords[2] - m_origin[2]) / m_spacing[2]);
            break;
    }

    for (i=0;i<size;i++)
    {
        index[0]=i;
        v[i]= m_myData->getTemporalImage()->GetPixel(index);
    }
    return v;
}

}

