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

#include "qplaintextedit2.h"

#include <QTextBlock>

namespace udg {

QPlainTextEdit2::QPlainTextEdit2(QWidget *parent)
    : QPlainTextEdit(parent), m_alignment(Qt::AlignLeft)
{
}

Qt::Alignment QPlainTextEdit2::getAlignment() const
{
    return m_alignment;
}

void QPlainTextEdit2::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
}

void QPlainTextEdit2::paintEvent(QPaintEvent *event)
{
    // Implementation idea from
    // https://bugreports.qt.io/browse/QTBUG-7516?focusedCommentId=372157&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-372157
    QTextBlock block = firstVisibleBlock();

    while (block.isValid())
    {
        QTextOption option = document()->defaultTextOption();
        option.setAlignment(m_alignment);
        block.layout()->setTextOption(option);
        block = block.next();
    }

    QPlainTextEdit::paintEvent(event);
}

} // namespace udg
