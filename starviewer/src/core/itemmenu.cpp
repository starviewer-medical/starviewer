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

#include "itemmenu.h"

#include <QEvent>

namespace udg {

namespace {

// Returns the appropriate stylesheet according to whether the item is selected and it has border.
QString getStyleSheet(bool selected, bool border)
{
    if (!selected && !border)
    {
        return "";
    }

    QString styleSheet = "udg--ItemMenu { ";

    if (selected)
    {
        styleSheet += "background-color: #8055a0ff; ";
    }

    if (border)
    {
        styleSheet += "border: 1px solid #909090; ";
    }

    styleSheet += "}";

    return styleSheet;
}

}

ItemMenu::ItemMenu(QWidget *parent)
 : QFrame(parent), m_selected(false), m_border(false)
{
    m_fixed = false;
}

ItemMenu::~ItemMenu()
{
}

bool ItemMenu::event(QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        setStyleSheet(getStyleSheet(true, m_border));
        emit isActive(this);
        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        setStyleSheet(getStyleSheet(true, m_border));
        emit isSelected(this);
        return true;
    }
    else if (event->type() == QEvent::Leave && !m_fixed && !m_selected)
    {
        setStyleSheet(getStyleSheet(false, m_border));
        return true;
    }
    else
    {
        return QWidget::event(event);
    }
}

void ItemMenu::setFixed(bool option)
{
    m_fixed = option;
}

void ItemMenu::setSelected(bool option)
{
    m_selected = option;
    setStyleSheet(getStyleSheet(m_selected, m_border));
}

void ItemMenu::setData(QString data)
{
    m_data = data;
}

QString ItemMenu::getData()
{
    return m_data;
}

bool ItemMenu::hasBorder() const
{
    return m_border;
}

void ItemMenu::setBorder(bool on)
{
    m_border = on;
    setStyleSheet(getStyleSheet(m_selected, m_border));
}

}
