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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (C) 2016 The Qt Company Ltd.
    Contact: https://www.qt.io/licensing/

    This file is part of the QtWidgets module of the Qt Toolkit.

    $QT_BEGIN_LICENSE:LGPL$
    Commercial License Usage
    Licensees holding valid commercial Qt licenses may use this file in
    accordance with the commercial license agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and The Qt Company. For licensing terms
    and conditions see https://www.qt.io/terms-conditions. For further
    information use the contact form at https://www.qt.io/contact-us.

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 3 as published by the Free Software
    Foundation and appearing in the file LICENSE.LGPL3 included in the
    packaging of this file. Please review the following information to
    ensure the GNU Lesser General Public License version 3 requirements
    will be met: https://www.gnu.org/licenses/lgpl-3.0.html.

    GNU General Public License Usage
    Alternatively, this file may be used under the terms of the GNU
    General Public License version 2.0 or (at your option) the GNU General
    Public license version 3 or any later version approved by the KDE Free
    Qt Foundation. The licenses are as published by the Free Software
    Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
    included in the packaging of this file. Please review the following
    information to ensure the GNU General Public License requirements will
    be met: https://www.gnu.org/licenses/gpl-2.0.html and
    https://www.gnu.org/licenses/gpl-3.0.html.

    $QT_END_LICENSE$
 *************************************************************************************/

#include "qscrollareawithbettersizehint.h"

#include <QScrollBar>

namespace udg {

QScrollAreaWithBetterSizeHint::QScrollAreaWithBetterSizeHint(QWidget *parent)
    : QScrollArea(parent)
{
}

// Adapted from QAbstractScrollArea::sizeHint.
// The only relevant difference is that the visibility of a scrollbar is checked with !scrollBar->isVisible() instead of scrollBar->isHidden().
// This is required because the scrollbar objects are never directly hidden, their containers (parent widgets) are hidden instead.
QSize QScrollAreaWithBetterSizeHint::sizeHint() const
{
    if (this->sizeAdjustPolicy() == QAbstractScrollArea::AdjustIgnored)
    {
        return QSize(256, 192);
    }

    if (this->sizeAdjustPolicy() == QAbstractScrollArea::AdjustToContents)
    {
        const int f = 2 * this->frameWidth();
        const QSize frame(f, f);
        const bool vbarHidden = !this->verticalScrollBar()->isVisible() || this->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff;
        const bool hbarHidden = !this->horizontalScrollBar()->isVisible() || this->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff;
        const QSize scrollbars(vbarHidden ? 0 : this->verticalScrollBar()->sizeHint().width(),
                               hbarHidden ? 0 : this->horizontalScrollBar()->sizeHint().height());
        QSize sizeHint = frame + scrollbars + viewportSizeHint();

        return sizeHint;
    }

    return QAbstractScrollArea::sizeHint();
}

void QScrollAreaWithBetterSizeHint::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    updateGeometry();
}

} // namespace udg
