#include "drawerpolygon.h"
#include "glutessellator.h"
#include "logging.h"
#include "mathtools.h"
// Vtk
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkPropAssembly.h>
// Qt
#include <QVector>

namespace udg {

DrawerPolygon::DrawerPolygon(QObject *parent)
 : DrawerPrimitive(parent), m_pointsChanged(false), m_vtkPolyData(0), m_vtkPoints(0), m_vtkCellArray(0), m_vtkActor(0), m_vtkBackgroundActor(0), m_vtkMapper(0),
   m_vtkPropAssembly(0)
{
}

DrawerPolygon::~DrawerPolygon()
{
    emit dying(this);

    if (m_vtkPropAssembly)
    {
        m_vtkPolyData->Delete();
        m_vtkPoints->Delete();
        m_vtkCellArray->Delete();
        m_vtkActor->Delete();
        m_vtkBackgroundActor->Delete();
        m_vtkMapper->Delete();
        m_vtkPropAssembly->Delete();
    }
}

void DrawerPolygon::addVertix(double point[3])
{
    this->addVertix(point[0], point[1], point[2]);
}

void DrawerPolygon::addVertix(double x, double y, double z)
{
    QVector<double> array(3);

    array[0] = x;
    array[1] = y;
    array[2] = z;

    m_pointsList << array;
    emit changed();
    m_pointsChanged = true;
}

void DrawerPolygon::setVertix(int i, double point[3])
{
    this->setVertix(i, point[0], point[1], point[2]);
}

void DrawerPolygon::setVertix(int i, double x, double y, double z)
{
    if (i >= m_pointsList.count() || i < 0)
    {
        addVertix(x, y, z);
    }
    else
    {
        QVector<double> array(3);
        array = m_pointsList.takeAt(i);
        array[0] = x;
        array[1] = y;
        array[2] = z;

        m_pointsList.insert(i, array);
        emit changed();
        m_pointsChanged = true;
    }
}

void DrawerPolygon::removeVertices()
{
    m_pointsList.clear();
    emit changed();
    m_pointsChanged = true;
}

const double* DrawerPolygon::getVertix(int i)
{
    if (i >= m_pointsList.count() || i < 0)
    {
        double *vertix = new double[3];
        return vertix;
    }
    else
    {
        return m_pointsList.at(i).data();
    }
}

vtkProp* DrawerPolygon::getAsVtkProp()
{
    updateVtkProp();
    return m_vtkPropAssembly;
}

void DrawerPolygon::update()
{
    switch (m_internalRepresentation)
    {
        case VTKRepresentation:
            updateVtkProp();
            break;

        case OpenGLRepresentation:
            break;
    }
}

void DrawerPolygon::updateVtkProp()
{
    // La pipeline s'ha de construir la primera vegada, tant si hi ha hagut modificacions com si no
    if (!m_vtkPropAssembly)
    {
        buildVtkPipeline();
        updateVtkActorProperties();
    }

    if (this->isModified())
    {
        updateVtkActorProperties();
        this->setModified(false);
    }

    // Si hi ha hagut modificacions dels punts reconstruïm els polígons de VTK
    if (m_pointsChanged)
    {
        buildVtkPoints();
        m_pointsChanged = false;
    }
}

void DrawerPolygon::buildVtkPipeline()
{
    m_vtkCellArray = vtkCellArray::New();
    m_vtkPoints = vtkPoints::New();
    m_vtkPolyData = vtkPolyData::New();
    m_vtkMapper = vtkPolyDataMapper2D::New();
    m_vtkMapper->SetInput(m_vtkPolyData);
    m_vtkActor = vtkActor2D::New();
    m_vtkActor->SetMapper(m_vtkMapper);
    m_vtkBackgroundActor = vtkActor2D::New();
    m_vtkBackgroundActor->SetMapper(m_vtkMapper);
    m_vtkPropAssembly = vtkPropAssembly::New();
    m_vtkPropAssembly->AddPart(m_vtkBackgroundActor);
    m_vtkPropAssembly->AddPart(m_vtkActor);
}

void DrawerPolygon::buildVtkPoints()
{
    // Primer comprovem si el polígon és tancat. En cas que l'últim i el primer no coincideixin, l'afegim
    // TODO es podria comprovar si com a mínim té tres punts, sinó, no es pot considerar polígon
    bool extraVertix = false;
    if (!m_pointsList.isEmpty())
    {
        double *firstPoint = m_pointsList.first().data();
        double *lastPoint = m_pointsList.last().data();
        if ((firstPoint[0] != lastPoint[0]) || (firstPoint[1] != lastPoint[1]) || (firstPoint[2] != lastPoint[2]))
        {
            extraVertix = true;
        }
    }

    // Especifiquem el nombre de vèrtexs que té el polígon
    int numberOfVertices = m_pointsList.count() + (extraVertix ? 1 : 0);
    m_vtkCellArray->Reset();

    if (this->isFilled())
    {
        QList<Vector3> vertices;
        for (int i = 0; i < m_pointsList.size(); i++)
        {
            vertices.append(Vector3(m_pointsList[i][0], m_pointsList[i][1], m_pointsList[i][2]));
        }

        GluTessellator tessellator;
        tessellator.tessellate(vertices);
        
        vertices = tessellator.getVertices();
        m_vtkPoints->SetNumberOfPoints(vertices.size());

        for (int i = 0; i < vertices.size(); i++)
        {
            double point[3] = { vertices[i].x, vertices[i].y, vertices[i].z };
            m_vtkPoints->SetPoint(i, point);
        }

        const QList<GluTessellator::Triangle> &triangles = tessellator.getTriangles();
        DEBUG_LOG(QString("%1 triangles").arg(triangles.size()));

        for (int i = 0; i < triangles.size(); i++)
        {
            m_vtkCellArray->InsertNextCell(3);
            for (int j = 0; j < 3; j++)
            {
                m_vtkCellArray->InsertCellPoint(triangles[i].indices[j]);
            }
        }

        m_vtkPolyData->Initialize();
        // Assignem els punts al polydata
        m_vtkPolyData->SetPoints(m_vtkPoints);
        m_vtkPolyData->SetPolys(m_vtkCellArray);
    }
    else
    {
        if (numberOfVertices > 0)
        {
            m_vtkCellArray->InsertNextCell(numberOfVertices);
        }
        m_vtkPoints->SetNumberOfPoints(numberOfVertices);

        // Donem els punts/vèrtexs
        int i = 0;
        foreach (const QVector<double> &vertix, m_pointsList)
        {
            m_vtkPoints->InsertPoint(i, vertix.data());
            m_vtkCellArray->InsertCellPoint(i);
            i++;
        }

        if (extraVertix)
        {
            // Tornem a afegir el primer punt
            m_vtkPoints->InsertPoint(numberOfVertices - 1, m_pointsList.at(0).data());
            m_vtkCellArray->InsertCellPoint(numberOfVertices - 1);
        }
        m_vtkPolyData->Initialize();
        // Assignem els punts al polydata
        m_vtkPolyData->SetPoints(m_vtkPoints);
        m_vtkPolyData->SetLines(m_vtkCellArray);
    }
}

void DrawerPolygon::updateVtkActorProperties()
{
    // Sistema de coordenades
    m_vtkMapper->SetTransformCoordinate(this->getVtkCoordinateObject());
    // Estil de la línia
    m_vtkActor->GetProperty()->SetLineStipplePattern(m_linePattern);
    m_vtkBackgroundActor->GetProperty()->SetLineStipplePattern(m_linePattern);
    // Assignem gruix de la línia
    m_vtkActor->GetProperty()->SetLineWidth(m_lineWidth);
    m_vtkBackgroundActor->GetProperty()->SetLineWidth(m_lineWidth + 2);
    // Assignem opacitat de la línia
    m_vtkActor->GetProperty()->SetOpacity(m_opacity);
    m_vtkBackgroundActor->GetProperty()->SetOpacity(m_opacity);
    // Mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility(this->isVisible());
    m_vtkBackgroundActor->SetVisibility(this->isVisible());

    // Assignem color
    QColor color = this->getColor();
    m_vtkActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
    m_vtkBackgroundActor->GetProperty()->SetColor(255.0, 255.0, 255.0);
}

int DrawerPolygon::getNumberOfPoints() const
{
    return m_pointsList.count();
}

double DrawerPolygon::getDistanceToPoint(double *point3D, double closestPoint[3])
{
    int closestEdge;
    return MathTools::getPointToClosestEdgeDistance(point3D, m_pointsList, true, closestPoint, closestEdge);
}

void DrawerPolygon::getBounds(double bounds[6])
{
    if (m_vtkPolyData)
    {
        m_vtkPolyData->GetBounds(bounds);
    }
    else
    {
        memset(bounds, 0.0, sizeof(double) * 6);
    }
}

double DrawerPolygon::computeArea(Q2DViewer::CameraOrientationType view, double *spacing)
{
    double volumeSpacing[3];
    if (spacing == NULL)
    {
        volumeSpacing[0] = volumeSpacing[1] = volumeSpacing[2] = 1.0;
    }
    else
    {
        volumeSpacing[0] = spacing[0];
        volumeSpacing[1] = spacing[1];
        volumeSpacing[2] = spacing[2];
    }
    // Mètode extret de http://alienryderflex.com/polygon_area/

    // Obtenim els índexs x,y depenent de la vista en que estan projectats els punts
    int xIndex = Q2DViewer::getXIndexForView(view);
    int yIndex = Q2DViewer::getYIndexForView(view);
    // Realitzem el càlcul de l'àrea
    double area = 0.0;
    int j = 0;
    int numberOfPoints = m_pointsList.count();
    for (int i = 0; i < numberOfPoints; i++)
    {
        j++;
        if (j == numberOfPoints)
        {
            j = 0;
        }

        area += (m_pointsList.at(i)[xIndex] + m_pointsList.at(j)[xIndex]) * volumeSpacing[xIndex] * (m_pointsList.at(i)[yIndex] - m_pointsList.at(j)[yIndex]) *
                 volumeSpacing[yIndex];
    }

    // En el cas de que l'àrea de la polilínia ens doni negativa, vol dir que hem anotat els punts en sentit antihorari,
    // per això cal girar-los per tenir una disposició correcta. Cal girar-ho del vtkPoints i de la QList de la ROI
    if (area < 0)
    {
        // Donem el resultat el valor absolut
        area *= -1;
        // Intercanviem els punts de la QList
        // TODO Cal realment fer això?
        for (int i = 0; i < (int)(numberOfPoints / 2); i++)
        {
            m_pointsList.swap(i, (numberOfPoints - 1) - i);
        }
    }

    return area * 0.5;
}

}
