#include "qenhancedmenutoolbutton.h"

#include <QMenu>

namespace udg {

namespace {

// Returns the top left point of the button's menu necessary for it to be positioned above the given button.
QPoint getAbovePosition(const QEnhancedMenuToolButton *button)
{
    return button->mapToGlobal(QPoint(0, -button->menu()->sizeHint().height()));
}

// Returns the top left point of the button's menu necessary for it to be positioned below the given button.
QPoint getBelowPosition(const QEnhancedMenuToolButton *button)
{
    return button->mapToGlobal(button->rect().bottomLeft());
}

// Modifies the given menu point so that the given button and its menu are right-aligned.
void alignRight(QPoint &menuPoint, const QEnhancedMenuToolButton *button)
{
    menuPoint.rx() -= button->menu()->sizeHint().width() - button->width();
}

}

QEnhancedMenuToolButton::QEnhancedMenuToolButton(QWidget *parent)
 : QToolButton(parent), m_menu(0), m_menuPosition(Below), m_menuAlignment(AlignLeft)
{
}

QEnhancedMenuToolButton::~QEnhancedMenuToolButton()
{
}

QMenu* QEnhancedMenuToolButton::menu() const
{
    return m_menu;
}

void QEnhancedMenuToolButton::setMenu(QMenu *menu)
{
    m_menu = menu;
    connect(this, SIGNAL(pressed()), this, SLOT(showMenu()), Qt::UniqueConnection);
}

void QEnhancedMenuToolButton::setMenuPosition(MenuPosition position)
{
    m_menuPosition = position;
}

void QEnhancedMenuToolButton::setMenuAlignment(MenuAlignment alignment)
{
    m_menuAlignment = alignment;
}

void QEnhancedMenuToolButton::showMenu()
{
    if (!menu())
    {
        return;
    }

    this->setDown(true);
    QPoint menuPoint;

    switch (m_menuPosition)
    {
        case Above:
            menuPoint = getAbovePosition(this);
            break;

        default:
        case Below:
            menuPoint = getBelowPosition(this);
            break;
    }

    switch (m_menuAlignment)
    {
        default:
        case AlignLeft:
            break;

        case AlignRight:
            alignRight(menuPoint, this);
            break;
    }

    menu()->exec(menuPoint);
    this->setDown(false);
}

}
