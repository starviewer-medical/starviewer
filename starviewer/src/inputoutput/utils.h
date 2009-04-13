/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UTILS
#define UTILS

namespace udg {

/**
Classe que conté utilitats vàries utilitzades per la classe inputoutput

*/
class Utils{

public:

    ///Indica si el port passat per paràmetre està en ús
    static bool isPortInUse(int port);

private:


};

};  //  end  namespace udg

#endif
