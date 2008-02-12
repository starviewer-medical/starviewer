/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qthickslabwidget.h"

#include "q2dviewer.h"
#include "volume.h"

namespace udg {

QThickSlabWidget::QThickSlabWidget(QWidget *parent)
 : QWidget(parent), m_currentViewer(0)
{
    setupUi(this);
    // omplim el combo amb els valors que volem
    m_projectionModeComboBox->clear();
    QStringList items;
    items << tr("Inactive") << tr("Maximum Intensity Projection (MIP)") << tr("Minimum Intensity Projection (MinIP)") << tr("Average");
    m_projectionModeComboBox->addItems( items );
}

QThickSlabWidget::~QThickSlabWidget()
{
}

void QThickSlabWidget::link( Q2DViewer *viewer )
{
    if( !viewer )
    {
        DEBUG_LOG("Al tanto, ens passen un viewer NUL! Sortida immediata!");
        return;
    }

    if( !m_currentViewer )
    {
        m_currentViewer = viewer;
        // si no teníem cap viewer fins ara creem les connexions necessàries
        connect( m_projectionModeComboBox, SIGNAL( currentIndexChanged(int) ), SLOT( applyProjectionMode(int) ) );
        connect( m_slabThicknessSlider, SIGNAL( valueChanged(int) ), SLOT( updateThicknessLabel(int) ) );
    }
    else
    {
        // primer deslinkem qualsevol altre viewer que tinguéssim linkat anteriorment
        disconnect( m_currentViewer, 0, 0, 0 );
        disconnect( m_slabThicknessSlider, 0, m_currentViewer, 0 );
    }
    // posem a punt el widget d'acord amb les dades del viewer
    m_currentViewer = viewer;
    if( m_currentViewer->isThickSlabActive() )
    {
        m_slabThicknessSlider->setValue( m_currentViewer->getSlabThickness() );
        updateMaximumThickness();
        applyProjectionMode( m_currentViewer->getSlabProjectionMode()+1 );
    }
    else
    {
        m_slabThicknessSlider->setRange(2,2);
        m_slabThicknessSlider->setValue(2);
        applyProjectionMode(0);
    }

    // creem els vincles
    connect( m_currentViewer, SIGNAL( viewChanged(int) ), SLOT( updateMaximumThickness() ) );
    connect( m_currentViewer, SIGNAL( slabThicknessChanged(int) ), m_slabThicknessSlider, SLOT( setValue(int) ) );
    // TODO es podria fer l'actualització de l'slab quan es deixa d'interactuar ( sliderReleased() )si és que fer-ho
    // "en viu" afecta molt al temps de resposta amb l'interacció. De moment ens atrevim a fer-ho "en viu"
    connect( m_slabThicknessSlider, SIGNAL( valueChanged(int) ), m_currentViewer, SLOT( setSlabThickness(int) ) );
}

void QThickSlabWidget::applyProjectionMode( int comboItem )
{
    QString projectionType = m_projectionModeComboBox->itemText( comboItem );
    if( projectionType == tr("Inactive") )
    {
        m_currentViewer->enableThickSlab(false);
        m_slabThicknessSlider->setEnabled(false);
        m_slabThicknessLabel->setEnabled(false);
    }
    else
    {
        m_currentViewer->enableThickSlab(true);
        m_slabThicknessSlider->setEnabled(true);
        m_slabThicknessLabel->setEnabled(true);
        // TODO ara fem la conversió a id d'enter, però en un futur anirà tot amb Strings
        int projectionModeID = -1;
        if( projectionType == tr("Maximum Intensity Projection (MIP)") )
        {
            projectionModeID = 0;
        }
        else if( projectionType == tr("Minimum Intensity Projection (MinIP)") )
        {
            projectionModeID = 1;
        }
        else if( projectionType == tr("Average") )
        {
            projectionModeID = 2;
        }
        m_currentViewer->setSlabProjectionMode( projectionModeID );
        m_currentViewer->setSlabThickness( m_slabThicknessSlider->value() );
    }
}

void QThickSlabWidget::updateMaximumThickness()
{
    m_slabThicknessSlider->setRange( 2, m_currentViewer->getMaximumSlice() + 1 );
}

void QThickSlabWidget::updateThicknessLabel(int value)
{
    m_slabThicknessLabel->setText( QString::number( value ) );
}

}
