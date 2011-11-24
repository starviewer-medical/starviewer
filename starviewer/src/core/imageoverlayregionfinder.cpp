#include "imageoverlayregionfinder.h"

#include "imageoverlay.h"

#include <QBitArray>
#include <QQueue>
#include <QRect>

namespace udg {

ImageOverlayRegionFinder::ImageOverlayRegionFinder(const ImageOverlay &overlay)
    : m_overlay(overlay)
{
}

void ImageOverlayRegionFinder::findRegions()
{
    m_regions.clear();

    if (!m_overlay.isValid())
    {
        return;
    }

    int rows = m_overlay.getRows();
    int columns = m_overlay.getColumns();
    unsigned char *data = m_overlay.getData();

    // Màscara que indica els píxels visitats
    QBitArray mask(rows * columns);

    for (int row = 0, i = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++, i++)
        {
            // Si trobem un objecte no tractat
            if (data[i] > 0 && !mask.testBit(i))
            {
                QRect region = growRegion(row, column, mask);
                addPadding(region);
                addRegion(region);
                fillMaskForRegion(mask, region);
            }
        }
    }
}

const QList<QRect>& ImageOverlayRegionFinder::regions() const
{
    return m_regions;
}

int ImageOverlayRegionFinder::distanceBetweenRegions(const QRect &region1, const QRect &region2)
{
    // Mètode inspirat en http://stackoverflow.com/questions/7286832/how-to-find-the-minimum-taxicab-manhattan-distance-between-two-parallel-rectangl

    // Consideracions:
    // - No hem d'oblidar que treballem amb píxels.
    // - La distància són els píxels de separació entre els rectangles (el màxim entre l'horitzontal i la vertical).
    // - Si hi ha intersecció entre els rectangles, la distància és 0.
    // - Si no hi ha intersecció però els rectangles es toquen, la distància és 0 (perquè no hi ha cap píxel de separació).
    //   - Això fa que haguem de restar 1 a tots els càlculs.
    // - Les coordenades avancen cap a la dreta i cap avall.
    // - Mantenim region1 com a referència.

    // Si hi ha intersecció, la distància és 0
    if (region1.intersects(region2))
    {
        return 0;
    }

    // Comprovem si region2 és en una cantonada
    if (region1.top() > region2.bottom() && region1.right() < region2.left())
    {
        //  2
        // 1
        return qMax(region1.top() - region2.bottom(), region2.left() - region1.right()) - 1;
    }
    else if (region1.bottom() < region2.top() && region1.right() < region2.left())
    {
        // 1
        //  2
        return qMax(region2.top() - region1.bottom(), region2.left() - region1.right()) - 1;
    }
    else if (region1.bottom() < region2.top() && region1.left() > region2.right())
    {
        //  1
        // 2
        return qMax(region2.top() - region1.bottom(), region1.left() - region2.right()) - 1;
    }
    else if (region1.top() > region2.bottom() && region1.left() > region2.right())
    {
        // 2
        //  1
        return qMax(region1.top() - region2.bottom(), region1.left() - region2.right()) - 1;
    }

    // Ara ja sabem que region2 no és en una cantonada, per tant només hem de mesurar la distància en una direcció
    if (region1.top() > region2.bottom())
    {
        // 2
        // 1
        return region1.top() - region2.bottom() - 1;
    }
    else if (region1.right() < region2.left())
    {
        // 1 2
        return region2.left() - region1.right() - 1;
    }
    else if (region1.bottom() < region2.top())
    {
        // 1
        // 2
        return region2.top() - region1.bottom() - 1;
    }
    else if (region1.left() > region2.right())
    {
        // 2 1
        return region1.left() - region2.right() - 1;
    }

    // No hem d'arribar mai aquí.
    Q_ASSERT(false);
    return -1;
}

int ImageOverlayRegionFinder::getDataIndex(int row, int column) const
{
    return row * m_overlay.getColumns() + column;
}

int ImageOverlayRegionFinder::getRowIndex(int i) const
{
    return i / m_overlay.getColumns();
}

int ImageOverlayRegionFinder::getColumnIndex(int i) const
{
    return i % m_overlay.getColumns();
}

QRect ImageOverlayRegionFinder::growRegion(int row, int column, QBitArray &mask)
{
    QRect region;
    region.setCoords(column, row, column, row);

    QQueue<int> queue;
    queue.enqueue(getDataIndex(row, column));

    while (!queue.isEmpty())
    {
        int i = queue.dequeue();

        if (m_overlay.getData()[i] > 0 && !mask.testBit(i))
        {
            mask.setBit(i);
            row = getRowIndex(i);
            column = getColumnIndex(i);
            
            if (row < region.top())
            {
                region.setTop(row);
            }
            if (row > region.bottom())
            {
                region.setBottom(row);
            }
            if (column < region.left())
            {
                region.setLeft(column);
            }
            if (column > region.right())
            {
                region.setRight(column);
            }

            if (column - 1 >= 0)
            {
                queue.enqueue(getDataIndex(row, column - 1));
            }
            if (column + 1 < static_cast<signed>(m_overlay.getColumns()))
            {
                queue.enqueue(getDataIndex(row, column + 1));
            }
            if (row - 1 >= 0)
            {
                queue.enqueue(getDataIndex(row - 1, column));
            }
            if (row + 1 < static_cast<signed>(m_overlay.getRows()))
            {
                queue.enqueue(getDataIndex(row + 1, column));
            }
        }
    }

    return region;
}

void ImageOverlayRegionFinder::fillMaskForRegion(QBitArray &mask, const QRect &region)
{
    for (int y = region.top(); y <= region.bottom(); y++)
    {
        int offset = y * m_overlay.getColumns();
        mask.fill(true, offset + region.left(), offset + region.right() + 1);
    }
}

void ImageOverlayRegionFinder::addPadding(QRect &region)
{
    if (region.top() > 0)
    {
        region.setTop(region.top() - 1);
    }
    if (region.bottom() < static_cast<signed>(m_overlay.getRows()) - 1)
    {
        region.setBottom(region.bottom() + 1);
    }
    if (region.left() > 0)
    {
        region.setLeft(region.left() - 1);
    }
    if (region.right() < static_cast<signed>(m_overlay.getColumns()) - 1)
    {
        region.setRight(region.right() + 1);
    }
}

void ImageOverlayRegionFinder::addRegion(QRect &newRegion)
{
    const int SizeDivisor = 10;

    int size = qMax(m_overlay.getRows(), m_overlay.getColumns());
    int joinThreshold = size / SizeDivisor;

    for (int i = 0; i < m_regions.size();)
    {
        QRect &region = m_regions[i];
        int distance = distanceBetweenRegions(newRegion, region);

        if (distance <= joinThreshold)
        {
            // join
            newRegion = newRegion.united(region);
            m_regions.removeAt(i);
            i = 0;
        }
        else
        {
            i++;
        }
    }

    m_regions << newRegion;
}

}
