/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "viewerslayout.h"

#include "logging.h"

namespace udg {

ViewersLayout::ViewersLayout(QWidget *parent) 
 : QWidget(parent), m_selectedViewer(0)
{
    m_numberOfVisibleViewers = 0;
    initLayouts();
}

ViewersLayout::~ViewersLayout()
{
}

void ViewersLayout::initLayouts()
{
    m_rows = 1;
    m_columns = 1;
    m_totalRows = 1;
    m_totalColumns = 1;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    m_gridLayout->setMargin(0);

    m_viewersLayout = new QGridLayout();
    m_viewersLayout->setSpacing(0);
    m_viewersLayout->setMargin(0);

    m_gridLayout->addLayout(m_viewersLayout, 0, 0);

    this->setLayout(m_gridLayout);
    m_gridLayout->update();

    m_isRegular = true;
}

void ViewersLayout::removeLayouts()
{
    int numberOfViewers = m_vectorViewers.size();

    for (int i = 0; i < numberOfViewers; i++)
    {
        m_viewersLayout->removeWidget(m_vectorViewers.at(i));
    }
    m_positionsList.clear();
    m_numberOfVisibleViewers = 0;
}

void ViewersLayout::restoreLayouts()
{
    int numberOfViewers = m_vectorViewers.size();

    /// Si tenim més visors que el grid regular que teníem construït, el regenerem.
    if (numberOfViewers > m_totalColumns * m_totalRows)
    {
        m_totalColumns = ceil(sqrt((double)numberOfViewers));
        m_totalRows = m_totalColumns;

        if ((m_totalRows * m_totalColumns) > numberOfViewers)
        {
            for (int i = 0; i < (m_totalRows * m_totalColumns) - numberOfViewers; i++)
            {
                m_vectorViewers.push_back(getNewQ2DViewerWidget());
            }
        }
    }

    m_viewersLayout->addWidget(m_vectorViewers.at(0), 0, 0);

    int column = 1;
    int row = 0;
    // S'amaguen tots i es deixa el principal, es tornen a posar amb layout
    for (int i = 1; i < m_vectorViewers.size(); i++)
    {
        m_vectorViewers.value(i)->hide();
        m_viewersLayout->addWidget(m_vectorViewers.at(i), row, column);

        column++;
        if (column >= m_totalColumns)
        {
            row++;
            column = 0;
        }
    }

    setSelectedViewer(m_vectorViewers.at(0));
    m_rows = 1;
    m_columns = 1;
}

Q2DViewerWidget* ViewersLayout::getSelectedViewer()
{
    return m_selectedViewer;
}

Q2DViewerWidget* ViewersLayout::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget(this);
    connect(newViewer, SIGNAL(selected(Q2DViewerWidget *)), SLOT(setSelectedViewer(Q2DViewerWidget *)));
    // Per defecte no li posem cap annotació
    newViewer->getViewer()->removeAnnotation(Q2DViewer::AllAnnotation);

    emit viewerAdded(newViewer);
    return newViewer;
}

void ViewersLayout::addColumns(int columns)
{
    int viewerPosition = m_columns;
    Q2DViewerWidget *newViewer = 0;
    
    while (columns > 0)
    {
        m_columns += 1;
        m_totalColumns += 1;
        for (int rows = 0; rows < m_viewersLayout->rowCount(); ++rows)
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget(newViewer, rows, m_totalColumns - 1);
            m_vectorViewers.insert(viewerPosition,newViewer);
            viewerPosition += m_columns;
            if (rows >= m_rows)
            {
                newViewer->hide();
                emit viewerRemoved(newViewer);
            }
        }
        viewerPosition = m_columns;
        columns--;
    }
}

void ViewersLayout::addRows(int rows)
{
    Q2DViewerWidget *newViewer = 0;

    while (rows > 0)
    {
        m_rows += 1;
        m_totalRows += 1;
        //Afegim tants widgets com columnes
        for (int column = 0; column < m_totalColumns; column++)
        {
            newViewer = getNewQ2DViewerWidget();
            m_viewersLayout->addWidget(newViewer, m_rows - 1, column);
            m_vectorViewers.push_back(newViewer);
            if (column >= m_columns)
            {
                newViewer->hide();
                emit viewerRemoved(newViewer);
            }
        }
        rows--;
    }
}

