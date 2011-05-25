#include "magictool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolygon.h"

#include <QMessageBox>
#include <qmath.h>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>

namespace udg {

static const double MagicSize = 3.0;
static const double MagicFactor = 1.0;

MagicTool::MagicTool(QViewer *viewer, QObject *parent)
: ROITool(viewer, parent)
{
    m_magicSize = MagicSize;
    m_magicFactor = MagicFactor;
    m_lowerLevel = 0.0;
    m_upperLevel = 0.0;
    m_toolName = "MagicTool";

    m_roiPolygon = NULL;

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

MagicTool::~MagicTool()
{
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }
}

void MagicTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = NULL;
}

void MagicTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            startMagicRegion();
            break;
        case vtkCommand::LeftButtonReleaseEvent:
            closeForm();
            break;
        case vtkCommand::MouseMoveEvent:
            modifyMagicFactor();
            break;
        default:
            break;
    }
}

void MagicTool::startMagicRegion()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getView() != Q2DViewer::Axial)
        {
            QMessageBox::warning(m_2DViewer->parentWidget(), tr("Error"),
                                 tr("This tool can only be used in the acquisition direction. Sorry for the inconvinience."));
        }
        else
        {
            if (m_2DViewer->getCurrentCursorImageCoordinate(m_pickedPosition))
            {
                m_magicFactor = MagicFactor;
                m_magicSize = MagicSize;
                m_roiPolygon = new DrawerPolygon;
                m_roiPolygon->increaseReferenceCount();
                m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

                this->generateRegion();
            }
        }
    }
}

void MagicTool::closeForm()
{
    if (m_roiPolygon)
    {
        this->generateRegion();
        this->printData();
        m_roiPolygon->decreaseReferenceCount();
        m_roiPolygon = NULL;
    }
}

void MagicTool::modifyMagicFactor()
{
    if (m_roiPolygon)
    {
        int dy = 0.1 * (m_viewer->getEventPositionY() - m_viewer->getLastEventPositionY());
        if (m_magicFactor - dy > 0.0)
        {
            m_magicFactor -= dy;
        }
        this->generateRegion();
    }
}

void MagicTool::generateRegion()
{
    this->computeLevelRange();

    // Posem a true els punts on la imatge està dins els llindard i connectat amb la llavor (region growing)
    this->computeRegionMask();

    // Trobem els punts frontera i creem el polígon
    this->computePolygon();

    m_2DViewer->render();
}

void MagicTool::computeLevelRange()
{
    int index[3];
    m_2DViewer->getInput()->getPixelData()->computeCoordinateIndex(m_pickedPosition, index);

    // Calculem la desviació estàndard dins la finestra que ens marca la magic size
    double stdv = getStandardDeviation(index[0], index[1], index[2]);

    // Calculem els llindars com el valor en el pixel +/- la desviació estàndard * magic factor
    QVector<double> voxelValue;
    m_2DViewer->getInput()->getVoxelValue(m_pickedPosition, voxelValue);
    m_lowerLevel = voxelValue.at(0) - m_magicFactor * stdv;
    m_upperLevel = voxelValue.at(0) + m_magicFactor * stdv;

}

