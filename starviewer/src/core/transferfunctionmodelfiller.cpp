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

#include "transferfunctionmodelfiller.h"

#include "transferfunctionio.h"
#include "transferfunctionmodel.h"

#include <QDirIterator>

namespace udg {

void TransferFunctionModelFiller::addEmptyTransferFunction(TransferFunctionModel *model) const
{
    if (model)
    {
        model->insertRow(0);
        model->setData(model->index(0, 0), QObject::tr("None"), Qt::DisplayRole);
    }
}

void TransferFunctionModelFiller::removeEmptyTransferFunction(TransferFunctionModel *model) const
{
    if (model)
    {
        model->removeRow(0);
    }
}

void TransferFunctionModelFiller::add2DTransferFunctions(TransferFunctionModel *model) const
{
    if (model)
    {
        QDirIterator it(":/cluts/2d");

        while (it.hasNext())
        {
            TransferFunction *transferFunction = TransferFunctionIO::fromXmlFile(it.next());
            model->insertRow(model->rowCount());
            model->setTransferFunction(model->rowCount() - 1, *transferFunction);
            delete transferFunction;
        }
    }
}

}