void ViewersLayout::removeColumns(int columns)
{
    int viewerPosition = m_columns - 1;
    Q2DViewerWidget *oldViewer = 0;
    
    while (columns > 0 && m_columns > 1)
    {
        // Eliminem un widget de cada fila per tenir una columna menys
        for (int rows = 0; rows < m_viewersLayout->rowCount(); ++rows)
        {
            oldViewer = m_vectorViewers.at(viewerPosition);
            m_viewersLayout->removeWidget(oldViewer);
            m_vectorViewers.remove(viewerPosition);
            if (m_selectedViewer == oldViewer)
            {
                setSelectedViewer(m_vectorViewers.at(0));
            }
            delete oldViewer;
            viewerPosition += (m_columns - 1);
        }
        m_columns--;
        viewerPosition = m_columns - 1;
        columns--;
    }
}

void ViewersLayout::removeRows(int rows)
{
    int viewerPosition = m_vectorViewers.count() - 1;
    Q2DViewerWidget *oldViewer = 0;

    while (rows > 0 && m_rows > 1)
    {
        // Eliminem tants widgets com columnes
        for (int i = 0; i < m_columns; i++)
        {
            oldViewer = m_vectorViewers.at(viewerPosition);
            m_vectorViewers.remove(viewerPosition);
            m_viewersLayout->removeWidget(oldViewer);
            // TODO eliminar els viewers que treiem del toolManager???
            if (m_selectedViewer == oldViewer)
            {
                setSelectedViewer(m_vectorViewers.at(0));
            }
            delete oldViewer;
            --viewerPosition;
        }
        m_rows--;
        rows--;
    }
}

void ViewersLayout::setGrid(int rows, int columns)
{
    // Mirem si les tenim amagades i mostrem totes les necessaries
    int windowsToShow = 0;
    int windowsToCreate = 0;
	
    if (!m_isRegular)
    {
        restoreLayouts();
    }

    if (rows > m_rows)
    {
        int windowsToHide = m_totalRows - m_rows;

        if (windowsToHide < (rows - m_rows))
        {
            windowsToShow = windowsToHide;
        }
        else
        {
            windowsToShow = rows - m_rows;
        }

        showRows(windowsToShow);

        if (rows > m_totalRows)
        {
            windowsToCreate = rows - m_totalRows;
        }

        addRows(windowsToCreate);
    }
    else if (rows < m_rows)
    {
        hideRows(m_rows - rows);
    }

    windowsToShow = 0;
    windowsToCreate = 0;

    if (columns > m_columns)
    {
        int windowsToHide = m_totalColumns - m_columns;

        if (windowsToHide < (columns - m_columns))
        {
            windowsToShow = windowsToHide;
        }
        else
        {
            windowsToShow = columns - m_columns;
        }

        showColumns(windowsToShow);

        if (columns > m_totalColumns)
        {
            windowsToCreate = columns - m_totalColumns;
        }

        addColumns(windowsToCreate);
    }
    else if (columns < m_columns)
    {
        hideColumns(m_columns - columns);
    }
    m_isRegular = true;
}

void ViewersLayout::setGrid(const QStringList &positionsList)
{
    Q2DViewerWidget *newViewer = 0;
    QStringList listOfPositions;
    QString position;
    int numberOfElements = positionsList.size();
    double x1;
    double y1;
    double x2;
    double y2;
    int screenX = this->width();
    int screenY = this->height();

    m_numberOfVisibleViewers = 0;

    if (m_gridLayout) 
    {
        removeLayouts();
    }

    for (int i = 0; i < numberOfElements; i++)
    {
        newViewer = m_vectorViewers.at(i);

        if (newViewer == 0)
        {
            newViewer = getNewQ2DViewerWidget();
            m_vectorViewers.push_back(newViewer);
        }
        position = positionsList.at(i);
        listOfPositions = position.split("\\");
        x1 = listOfPositions.at(0).toDouble();
        y1 = listOfPositions.at(1).toDouble();
        x2 = listOfPositions.at(2).toDouble();
        y2 = listOfPositions.at(3).toDouble();
        newViewer->setGeometry(x1*screenX, (1 - y1)*screenY, (x2 - x1)*screenX, (y1 - y2)*screenY);
        
        m_numberOfVisibleViewers++;
    }

    m_positionsList = positionsList;
    m_isRegular = false;
}

