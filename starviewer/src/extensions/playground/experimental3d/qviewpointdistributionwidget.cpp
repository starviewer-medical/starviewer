#include "qviewpointdistributionwidget.h"

#include <cmath>


namespace udg {


QViewpointDistributionWidget::QViewpointDistributionWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    createConnections();
}


QViewpointDistributionWidget::~QViewpointDistributionWidget()
{
}


bool QViewpointDistributionWidget::isUniform() const
{
    return m_uniformRadioButton->isChecked();
}


unsigned int QViewpointDistributionWidget::numberOfViewpoints() const
{
    if ( isUniform() ) return m_uniformNumberOfViewpointsComboBox->currentText().toUInt();
    else return numberOfQuasiUniformViewpoints();
}


unsigned int QViewpointDistributionWidget::recursionLevel() const
{
    return m_quasiUniformRecursionLevelSpinBox->value();
}


void QViewpointDistributionWidget::createConnections()
{
    connect( m_uniformRadioButton, SIGNAL( toggled(bool) ), m_uniformNumberOfViewpointsLabel, SLOT( setEnabled(bool) ) );
    connect( m_uniformRadioButton, SIGNAL( toggled(bool) ), m_uniformNumberOfViewpointsComboBox, SLOT( setEnabled(bool) ) );

    connect( m_quasiUniformRadioButton, SIGNAL( toggled(bool) ), m_quasiUniformRecursionLevelLabel, SLOT( setEnabled(bool) ) );
    connect( m_quasiUniformRadioButton, SIGNAL( toggled(bool) ), m_quasiUniformRecursionLevelSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_quasiUniformRadioButton, SIGNAL( toggled(bool) ), m_quasiUniformNumberOfViewpointsLabel, SLOT( setEnabled(bool) ) );

    connect( m_uniformRadioButton, SIGNAL( toggled(bool) ), SLOT( updateNumberOfViewpoints() ) );
    connect( m_uniformNumberOfViewpointsComboBox, SIGNAL( currentIndexChanged(int) ), SLOT( updateNumberOfViewpoints() ) );
    connect( m_quasiUniformRecursionLevelSpinBox, SIGNAL( valueChanged(int) ), SLOT( updateNumberOfViewpoints() ) );
}


unsigned int QViewpointDistributionWidget::numberOfQuasiUniformViewpoints() const
{
    return static_cast<unsigned int>( 10.0 * pow( 4.0, recursionLevel() ) + 2.0 );
}


void QViewpointDistributionWidget::updateNumberOfViewpoints()
{
    m_quasiUniformNumberOfViewpointsLabel->setText( QString( tr("%1 viewpoints") ).arg( numberOfQuasiUniformViewpoints() ) );
    emit numberOfViewpointsChanged( numberOfViewpoints() );
}


}
