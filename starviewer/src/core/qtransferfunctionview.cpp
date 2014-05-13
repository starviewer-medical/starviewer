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

#include "qtransferfunctionview.h"

#include "transferfunctioneditor.h"

namespace udg {

QTransferFunctionView::QTransferFunctionView(TransferFunctionEditor *editor, QWidget *parent)
 : QWidget(parent), m_editor(editor)
{
}

QTransferFunctionView::~QTransferFunctionView()
{
}

void QTransferFunctionView::makeConnections()
{
    connect(m_editor, SIGNAL(transferFunctionChanged(TransferFunction)), SLOT(setTransferFunction(TransferFunction)));
    connect(m_editor, SIGNAL(nameChanged(QString)), SLOT(setName(QString)));
    connect(m_editor, SIGNAL(colorTransferFunctionChanged(ColorTransferFunction)), SLOT(setColorTransferFunction(ColorTransferFunction)));
    connect(m_editor, SIGNAL(scalarOpacityTransferFunctionChanged(OpacityTransferFunction)), SLOT(setScalarOpacityTransferFunction(OpacityTransferFunction)));
//    connect(m_editor, SIGNAL(gradientOpacityTransferFunctionChanged(OpacityTransferFunction)),
//            SLOT(setGradientOpacityTransferFunction(OpacityTransferFunction)));
    connect(m_editor, SIGNAL(colorPointAdded(double, QColor)), SLOT(addColorPoint(double, QColor)));
    connect(m_editor, SIGNAL(colorPointRemoved(double)), SLOT(removeColorPoint(double)));
    connect(m_editor, SIGNAL(scalarOpacityPointAdded(double, double)), SLOT(addScalarOpacityPoint(double, double)));
    connect(m_editor, SIGNAL(scalarOpacityPointRemoved(double)), SLOT(removeScalarOpacityPoint(double)));
//    connect(m_editor, SIGNAL(gradientOpacityPointAdded(double,double)), SLOT(addGradientOpacityPoint(double,double)));
//    connect(m_editor, SIGNAL(gradientOpacityPointRemoved(double)), SLOT(removeGradientOpacityPoint(double)));
}

} // End namespace udg