void MagicTool::computeRegionMask()
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);

    // Creem la màscara
    if (ext[0] == 0 && ext[2] == 0)
    {
        m_mask = QVector<bool>((ext[1] + 1) * (ext[3] + 1), false);
    }
    else
    {
        DEBUG_LOG("ERROR: extension no comença a 0");
    }

    // Busquem el voxel inicial
    int index[3];
    m_2DViewer->getInput()->getPixelData()->computeCoordinateIndex(m_pickedPosition, index);
    int a = index[0];
    int b = index[1];
    int c = index[2];
    // \TODO S'hauria de fer servir VolumePixelData::getVoxelValue o similar
    double value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(a, b, c, 0);
    if ((value >= m_lowerLevel) && (value <= m_upperLevel))
    {
        m_mask[b * ext[1] + a] = true;
    }
    else
    {
        return;
    }

    // Comencem el Region Growing
    QVector<int> movements;
    // First movement \TODO Codi duplicat amb main loop
    int i = 0;
    bool trobat = false;
    while (i < 4 && !trobat)
    {
        this->doMovement(a, b, i);
        // \TODO S'hauria de fer servir VolumePixelData::getVoxelValue o similar
        value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(a, b, c, 0);
        if ((value >= m_lowerLevel) && (value <= m_upperLevel))
        {
            m_mask[b * ext[1] + a] = true;
            trobat = true;
            movements.push_back(i);
        }
        if (!trobat)
        {
            this->undoMovement(a, b, i);
        }
        ++i;
    }

    // Main loop
    i = 0;
    while (movements.size() > 0)
    {
        trobat = false;
        while (i < 4 && !trobat)
        {
            this->doMovement(a, b, i);
            if ((a > ext[0]) && (a < ext[1]) && (b > ext[2]) && (b < ext[3]))
            {
                // \TODO S'hauria de fer servir VolumePixelData::getVoxelValue o similar
                value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(a, b, c, 0);
                if ((value >= m_lowerLevel) && (value <= m_upperLevel) && (!m_mask[b * ext[1] + a]))
                {
                    m_mask[b * ext[1] + a] = true;
                    trobat = true;
                    movements.push_back(i);
                    i = 0;
                }
            }
            if (!trobat)
            {
                this->undoMovement(a, b, i);
                ++i;
            }
        }
        if (!trobat)
        {
            this->undoMovement(a, b, movements.back());
            i = movements.back();
            ++i;
            movements.pop_back();
        }
    }
}

