/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcustomwindowleveldialog.h"
#include "logging.h"

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

void QCustomWindowLevelDialog::setDefaultWindowLevel( double window , double level )
{
    m_windowSpinBox->setValue( window );
    m_levelSpinBox->setValue( level );
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
        DEBUG_LOG("Falta validar el valor del window");
    }
    if( m_levelSpinBox->value() )
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del level");
    }

    emit windowLevel( m_windowSpinBox->value() , m_levelSpinBox->value() );
    this->close();
}

}
