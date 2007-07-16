/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qsegmentationframeworkextension.h"
#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"

namespace udg {

QSegmentationFrameworkExtension::QSegmentationFrameworkExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;
    m_maskVolume = new Volume();
}

QSegmentationFrameworkExtension::~QSegmentationFrameworkExtension()
{
}

void QSegmentationFrameworkExtension::setInput( Volume *input )
{
    m_mainVolume = input;
}

}
