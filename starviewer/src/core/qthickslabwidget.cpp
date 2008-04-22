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
    items << tr("Disabled") << tr("MIP") << tr("MinIP") << tr("Average");
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
        disconnect( m_currentViewer, 0, this, 0 );
        disconnect( m_currentViewer, 0, this, 0 );
        disconnect( m_slabThicknessSlider, 0, m_currentViewer, 0 );
    }
    // posem a punt el widget d'acord amb les dades del viewer
    m_currentViewer = viewer;
    if( m_currentViewer->isThickSlabActive() )
    {
        updateMaximumThickness();
        m_slabThicknessSlider->setValue( m_currentViewer->getSlabThickness() );
        m_projectionModeComboBox->setCurrentIndex( m_currentViewer->getSlabProjectionMode()+1 );
    }
    else
    {
        m_slabThicknessSlider->setValue(2);
        applyProjectionMode(0);
        m_projectionModeComboBox->setCurrentIndex(0);
    }

    // creem els vincles
    connect( m_currentViewer, SIGNAL( volumeChanged(Volume *) ), SLOT( reset() ) );
    connect( m_currentViewer, SIGNAL( viewChanged(int) ), SLOT( updateMaximumThickness() ) );
    connect( m_currentViewer, SIGNAL( slabThicknessChanged(int) ), m_slabThicknessSlider, SLOT( setValue(int) ) );
}

void QThickSlabWidget::applyProjectionMode( int comboItem )
{
    emit projectionModeChanged( comboItem );
    QString projectionType = m_projectionModeComboBox->itemText( comboItem );
    if( projectionType == tr("Disabled") )
    {
        //desconnexió nova pel ticket #486
        disconnect( m_slabThicknessSlider, SIGNAL( sliderReleased () ), this, SLOT( applyThickSlab() ) );
        
        //desconnexió antiga
        //disconnect( m_slabThicknessSlider, SIGNAL( valueChanged(int) ), m_currentViewer, SLOT( setSlabThickness(int) ) );
        m_currentViewer->enableThickSlab(false);
        m_slabThicknessSlider->setEnabled(false);
        m_slabThicknessLabel->setEnabled(false);
        // TODO això s'hauria de fer automàticament quan tenim slab thickness d'1. Cal repassar bé tot el pipeline del Q2DViewer
        m_currentViewer->updateSliceAnnotationInformation();
    }
    else
    {
        m_currentViewer->enableThickSlab(true);
        m_slabThicknessSlider->setEnabled(true);
        m_slabThicknessLabel->setEnabled(true);
        // TODO es podria fer l'actualització de l'slab quan es deixa d'interactuar ( sliderReleased() )si és que fer-ho
        // "en viu" afecta molt al temps de resposta amb l'interacció. De moment ens atrevim a fer-ho "en viu"

        //connexió antiga
        //connect( m_slabThicknessSlider, SIGNAL( valueChanged(int) ), m_currentViewer, SLOT( setSlabThickness(int) ) );
        
        //connexió nova pel ticket #486
        connect( m_slabThicknessSlider, SIGNAL( sliderReleased () ),this,  SLOT( applyThickSlab() ) );
        
        // TODO ara fem la conversió a id d'enter, però en un futur anirà tot amb Strings
        int projectionModeID = -1;
        if( projectionType == tr("MIP") )
        {
            projectionModeID = 0;
        }
        else if( projectionType == tr("MinIP") )
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

void QThickSlabWidget::applyThickSlab()
{
    ///\TODO per a donar sensació d'espera, canviem el cursor abans d'aplicar el thickslab i el restaurem quan s'acaba el procés. S'hauria de fer de manera més centralizada per tal de que si es crida des de qualsevol lloc, es facin aquestes accions sobre el cursor, és a dir, que no calgui programar això en cada lloc on s'apliqui thickslab.
    QApplication::setOverrideCursor( Qt::WaitCursor );
    m_currentViewer->setSlabThickness( m_slabThicknessSlider->value() );
    QApplication::restoreOverrideCursor();
}

void QThickSlabWidget::updateMaximumThickness()
{
    m_slabThicknessSlider->setRange( 2, m_currentViewer->getMaximumSlice() + 1 );
}

void QThickSlabWidget::updateThicknessLabel(int value)
{
    m_slabThicknessLabel->setText( QString::number( value ) );
}

void QThickSlabWidget::reset()
{
    m_currentViewer->enableThickSlab(false);
    m_currentViewer->setSlabThickness( 1 );
    this->link( m_currentViewer );
}

}
