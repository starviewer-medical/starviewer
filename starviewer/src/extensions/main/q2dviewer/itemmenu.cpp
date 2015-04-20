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
#include <QApplication>
#include <QPalette>

namespace udg {

namespace {

// Returns the default application palette.
QPalette getDefaultPalette()
{
    return qApp->palette();
}

// Returns the palette to mark the item as selected.
QPalette getSelectedPalette()
{
    QPalette palette = getDefaultPalette();
    QBrush selected(QColor(85, 160, 255, 128));
    selected.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, selected);
    return palette;
}

}

ItemMenu::ItemMenu(QWidget *parent)
 : QFrame(parent), m_selected(false)
{
    setAutoFillBackground(true);
    m_fixed = false;

    QPalette systemPalette(qApp->palette());
    setPalette(systemPalette);
}

ItemMenu::~ItemMenu()
{

}

bool ItemMenu::event(QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        setPalette(getSelectedPalette());
        emit isActive(this);
        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        setPalette(getSelectedPalette());
        emit isSelected(this);
        return true;
    }
    else if (event->type() == QEvent::Leave && !m_fixed && !m_selected)
    {
        setPalette(getDefaultPalette());
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

    if (option)
    {
        setPalette(getSelectedPalette());
    }
    else
    {
        setPalette(getDefaultPalette());
    }
}

void ItemMenu::setData(QString data)
{
    m_data = data;
}

QString ItemMenu::getData()
{
    return m_data;
}

}
