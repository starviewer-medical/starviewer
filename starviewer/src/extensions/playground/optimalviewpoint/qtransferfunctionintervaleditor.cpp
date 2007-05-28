/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qtransferfunctionintervaleditor.h"

#include <QColorDialog>

namespace udg {

QTransferFunctionIntervalEditor::QTransferFunctionIntervalEditor( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    m_isFirst = m_isLast = false;

    m_intervalEndSpinBox->setVisible( m_isIntervalCheckBox->isChecked() );

    connect( m_isIntervalCheckBox, SIGNAL( toggled(bool) ), this, SLOT( isIntervalToggled(bool) ) );
    connect( m_intervalStartSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( adjustWithNewStart(int) ) );
    connect( m_intervalEndSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( adjustWithNewEnd(int) ) );
    connect( m_intervalStartSpinBox, SIGNAL( valueChanged(int) ), this, SIGNAL( startChanged(int) ) );
    connect( m_intervalEndSpinBox, SIGNAL( valueChanged(int) ), this, SIGNAL( endChanged(int) ) );
    connect( m_selectColorPushButton, SIGNAL( clicked() ), this, SLOT( selectColor() ) );
}


QTransferFunctionIntervalEditor::~QTransferFunctionIntervalEditor()
{
}


void QTransferFunctionIntervalEditor::setIsInterval( bool isInterval )
{
    m_isIntervalCheckBox->setChecked( isInterval );
}


void QTransferFunctionIntervalEditor::setIsFirst( bool isFirst )
{
    m_isFirst = isFirst;
    if ( m_isFirst && m_isLast ) firstAndLast();
    else
    {
        m_isIntervalCheckBox->setEnabled( true );
        if ( m_isFirst ) this->setStart( 0 );
        m_intervalStartSpinBox->setReadOnly( m_isFirst );
    }
}


void QTransferFunctionIntervalEditor::setIsLast( bool isLast )
{
    m_isLast = isLast;
    if ( m_isFirst && m_isLast ) firstAndLast();
    else
    {
        m_isIntervalCheckBox->setEnabled( true );
        if ( m_isLast ) this->setEnd( 255 );
        m_intervalEndSpinBox->setReadOnly( m_isLast );
        if ( !m_isIntervalCheckBox->isChecked() )
        {
            if ( m_isLast ) this->setStart( 255 );
            m_intervalStartSpinBox->setReadOnly( m_isLast );
        }
    }
}


int QTransferFunctionIntervalEditor::start() const
{
    return m_intervalStartSpinBox->value();
}


int QTransferFunctionIntervalEditor::end() const
{
    return m_intervalEndSpinBox->value();
}


QColor QTransferFunctionIntervalEditor::color() const
{
    return m_colorSpinBox->getColor();
}


void QTransferFunctionIntervalEditor::setStart( int start )
{
    m_intervalStartSpinBox->setValue( start );
    emit startChanged( this->start() );
}


void QTransferFunctionIntervalEditor::setEnd( int end )
{
    m_intervalEndSpinBox->setValue( end );
    emit endChanged( this->end() );
}


void QTransferFunctionIntervalEditor::setColor( QColor color )
{
    m_colorSpinBox->setColor( color );
}


QGradientStops QTransferFunctionIntervalEditor::getInterval() const
{
    //typedef QPair<qreal,QColor> QGradientStop;
    //typedef QVector<QGradientStop> QGradientStops;
    QGradientStops interval;
    interval << QGradientStop( start() / 255.0, m_colorSpinBox->getColor() );
    if ( m_isIntervalCheckBox->isChecked() )
        interval << QGradientStop( end() / 255.0, m_colorSpinBox->getColor() );
    return interval;
}


void QTransferFunctionIntervalEditor::setPreviousEnd( int previousEnd )
{
    if ( previousEnd >= this->start() ) this->setStart( previousEnd + 1 );
}


void QTransferFunctionIntervalEditor::setNextStart( int nextStart )
{
    if ( nextStart <= this->end() ) this->setEnd( nextStart - 1 );
}


void QTransferFunctionIntervalEditor::isIntervalToggled( bool checked )
{
    if ( !checked )
        this->setEnd( this->start() );
    if ( m_isLast )
    {
        if ( !checked ) this->setStart( 255 );
        m_intervalStartSpinBox->setReadOnly( !checked );
    }
}


void QTransferFunctionIntervalEditor::adjustWithNewStart( int start )
{
    if ( !m_isIntervalCheckBox->isChecked() || start > this->end() )
        this->setEnd( start );
}


void QTransferFunctionIntervalEditor::adjustWithNewEnd( int end )
{
    if ( end < this->start() )
        this->setStart( end );
}


void QTransferFunctionIntervalEditor::selectColor()
{
    QColor color = this->color();
    bool ok;
    color = QColor::fromRgba( QColorDialog::getRgba( color.rgba(), &ok, this ) );
    if ( ok ) this->setColor( color );
}


void QTransferFunctionIntervalEditor::firstAndLast()
{
    m_isIntervalCheckBox->setChecked( true );
    m_isIntervalCheckBox->setDisabled( true );
    this->setStart( 0 );
    m_intervalStartSpinBox->setReadOnly( true );
    this->setEnd( 255 );
    m_intervalEndSpinBox->setReadOnly( true );
}


}
