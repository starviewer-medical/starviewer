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

#ifndef UDG_PACSDEVICEMODEL_H
#define UDG_PACSDEVICEMODEL_H

#include <QAbstractTableModel>

#include "pacsdevicemanager.h"

namespace udg {

/**
 * @brief The PacsDeviceModel class is Qt model class that fetches data from PacsDeviceManager.
 */
class PacsDeviceModel : public QAbstractTableModel
{
    Q_OBJECT    // for tr()

public:
    /// Columns of this model.
    enum Columns { PacsId, AeTitleOrBaseUri, Institution, Description, Default, NumberOfColumns };

    explicit PacsDeviceModel(QObject *parent = nullptr);

    /// Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /// Returns the number of columns for the children of the given parent.
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /// Returns the data stored under the given role for the item referred to by the index.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /// Returns the data for the given role and section in the header with the specified orientation.
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /// Refreshes the model metadata to update views associated to it.
    void refresh();

    /// Sets a filter to include only some PACS in the model.
    void setPacsFilter(PacsDeviceManager::PacsFilter filter);

private:
    /// Filter to include only some PACS in the model.
    PacsDeviceManager::PacsFilter m_pacsFilter;
};

} // namespace udg

#endif // UDG_PACSDEVICEMODEL_H