void MagicTool::doMovement(int &a, int &b, int movement)
{
    switch (movement)
    {
        // Up
        case 0:
            a++;
            break;
        // Down
        case 1:
            a--;
            break;
        case 2:
            b++;
            break;
        case 3:
            b--;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicTool::undoMovement(int &a, int &b, int movement)
{
    switch (movement)
    {
        // Up
        case 0:
            a--;
            break;
        // Down
        case 1:
            a++;
            break;
        case 2:
            b--;
            break;
        case 3:
            b++;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicTool::computePolygon()
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);

    int i = ext[0];
    int j;

    // Busquem el primer punt
    bool trobat = false;
    while ((i <= ext[1]) && !trobat)
    {
        j = ext[2];
        while ((j <= ext[3]) && !trobat)
        {
            if (m_mask[j * ext[1] + i])
            {
                trobat = true;
            }
            ++j;
        }
        ++i;
    }
    // L'índex és -1 pq els hem incrementat una vegada més
    int index[3];
    index[0] = i - 1;
    index[1] = j - 1;
    index[2] = m_2DViewer->getCurrentSlice();

    m_roiPolygon->removeVertices();

    // \TODO Acabar d'entendre això
    this->addPoint(7, index[0], index[1], index[2]);
    this->addPoint(1, index[0], index[1], index[2]);

    int nextIndex[3];
    nextIndex[2] = index[2];

    int direction = 0;

    bool loop = false;
    bool next = false;
    while (!loop)
    {
        this->getNextIndex(direction, index[0], index[1], nextIndex[0], nextIndex[1]);
        next = m_mask[nextIndex[1] * ext[1] + nextIndex[0]];
        while (!next && !loop)
        {
            if (((direction % 2) != 0) && !next)
            {
                this->addPoint(direction, index[0], index[1], index[2]);
                loop = this->isLoopReached();
            }
            direction = this->getNextDirection(direction);
            this->getNextIndex(direction, index[0], index[1], nextIndex[0], nextIndex[1]);
            next = m_mask[nextIndex[1] * ext[1] + nextIndex[0]];
        }
        index[0] = nextIndex[0];
        index[1] = nextIndex[1];
        direction = this->getInverseDirection(direction);
        direction = this->getNextDirection(direction);
    }

    m_roiPolygon->update();
    m_hasToComputeStatisticsData = true;
}

void MagicTool::getNextIndex(int direction, int x1, int y1, int &x2, int &y2)
{
    switch (direction)
    {
        case 0:
            x2 = x1 - 1;
            y2 = y1 - 1;
            break;
        case 1:
            x2 = x1;
            y2 = y1 - 1;
            break;
        case 2:
            x2 = x1 + 1;
            y2 = y1 - 1;
            break;
        case 3:
            x2 = x1 + 1;
            y2 = y1;
            break;
        case 4:
            x2 = x1 + 1;
            y2 = y1 + 1;
            break;
        case 5:
            x2 = x1;
            y2 = y1 + 1;
            break;
        case 6:
            x2 = x1 - 1;
            y2 = y1 + 1;
            break;
        case 7:
            x2 = x1 - 1;
            y2 = y1;
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }

}
int MagicTool::getNextDirection(int direction)
{
    int a = direction + 1;
    return (a == 8)? 0: a;
}

int MagicTool::getInverseDirection(int direction)
{
    return (direction + 4) % 8;
}

void MagicTool::addPoint(int direction, int x1, int y1, double z1)
{
    double point[3];
    double origin[3];
    double spacing[3];
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);

    switch (direction)
    {
        case 1:
            point[0] = x1 * spacing[0] + origin[0];
            point[1] = (y1 - 0.5) * spacing[1] + origin[1];
            break;
        case 3:
            point[0] = (x1 + 0.5) * spacing[0] + origin[0];
            point[1] = y1 * spacing[1] + origin[1];
            break;
        case 5:
            point[0] = x1 * spacing[0] + origin[0];
            point[1] = (y1 + 0.5) * spacing[1] + origin[1];
            break;
        case 7:
            point[0] = (x1 - 0.5) * spacing[0] + origin[0];
            point[1] = y1 * spacing[1] + origin[1];
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }
    point[2] = z1 * spacing[2] + origin[2];

    m_roiPolygon->addVertix(point);
}

bool MagicTool::isLoopReached()
{
    const double *firstVertix = this->m_roiPolygon->getVertix(0);
    const double *lastVertix = this->m_roiPolygon->getVertix(m_roiPolygon->getNumberOfPoints() - 1);
    return ((qAbs(firstVertix[0] - lastVertix[0]) < 0.0001) && (qAbs(firstVertix[1] - lastVertix[1]) < 0.0001));
}

double MagicTool::getStandardDeviation(int a, int b, int c)
{
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
    int minX = qMax(a - m_magicSize, ext[0]);
    int maxX = qMin(a + m_magicSize, ext[1]);
    int minY = qMax(b - m_magicSize, ext[2]);
    int maxY = qMin(b + m_magicSize, ext[3]);

    int index[3];
    index[2] = c;

    // Calculem la mitjana
    double mean = 0.0;
    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            index[0] = i;
            index[1] = j;
            // \TODO S'hauria de fer servir VolumePixelData::getVoxelValue o similar
            double value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(index[0], index[1], index[2], 0);
            mean += value;
        }
    }
    int numberOfSamples = (maxX - minX + 1) * (maxY - minY + 1);
    mean = mean / (double)numberOfSamples;

    // Calculem la desviació estandard
    double deviation = 0.0;
    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            index[0] = i;
            index[1] = j;
            // \TODO S'hauria de fer servir VolumePixelData::getVoxelValue o similar
            double value = m_2DViewer->getInput()->getVtkData()->GetScalarComponentAsDouble(index[0], index[1], index[2], 0);
            deviation += qPow(value - mean, 2);
        }
    }
    deviation = qSqrt(deviation / (double)numberOfSamples);
    return deviation;
}

}
