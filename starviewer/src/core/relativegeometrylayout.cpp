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

#include "relativegeometrylayout.h"

#include <cmath>

namespace udg {

RelativeGeometryLayout::RelativeGeometryLayout(QWidget *parent) :
    QLayout(parent)
{
}

RelativeGeometryLayout::RelativeGeometryLayout()
{
}

RelativeGeometryLayout::~RelativeGeometryLayout()
{
    foreach (ItemWrapper *wrapper, m_items)
    {
        delete wrapper->item;
        delete wrapper;
    }
}

void RelativeGeometryLayout::addItem(QLayoutItem *item, const QRectF &geometry)
{
    m_items.append(new ItemWrapper(item, geometry));
}

void RelativeGeometryLayout::addWidget(QWidget *widget, const QRectF &geometry)
{
    addChildWidget(widget);
    addItem(new QWidgetItem(widget), geometry);
}

QRectF RelativeGeometryLayout::geometry(QLayoutItem *item) const
{
    return geometry(item->widget());
}

QRectF RelativeGeometryLayout::geometry(QWidget *widget) const
{
    return geometryAt(indexOf(widget));
}

QRectF RelativeGeometryLayout::geometryAt(int index) const
{
    if (index >= 0 && index < m_items.size())
    {
        return m_items[index]->geometry;
    }
    else
    {
        return QRectF();
    }
}

void RelativeGeometryLayout::setGeometry(QLayoutItem *item, const QRectF &geometry)
{
    setGeometry(item->widget(), geometry);
}

void RelativeGeometryLayout::setGeometry(QWidget *widget, const QRectF &geometry)
{
    setGeometryAt(indexOf(widget), geometry);
}

void RelativeGeometryLayout::setGeometryAt(int index, const QRectF &geometry)
{
    if (index >= 0 && index < m_items.size())
    {
        m_items[index]->geometry = geometry;
    }
}

int RelativeGeometryLayout::count() const
{
    return m_items.size();
}

QLayoutItem* RelativeGeometryLayout::itemAt(int index) const
{
    if (index >= 0 && index < m_items.count())
    {
        return m_items.at(index)->item;
    }
    else
    {
        return 0;
    }
}

QLayoutItem* RelativeGeometryLayout::takeAt(int index)
{
    if (index >= 0 && index < m_items.count())
    {
        ItemWrapper *wrapper = m_items.takeAt(index);
        QLayoutItem *item = wrapper->item;
        delete wrapper;
        return item;
    }
    else
    {
        return 0;
    }
}

void RelativeGeometryLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);

    foreach (ItemWrapper *wrapper, m_items)
    {
        int x = rect.x() + rect.width() * wrapper->geometry.x();
        int y = rect.y() + rect.height() * wrapper->geometry.y();
        int width = ceil(rect.width() * wrapper->geometry.width());
        int height = ceil(rect.height() * wrapper->geometry.height());
        wrapper->item->setGeometry(QRect(x, y, width, height));
    }
}

QSize RelativeGeometryLayout::sizeHint() const
{
    return QSize();
}

void RelativeGeometryLayout::addItem(QLayoutItem *item)
{
    addItem(item, QRectF(0.0, 0.0, 1.0, 1.0));
}

} // namespace udg
