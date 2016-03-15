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

#ifndef UDG_RELATIVEGEOMETRYLAYOUT_H
#define UDG_RELATIVEGEOMETRYLAYOUT_H

#include <QLayout>

namespace udg {

/**
 * @brief The RelativeGeometryLayout class is a layout manager that lays out items according to the relative geometry associated with each item.
 *
 * The relative geometry is given as a QRectF where coordinates in each axis must be in the range [0, 1].
 */
class RelativeGeometryLayout : public QLayout
{
    Q_OBJECT
public:
    explicit RelativeGeometryLayout(QWidget *parent);
    explicit RelativeGeometryLayout();
    virtual ~RelativeGeometryLayout();

    /// Adds the given item to this layout with the given relative geometry.
    void addItem(QLayoutItem *item, const QRectF &geometry);
    /// Adds the given widget to this layout with the given relative geometry.
    void addWidget(QWidget *widget, const QRectF &geometry);

    /// Returns the relative geometry of the given item in this layout. If the given item is not in this layout, a null QRectF is returned.
    QRectF geometry(QLayoutItem *item) const;
    /// Returns the relative geometry of the given widget in this layout. If the given widget is not in this layout, a null QRectF is returned.
    QRectF geometry(QWidget *widget) const;
    /// Returns the relative geometry of the item at the given index in this layout. If there is nothing at the given index, a null QRectF is returned.
    QRectF geometryAt(int index) const;

    /// Sets a new relative geometry to the given item in this layout. If the given item is not in this layout, nothing happens.
    void setGeometry(QLayoutItem *item, const QRectF &geometry);
    /// Sets a new relative geometry to the given widget in this layout. If the given widget is not in this layout, nothing happens.
    void setGeometry(QWidget *widget, const QRectF &geometry);
    /// Sets a new relative geometry to the item at the given index in this layout. If there is nothing at the given index, nothing happens.
    void setGeometryAt(int index, const QRectF &geometry);

    /// Returns the number of items in this layout.
    virtual int count() const;

    /// Returns the item at the given index in this layout. If there is nothing at the given index, returns 0.
    virtual QLayoutItem* itemAt(int index) const;
    /// Removes and returns the item at the given index in this layout. If there is nothing at the given index, returns 0.
    virtual QLayoutItem* takeAt(int index);

    /// Sets the geometry of this layout. The geometry of all the items is then adjusted.
    virtual void setGeometry(const QRect &rect);

    /// Returns the preferred size of this layout, which is (0, 0).
    virtual QSize sizeHint() const;

protected:
    /// Adds the given item to this layout with a relative geometry of (0, 0, 1, 1).
    virtual void addItem(QLayoutItem *item);

private:
    Q_DISABLE_COPY(RelativeGeometryLayout)

private:
    /// This struct wraps a layout item with its corresponding relative geometry.
    struct ItemWrapper
    {
        ItemWrapper(QLayoutItem *item, const QRectF &geometry) :
            item(item), geometry(geometry)
        {
        }

        QLayoutItem *item;
        QRectF geometry;
    };

    /// List of all the items in this layout.
    QList<ItemWrapper*> m_items;

};

} // namespace udg

#endif // UDG_RELATIVEGEOMETRYLAYOUT_H
