/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGQOPTIMALVIEWPOINTEXTENSION_H
#define UDGQOPTIMALVIEWPOINTEXTENSION_H



#include "ui_qoptimalviewpointextensionbase.h"



namespace udg {



class OptimalViewpointDirector;
class OptimalViewpointParameters;
class OptimalViewpointInputParametersForm;

class Volume;



/**
 * ...
 *
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QOptimalViewpointExtension : public QWidget, private ::Ui::QOptimalViewpointExtensionBase {

    Q_OBJECT

public:

    QOptimalViewpointExtension( QWidget * parent = 0 );
    virtual ~QOptimalViewpointExtension();

    void setInput( Volume * input );

private:

    OptimalViewpointDirector * m_optimalViewpointDirector;
    OptimalViewpointParameters * m_optimalViewpointParameters;
    OptimalViewpointInputParametersForm * m_optimalViewpointInputParametersForm;

};

} // end namespace udg



#endif