Q2DViewerWidget* ViewersLayout::addViewer(const QString &position)
{
    Q2DViewerWidget *newViewer = 0;
    QStringList listOfPositions;
    double x1;
    double y1;
    double x2;
    double y2;
    int screenX = this->width();
    int screenY = this->height();

    m_rows = 0;
    m_columns = 0;
    if (m_isRegular) 
    {
        removeLayouts();
    }

    if (m_numberOfVisibleViewers < m_vectorViewers.size())
    {
        newViewer = m_vectorViewers.at(m_numberOfVisibleViewers);
    }
    else
    {
        newViewer = getNewQ2DViewerWidget();
        m_vectorViewers.push_back(newViewer);
    }
	
    listOfPositions = position.split("\\");
    x1 = listOfPositions.at(0).toDouble();
    y1 = listOfPositions.at(1).toDouble();
    x2 = listOfPositions.at(2).toDouble();
    y2 = listOfPositions.at(3).toDouble();
    newViewer->setGeometry(x1*screenX, (1 - y1)*screenY, ((x2 - x1)*screenX), (y1 - y2)*screenY);
    newViewer->show();
    m_numberOfVisibleViewers++;

    emit viewerAdded(newViewer);
    
    m_positionsList << position;
    m_isRegular = false;

    return newViewer;
}

void ViewersLayout::resizeEvent(QResizeEvent *event)
{
    double x1;
    double y1;
    double x2;
    double y2;
    int numberOfElements = m_vectorViewers.size();
    Q2DViewerWidget *viewer = 0;
    QStringList listOfPositions;
    QString position;

    QWidget::resizeEvent(event);

    if (!m_isRegular)
    {
        int screenX = this->width();
        int screenY = this->height();

        for (int i = 0; i < numberOfElements; i++)
        {
            viewer = m_vectorViewers.at(i);
            position = m_positionsList.at(i);
            listOfPositions = position.split("\\");
            x1 = listOfPositions.at(0).toDouble();
            y1 = listOfPositions.at(1).toDouble();
            x2 = listOfPositions.at(2).toDouble();
            y2 = listOfPositions.at(3).toDouble();
            viewer->setGeometry(x1*screenX, (1 - y1)*screenY, (x2 - x1)*screenX, (y1 - y2)*screenY);
        }
    }
}

void ViewersLayout::setSelectedViewer(Q2DViewerWidget *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG("El Viewer donat és NUL!");
        return;
    }

    if (viewer != m_selectedViewer)
    {
        if (m_selectedViewer)
        {
            m_selectedViewer->setSelected(false);
        }
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected(true);
        emit selectedViewerChanged(m_selectedViewer);
    }
}

void ViewersLayout::showRows(int rows)
{
    Q2DViewerWidget *viewer = 0;

    while (rows > 0)
    {
        for (int columnNumber = 0; columnNumber < m_columns; columnNumber++)
        {
            viewer = m_vectorViewers.at((m_totalColumns * m_rows) + columnNumber);
            viewer->show();
        }
        m_rows++;
        rows--;
    }
}

void ViewersLayout::hideRows(int rows)
{
    Q2DViewerWidget *viewer = 0;

    while (rows > 0)
    {
        m_rows--;
        for (int columnNumber = 0; columnNumber < m_columns; columnNumber++)
        {
            viewer = m_vectorViewers.at(((m_totalColumns * m_rows) + columnNumber));
            viewer->hide();
            emit viewerRemoved(viewer);
            if (m_selectedViewer == viewer)
            {
                setSelectedViewer(m_vectorViewers.at(0));
            }
        }
        rows--;
    }
}

void ViewersLayout::showColumns(int columns)
{
    Q2DViewerWidget *viewer = 0;

    while (columns > 0)
    {
        for (int rowNumber = 0; rowNumber < m_rows; rowNumber++)
        {
            viewer = m_vectorViewers.at((m_totalColumns * rowNumber) + m_columns);
            viewer->show();
        }
        m_columns++;
        columns--;
    }
}

void ViewersLayout::hideColumns(int columns)
{
    Q2DViewerWidget *viewer;

    while (columns > 0)
    {
        m_columns--;
        for (int rowNumber = 0; rowNumber < m_rows; rowNumber++)
        {
            viewer = m_vectorViewers.at((m_totalColumns * rowNumber) + m_columns);
            viewer->hide();
            emit viewerRemoved(viewer);
            if (m_selectedViewer == viewer)
            {
                setSelectedViewer(m_vectorViewers.at(0));
            }
        }
        columns--;
    }
}

int ViewersLayout::getNumberOfViewers()
{
    return m_vectorViewers.size();
}

Q2DViewerWidget* ViewersLayout::getViewerWidget(int number)
{
    return m_vectorViewers.at(number);
}

}