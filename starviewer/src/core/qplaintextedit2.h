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

#ifndef UDG_QPLAINTEXTEDIT2_H
#define UDG_QPLAINTEXTEDIT2_H

#include <QPlainTextEdit>

namespace udg {

/**
 * @brief The QPlainTextEdit2 class is a subclass of QPlainTextEdit with some additional useful features.
 *
 * Additional features:
 * - horizontal text alignment
 */
class QPlainTextEdit2 : public QPlainTextEdit
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment alignment MEMBER m_alignment READ getAlignment  WRITE setAlignment)

public:
    /// Sets text alignment to left by default.
    explicit QPlainTextEdit2(QWidget *parent = nullptr);

    /// Returns the current horizontal text alignment.
    Qt::Alignment getAlignment() const;
    /// Sets the horizontal text alignment. Valid values are Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter and Qt::AlignJustify, but they are not checked.
    void setAlignment(Qt::Alignment alignment);

protected:
    /// Reimplemented to render with the set alignment.
    void paintEvent(QPaintEvent *event) override;

private:
    /// Horizontal text alignment.
    Qt::Alignment m_alignment;
};

} // namespace udg

#endif // UDG_QPLAINTEXTEDIT2_H
