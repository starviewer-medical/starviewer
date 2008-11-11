#ifndef UDGRENDERINGSTYLE_H
#define UDGRENDERINGSTYLE_H


#include "transferfunction.h"


namespace udg {


/**
 * Classe que encapsula tots els paràmetres de rendering.
 */
class RenderingStyle {

public:

    QVariant toVariant() const;
    static RenderingStyle fromVariant( const QVariant &variant );

    // Atributs públics
    bool diffuseLighting;
    bool specularLighting;
    double specularPower;
    TransferFunction transferFunction;

};


}


#endif
