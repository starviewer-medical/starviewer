/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "menugridwidget.h"

#include "gridicon.h"
#include "itemmenu.h"
#include "logging.h"
#include "math.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include <QFrame>
#include <QPalette>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpacerItem>
#include <QMovie>
#include <QLabel>

namespace udg {

const int MenuGridWidget::MaximumNumberOfColumns = 5;

MenuGridWidget::MenuGridWidget(QWidget *parent)
 : QWidget(parent), m_searchingWidget(0)
{
    setWindowFlags(Qt::Popup);

    initializeWidget();

    // Creem el widget amb l'animació de "searching"
    createSearchingWidget();
}

MenuGridWidget::~MenuGridWidget()
{
}

void MenuGridWidget::initializeWidget()
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(6);
    m_gridLayout->setMargin(6);

    m_nextHangingProtocolRow = 0;
    m_nextHangingProtocolColumn = 0;

    m_gridLayoutHanging = new QGridLayout();
    m_gridLayoutHanging->setSpacing(6);
    m_gridLayoutHanging->setMargin(6);
    QSpacerItem *spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_gridLayoutHanging->addItem(spacerItem, 0, MaximumNumberOfColumns, 1, 1);

    QFrame *lineHanging = new QFrame(this);
    lineHanging->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineHanging->setFrameShape(QFrame::HLine);
    lineHanging->setFrameShadow(QFrame::Sunken);
    QLabel *labelHanging = new QLabel(this);
    labelHanging->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    labelHanging->setText("Hanging protocols");
    QHBoxLayout *hBoxLayoutHanging = new QHBoxLayout();
    hBoxLayoutHanging->setMargin(0);
    hBoxLayoutHanging->setSpacing(6);
    hBoxLayoutHanging->addWidget(lineHanging);
    hBoxLayoutHanging->addWidget(labelHanging);

    m_gridLayout->addLayout(hBoxLayoutHanging, 2, 0, 1, 1);
    m_gridLayout->addLayout(m_gridLayoutHanging, 3, 0, 1, 1);
}

ItemMenu* MenuGridWidget::createIcon(const HangingProtocol *hangingProtocol)
{
    ItemMenu *icon = new ItemMenu(this);
    icon->setData(QString(tr("%1").arg(hangingProtocol->getIdentifier())));
    icon->setGeometry(0, 0, 64, 80);
    icon->setMaximumWidth(64);
    icon->setMinimumWidth(64);
    icon->setMinimumHeight(80);
    icon->setMaximumHeight(80);
    icon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

    QLabel *sizeText = new QLabel(icon);
    sizeText->setText(hangingProtocol->getName());
    sizeText->setAlignment(Qt::AlignHCenter);
    sizeText->setGeometry(0, 64, 64, 80);

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol->getDisplaySets())
    {
        QString iconType = displaySet->getIconType();

        if (iconType.isEmpty())
        {
            iconType = hangingProtocol->getIconType();
        }

        GridIcon *newIcon = new GridIcon(icon, iconType);

        QStringList listOfPositions = displaySet->getPosition().split("\\");
        double x1 = listOfPositions.value(0).toDouble();
        double y1 = listOfPositions.value(1).toDouble();
        double x2 = listOfPositions.value(2).toDouble();
        double y2 = listOfPositions.value(3).toDouble();

        newIcon->setGeometry(x1*64, (1-y1)*64, ((x2-x1)*64), (y1-y2)*64);
        newIcon->show();
    }

    icon->show();
    connect(icon, SIGNAL(isSelected(ItemMenu*)), this, SLOT(emitSelected(ItemMenu*)));
    return icon;
}

void MenuGridWidget::emitSelected(ItemMenu *selected)
{
    hide();
    emit selectedGrid(selected->getData().toInt());
}

void MenuGridWidget::dropContent()
{
    foreach (ItemMenu *item, m_itemList)
    {
        m_gridLayoutHanging->removeWidget(item);
        delete item;
    }
    m_itemList.clear();
}

void MenuGridWidget::setHangingItems(const QList<HangingProtocol*> &listOfCandidates)
{
    dropContent();
    addHangingItems(listOfCandidates);
}

void MenuGridWidget::addHangingItems(const QList<HangingProtocol*> &items)
{
    int positionRow = 0;
    int positionColumn = 0;

    foreach (HangingProtocol *hangingProtocol, items)
    {
        ItemMenu *icon = createIcon(hangingProtocol);

        m_gridLayoutHanging->addWidget(icon, positionRow, positionColumn);
        m_itemList.push_back(icon);
        positionColumn ++;

        if (positionColumn == MaximumNumberOfColumns)
        {
            positionColumn = 0;
            positionRow++;
        }
    }

    m_nextHangingProtocolRow = positionRow;
    m_nextHangingProtocolColumn = positionColumn;

    if (m_putLoadingItem)
    {
        addSearchingItem();
    }

}

void MenuGridWidget::setSearchingItem(bool state)
{
    m_putLoadingItem = state;

    if (state == false)
    {
        if (m_gridLayoutHanging != 0)
        {
            m_searchingWidget->setVisible(false);
            m_gridLayoutHanging->removeWidget(m_searchingWidget);
        }
    }
}

void MenuGridWidget::addSearchingItem()
{
    // S'assumeix que el widget ha d'estar creat
    Q_ASSERT(m_searchingWidget);
    
    if (m_searchingWidget->isVisible() || (m_gridLayoutHanging == 0))
    {
        return;
    }

    // Afegim el widget dins del layout del menú i el fem visible
    m_gridLayoutHanging->addWidget(m_searchingWidget, m_nextHangingProtocolColumn, m_nextHangingProtocolRow);
    m_searchingWidget->setVisible(true);
	
    m_loadingColumn = m_nextHangingProtocolColumn;
    m_loadingRow = m_nextHangingProtocolRow;
}

void MenuGridWidget::createSearchingWidget()
{
    if (!m_searchingWidget)
    {
        m_searchingWidget = new QWidget(this);
        m_searchingWidget->setVisible(false);
        m_searchingWidget->setGeometry (0, 0, 64, 64);
        m_searchingWidget->setMaximumWidth(64);
        m_searchingWidget->setMinimumWidth(64);
        m_searchingWidget->setMinimumHeight(64);
        m_searchingWidget->setMaximumHeight(64);
        m_searchingWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
        QVBoxLayout *verticalLayout = new QVBoxLayout(m_searchingWidget);

        // Construcció del label per l'animació
        QMovie *searchingMovie = new QMovie(m_searchingWidget);
        searchingMovie->setFileName(QString::fromUtf8(":/images/loader.gif"));
        QLabel *searchingLabelMovie = new QLabel(m_searchingWidget);
        searchingLabelMovie->setMovie(searchingMovie);
        searchingLabelMovie->setAlignment(Qt::AlignCenter);

        // Construcció del label pel text
        QLabel *searchingLabelText = new QLabel(m_searchingWidget);
        searchingLabelText->setText("Searching...");

        // Es col·loca dins al widget i a la graella per mostrar-ho
        verticalLayout->addWidget(searchingLabelMovie);
        verticalLayout->addWidget(searchingLabelText);

        searchingMovie->start();
    }
}

}
