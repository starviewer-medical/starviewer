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

#include "qopacitytransferfunctiongraphicalviewline.h"

#include "qopacitytransferfunctiongraphicalviewnode.h"

namespace udg {

QOpacityTransferFunctionGraphicalViewLine::QOpacityTransferFunctionGraphicalViewLine()
 : m_leftNode(0), m_rightNode(0)
{
    setZValue(-1.0);
}

QOpacityTransferFunctionGraphicalViewNode* QOpacityTransferFunctionGraphicalViewLine::leftNode() const
{
    return m_leftNode;
}

void QOpacityTransferFunctionGraphicalViewLine::setLeftNode(QOpacityTransferFunctionGraphicalViewNode *leftNode)
{
    m_leftNode = leftNode;
    update();
}

QOpacityTransferFunctionGraphicalViewNode* QOpacityTransferFunctionGraphicalViewLine::rightNode() const
{
    return m_rightNode;
}

void QOpacityTransferFunctionGraphicalViewLine::setRightNode(QOpacityTransferFunctionGraphicalViewNode *rightNode)
{
    m_rightNode = rightNode;
    update();
}

void QOpacityTransferFunctionGraphicalViewLine::update()
{
    if (m_leftNode && m_rightNode)
    {
        setLine(m_leftNode->x(), m_leftNode->y(), m_rightNode->x(), m_rightNode->y());
    }
    else
    {
        setLine(QLineF());
    }
}

} // End namespace udg
