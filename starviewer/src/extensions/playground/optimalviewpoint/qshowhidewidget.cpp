/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qshowhidewidget.h"

namespace udg {

QShowHideWidget::QShowHideWidget(QWidget *parent)
 : QWidget(parent)
{
    m_inited = false;
    setupUi( this );
    m_inited = true;
    m_frameLayout = 0;
}


QShowHideWidget::~QShowHideWidget()
{
}


QString QShowHideWidget::title() const
{
    return m_checkBox->text();
}


void QShowHideWidget::setTitle( const QString & title )
{
    m_checkBox->setText( title );
}


void QShowHideWidget::childEvent( QChildEvent * event )
{
    if ( !( m_inited && event->added() ) ) return;

    QWidget * child = qobject_cast<QWidget *>( event->child() );
    if ( child )
    {
        if ( m_frameLayout )
        {
            QLayout * layout = qobject_cast<QLayout *>( m_frameLayout );
            layout->setParent( m_frame );
            m_frame->setLayout( layout );
            m_frameLayout = 0;
        }

        child->setParent( m_frame );
        child->show();
        return;
    }

    m_frameLayout = event->child();
}


}
