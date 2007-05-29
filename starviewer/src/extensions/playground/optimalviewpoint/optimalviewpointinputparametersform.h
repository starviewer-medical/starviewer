/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTINPUTPARAMETERSFORM_H
#define UDGOPTIMALVIEWPOINTINPUTPARAMETERSFORM_H

#include "qinputparameters.h"
#include "ui_optimalviewpointinputparametersformbase.h"

#include "optimalviewpoint.h"

namespace udg {

class OptimalViewpointParameters;
class Volume;

/**
 * Interfície per definir tots els paràmetres del mètode de visualització del
 * Punt de Vista Òptim.
 */
class OptimalViewpointInputParametersForm
    : public udg::QInputParameters, public Ui::OptimalViewpointInputParametersFormBase
{

    Q_OBJECT

public:

    OptimalViewpointInputParametersForm( QWidget * parent = 0 );
    virtual ~OptimalViewpointInputParametersForm();

    /// Assigna l'objecte que guardarà els paràmetres.
    void setParameters( OptimalViewpointParameters * parameters );

public slots:

    /**
     * Slot que ens serveix per indicar que hem d'actualitzar el paràmetre que
     * ens diguin mitjançant un identificador (que, en realitat, serà un enum).
     * Serveix per canviar els valors a partir d'una classe Parameters.
     */
    virtual void readParameter( int index );

    /**
     * Escriu tots els valors de paràmetres que té actualment al Parameters
     * associat.
     */
    virtual void writeAllParameters();

    /// Assigna la funció de transferència actual.
    void setTransferFunction( const QGradientStops & stops );

protected:

    /**
     * Mètode reimplementat per inicialitzar la funció de transferència quan es
     * mostra el widget per primer cop. Si no es fa així hi ha problemes amb el
     * GradientEditor.
     */
    virtual void showEvent( QShowEvent * event );

private:

    /// Objecte que guardarà els paràmetres.
    OptimalViewpointParameters * m_parameters;

    /// Identificador del volum tractat.
//     Identifier m_volumeId;

    /// Funció de transferència actual.
    OptimalViewpoint::TransferFunction m_transferFunction;

    /// Cert si s'ha inicialitzat la funció de transferència i fals altrament.
    bool m_inited;

private slots:

    /**
     * Mostra el quadre de diàleg que permet seleccionar el volum que es farà
     * servir.
     */
//     void selectVolume();
    void setAdjustedTransferFunction( const OptimalViewpoint::TransferFunction & adjustedTransferFunction );
    void setNumberOfPlanes( const QString & numberOfPlanes );
    void openSegmentationFile();
    void toggleSegmentationParametersPushButtonText( bool checked );

signals:

    void executionRequested();

}; // end class OptimalViewpointInputParametersForm

}; // end namespace udg

#endif // UDGOPTIMALVIEWPOINTINPUTPARAMETERSFORM_H
