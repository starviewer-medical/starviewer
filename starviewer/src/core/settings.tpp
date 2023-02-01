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

namespace udg {

template<class C>
void Settings::saveColumnsWidths(const QString &key, const C *widget)
{
    Q_ASSERT(widget);

    int columnCount = widget->model()->columnCount();

    for (int column = 0; column < columnCount; column++)
    {
        QString columnKey = key + "/columnWidth" + QString::number(column);
        this->setValue(columnKey, widget->columnWidth(column));
    }
}

template<class C>
void Settings::restoreColumnsWidths(const QString &key, C *widget)
{
    Q_ASSERT(widget);

    int columnCount = widget->model()->columnCount();

    for (int column = 0; column < columnCount; column++)
    {
        QString columnKey = key + "/columnWidth" + QString::number(column);

        if (!this->contains(columnKey))
        {
            widget->resizeColumnToContents(column);
        }
        else
        {
            widget->setColumnWidth(column, this->getValue(columnKey).toInt());
        }
    }
}

}
