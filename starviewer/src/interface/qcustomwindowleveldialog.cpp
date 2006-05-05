/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcustomwindowleveldialog.h"

namespace udg {

QCustomWindowLevelDialog::QCustomWindowLevelDialog( QDialog *parent )
 : QDialog( parent )
{
    setupUi( this );
    createConnections();
}

QCustomWindowLevelDialog::~QCustomWindowLevelDialog()
{
}

void QCustomWindowLevelDialog::createConnections()
{
    connect( m_okButton , SIGNAL( clicked() ) , this , SLOT( confirmWindowLevel() ) );
    connect( m_cancelButton , SIGNAL( clicked() ) , this , SLOT( close() ) );
}

void QCustomWindowLevelDialog::confirmWindowLevel()
{
    // validar els spin box
    if( m_windowSpinBox->value() )
    {
        // \TODO implement me!
    }
    if( m_levelSpinBox->value() )
    {
        // \TODO implement me!
    }

    emit windowLevel( m_windowSpinBox->value() , m_levelSpinBox->value() );
    this->close();
}

}
