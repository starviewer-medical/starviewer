#include "qviewpointdistributionwidget.h"

#include <cmath>

#include "viewpointgenerator.h"


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


int QViewpointDistributionWidget::recursionLevel() const
{
    return m_quasiUniformRecursionLevelSpinBox->value();
}


QVector<Vector3> QViewpointDistributionWidget::viewpoints() const
{
    ViewpointGenerator viewpointGenerator;

    if ( isUniform() )
    {
        switch ( numberOfViewpoints() )
        {
            case 4: viewpointGenerator.setToUniform4(); break;
            case 6: viewpointGenerator.setToUniform6(); break;
            case 8: viewpointGenerator.setToUniform8(); break;
            case 12: viewpointGenerator.setToUniform12(); break;
            case 20: viewpointGenerator.setToUniform20(); break;
            default: Q_ASSERT_X( false, "viewpoints", qPrintable( QString( "Nombre de punts de vista uniformes incorrecte: %1" ).arg( numberOfViewpoints() ) ) );
        }
    }
    else viewpointGenerator.setToQuasiUniform( recursionLevel() );

    return viewpointGenerator.viewpoints();
}


ViewpointGenerator QViewpointDistributionWidget::viewpointGenerator( float radius ) const
{
    ViewpointGenerator viewpointGenerator;

    if ( isUniform() )
    {
        switch ( numberOfViewpoints() )
        {
            case 4: viewpointGenerator.setToUniform4( radius ); break;
            case 6: viewpointGenerator.setToUniform6( radius ); break;
            case 8: viewpointGenerator.setToUniform8( radius ); break;
            case 12: viewpointGenerator.setToUniform12( radius ); break;
            case 20: viewpointGenerator.setToUniform20( radius ); break;
            default: Q_ASSERT_X( false, "viewpoints", qPrintable( QString( "Nombre de punts de vista uniformes incorrecte: %1" ).arg( numberOfViewpoints() ) ) );
        }
    }
    else viewpointGenerator.setToQuasiUniform( recursionLevel(), radius );

    return viewpointGenerator;
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
