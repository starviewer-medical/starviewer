#include "drawerbitmap.h"

#include "logging.h"
#include "mathtools.h"

#include <QColor>

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

namespace udg {

DrawerBitmap::DrawerBitmap(QObject *parent)
 : DrawerPrimitive(parent)
{
    m_origin[0] = m_origin[1] = m_origin[2] = 0.0;
    m_spacing[0] = m_spacing[1] = m_spacing[2] = 1.0;
    m_width = m_height = 0;
    m_data = 0;
    m_backgroundOpacity = 0.0;
    m_backgroundColor = Qt::black;
    m_foregroundColor = Qt::white;
    m_imageActor = 0;
}

DrawerBitmap::~DrawerBitmap()
{
    emit dying(this);
}

void DrawerBitmap::setOrigin(double origin[3])
{
    memcpy(m_origin, origin, 3 * sizeof(double));
}

void DrawerBitmap::setSpacing(double spacing[3])
{
    memcpy(m_spacing, spacing, 3 * sizeof(double));
}

void DrawerBitmap::setData(unsigned int width, unsigned int height, unsigned char *data)
{
    m_width = width;
    m_height = height;    
    m_data = data;
}

void DrawerBitmap::setBackgroundOpacity(double opacity)
{
    m_backgroundOpacity = opacity;
}

void DrawerBitmap::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
}

void DrawerBitmap::setForegroundColor(const QColor &color)
{
    m_foregroundColor = color;
}

vtkProp* DrawerBitmap::getAsVtkProp()
{
    if (!m_imageActor)
    {
        vtkImageData *imageData = rawDataToVtkImageData(m_data);   
        if (imageData)
        {
            // Construim LUT per aplicar transparències: fet a partir del codi de http://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/Transparency
            vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
            lookupTable->SetNumberOfTableValues(2);
            lookupTable->SetRange(0.0, 1.0);
            // Valor 0 té m_backgroundOpacity i color  m_backgroundColor
            lookupTable->SetTableValue(0,  m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundOpacity); 
            // Valor 1 és opac amb color m_foregroundColor
            lookupTable->SetTableValue(1, m_foregroundColor.redF(), m_foregroundColor.greenF(), m_foregroundColor.blueF(), 1.0);
            lookupTable->Build();
 
            vtkSmartPointer<vtkImageMapToColors> mapTransparency = vtkSmartPointer<vtkImageMapToColors>::New();
            mapTransparency->SetLookupTable(lookupTable);
            mapTransparency->SetInput(imageData);
            imageData->Delete();
            mapTransparency->PassAlphaToOutputOn();
 
            // Creem l'actor
            m_imageActor = vtkSmartPointer<vtkImageActor>::New();
            m_imageActor->SetInput(mapTransparency->GetOutput());
            m_imageActor->SetDisplayExtent(0, m_width - 1, 0, m_height - 1, 0, 0);
            m_imageActor->SetVisibility(this->isVisible());
        }
        else
        {
            DEBUG_LOG("Error al passar les dades del bitmap a format vtkImageActor o bé no hi ha dades");
        }
    }

    return m_imageActor;
}

double DrawerBitmap::getDistanceToPoint(double *point3D, double closestPoint[3])
{
    // Si el punt es troba dins del requadre del bitmap (només coordenades X,Y), retornarem distància 0 i com a closestPoint el point3D
    // Si el punt es troba fora del requadre del bitmap, la distància serà la d'aquella a l'aresta més propera al punt
    double bounds[6];
    getBounds(bounds);

    if (point3D[0] >= bounds[0] && point3D[0] <= bounds[1] && 
        point3D[1] >= bounds[2] && point3D[1] <= bounds[3])
    {
        memcpy(closestPoint, point3D, 3 * sizeof(double));
        return 0.0;
    }

    double topLeftCorner[3] = { bounds[0], bounds[2], bounds[4] };
    double topRightCorner[3] = { bounds[1], bounds[2], bounds[4] };
    double bottomRightCorner[3] = { bounds[1], bounds[3], bounds[4] };
    double bottomLeftCorner[3] = { bounds[0], bounds[3], bounds[4] };
    
    double minimumDistanceFound = MathTools::DoubleMaximumValue;
    double distance;
    double localClosestPoint[3];

    distance = MathTools::getPointToFiniteLineDistance(point3D, topLeftCorner, topRightCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        memcpy(closestPoint, localClosestPoint, 3 * sizeof(double));
    }

    distance = MathTools::getPointToFiniteLineDistance(point3D, topRightCorner, bottomRightCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        memcpy(closestPoint, localClosestPoint, 3 * sizeof(double));
    }
    
    distance = MathTools::getPointToFiniteLineDistance(point3D, bottomRightCorner, bottomLeftCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        memcpy(closestPoint, localClosestPoint, 3 * sizeof(double));
    }
    
    distance = MathTools::getPointToFiniteLineDistance(point3D, bottomLeftCorner, topLeftCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        memcpy(closestPoint, localClosestPoint, 3 * sizeof(double));
    }

    return minimumDistanceFound;
}

void DrawerBitmap::getBounds(double bounds[6])
{
    bounds[0] = m_origin[0];
    bounds[1] = m_origin[0] + m_spacing[0] * m_width;

    bounds[2] = m_origin[1];
    bounds[3] = m_origin[1] + m_spacing[1] * m_height;

    bounds[4] = m_origin[2];
    bounds[5] = m_origin[2] + m_spacing[2];
}

void DrawerBitmap::update()
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

void DrawerBitmap::updateVtkProp()
{
    if (m_imageActor)
    {
        // TODO De moment únicament contemplem que un cop creat el bitmap, l'únic que podrem modificar serà la seva visibilitat
        m_imageActor->SetVisibility(this->isVisible());
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar el bitmap, ja que l'actor encara no s'ha creat");
    }
}

vtkImageData* DrawerBitmap::rawDataToVtkImageData(unsigned char *data)
{
    if (!data)
    {
        return 0;
    }

    vtkImageData* imageData = vtkImageData::New();
    imageData->SetExtent(0, m_width - 1, 0, m_height - 1, 0, 0);
    imageData->SetScalarTypeToUnsignedChar(); // The data will be 8 bit
    imageData->SetNumberOfScalarComponents(1);
    imageData->AllocateScalars();
    
    int size = m_width * m_height;
    vtkUnsignedCharArray *ucharArray = vtkUnsignedCharArray::New();
    ucharArray->SetNumberOfTuples(size);
    // TODO De moment fem que vtk no esborri mai les dades a on apunta data quan l'objecte es destrueixi (3r paràmetre == 1)
    ucharArray->SetArray(data, size, 1);
    imageData->GetPointData()->SetScalars(ucharArray);
    ucharArray->Delete();

    imageData->SetOrigin(m_origin);
    imageData->SetSpacing(m_spacing);

    return imageData;
}

} // end namespace udg
