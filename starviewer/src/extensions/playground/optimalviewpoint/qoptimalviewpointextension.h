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


class OptimalViewpoint;
// class OptimalViewpointDirector;
// class OptimalViewpointInputParametersForm;
class OptimalViewpointParameters;
class OptimalViewpointViewer;

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

public slots:

    /// Carrega la segmentació des d'un fitxer.
    void loadSegmentation();
    /// Fa una segmentació automàtica.
    void automaticSegmentation();

    /**
     * Aquest mètode aplica el mètode de visualització. Comprova primer si té
     * els paràmetres necessaris, sinó no fa res.
     */
    void execute();

private:

//     OptimalViewpointDirector * m_optimalViewpointDirector;
    OptimalViewpointParameters * m_parameters;
//     OptimalViewpointInputParametersForm * m_optimalViewpointInputParametersForm;

    /// Mètode.
    OptimalViewpoint * m_method;

    /// Visualitzador principal.
    OptimalViewpointViewer * m_viewer;

};

} // end namespace udg



#endif
