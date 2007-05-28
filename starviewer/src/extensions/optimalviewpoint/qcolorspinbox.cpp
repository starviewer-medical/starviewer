/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcolorspinbox.h"

QColorSpinBox::QColorSpinBox(QWidget *parent)
 : QSpinBox(parent)
{
    setColor( QColor( 0, 0, 0, 0 ) );

    connect( this, SIGNAL( valueChanged(int) ), this, SLOT( setOpacity(int) ) );
}


QColorSpinBox::~QColorSpinBox()
{
}


QColor QColorSpinBox::getColor() const
{
    return m_color;
}


void QColorSpinBox::setColor( const QColor & color )
{
    m_color = color;
    QString foreground = QString( ";color:" ) + ( color.value() < 128 ? "white" : "black" );
    this->setStyleSheet( QString( "background-color:" ) + color.name() + foreground );
    this->setValue( color.alpha() );
}


void QColorSpinBox::setOpacity( int opacity )
{
    if ( opacity != m_color.alpha() ) m_color.setAlpha( opacity );
}
