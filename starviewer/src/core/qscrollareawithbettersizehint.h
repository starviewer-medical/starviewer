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

#ifndef UDG_QSCROLLAREAWITHBETTERSIZEHINT_H
#define UDG_QSCROLLAREAWITHBETTERSIZEHINT_H

#include <QScrollArea>

namespace udg {

/**
 * @brief The QScrollAreaWithBetterSizeHint class is subclass of QScrollArea that reimplements the sizeHint() to take into account the space occupied by the
 * scrollbars if these are shown.
 *
 * The resizeEvent() is also reimplemented to call updateGeometry() to indicate that the sizeHint() may have changed.
 */
class QScrollAreaWithBetterSizeHint : public QScrollArea
{
    Q_OBJECT
public:
    explicit QScrollAreaWithBetterSizeHint(QWidget *parent = nullptr);

    /// Implementation adapted from QAbstractScrollArea correcting the scrollbars visibility check.
    QSize sizeHint() const override;

protected:
    /// Calls the parent's resizeEvent() and updateGeometry().
    void resizeEvent(QResizeEvent *event) override;

};

} // namespace udg

#endif // UDG_QSCROLLAREAWITHBETTERSIZEHINT_H
