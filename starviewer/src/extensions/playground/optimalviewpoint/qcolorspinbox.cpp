/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qcolorspinbox.h"


namespace udg {


QColorSpinBox::QColorSpinBox( QWidget * parent )
    : QSpinBox( parent )
{
    m_settingColor = false;

    setMaximum( 255 );
    setColor( QColor( 255, 255, 255, 0 ) );

    connect( this, SIGNAL( valueChanged(int) ), SLOT( setOpacity(int) ) );
}


QColorSpinBox::~QColorSpinBox()
{
}


const QColor & QColorSpinBox::getColor() const
{
    return m_color;
}


void QColorSpinBox::setColor( const QColor & color )
{
    m_settingColor = true;
    m_color = color;
    QString foreground = QString( ";color:" ) + ( color.value() < 128 ? "white" : "black" );
    this->setStyleSheet( QString( "background-color:" ) + color.name() + foreground );
    this->setValue( color.alpha() );
    emit colorChanged( m_color );
    m_settingColor = false;
}


void QColorSpinBox::setOpacity( int opacity )
{
    m_color.setAlpha( opacity );
    if ( !m_settingColor ) emit colorChanged( m_color );
}


}
