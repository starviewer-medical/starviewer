/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQSEGMENTATIONFRAMEWORK_H
#define UDGQSEGMENTATIONFRAMEWORK_H

#include "ui_qsegmentationframeworkextensionbase.h"

namespace udg {

// FWD declarations
class Volume;
class SegmentationFrameworkMethod;
class ToolsActionFactory;

/**

	@author Grup de GrÀics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QSegmentationFrameworkExtension : public QWidget , private ::Ui::QSegmentationFrameworkExtensionBase {
Q_OBJECT
public:
    QSegmentationFrameworkExtension( QWidget *parent = 0 );

    ~QSegmentationFrameworkExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );
private:

    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentaciÓ
    Volume *m_maskVolume;
};

} // end namespace udg

#endif
