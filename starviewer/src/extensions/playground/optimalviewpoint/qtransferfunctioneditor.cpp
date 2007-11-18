/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qtransferfunctioneditor.h"


namespace udg {


QTransferFunctionEditor::QTransferFunctionEditor( QWidget * parent )
    : QWidget( parent )
{
    m_maximum = 255;
}


QTransferFunctionEditor::~QTransferFunctionEditor()
{
}


unsigned short QTransferFunctionEditor::maximum() const
{
    return m_maximum;
}


void QTransferFunctionEditor::setMaximum( unsigned short maximum )
{
    m_maximum = maximum;
}


}
