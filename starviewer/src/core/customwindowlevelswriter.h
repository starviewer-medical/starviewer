#ifndef UDGCUSTOMWINDOWLEVELSWRITER_H
#define UDGCUSTOMWINDOWLEVELSWRITER_H

#include "customwindowlevel.h"

namespace udg {

/**
    Classe encarregada de llegir els custom window levels del repository
    i crear un fitxer XML per guardar-los.
*/

class CustomWindowLevelsWriter {

public:
    CustomWindowLevelsWriter();

    ~CustomWindowLevelsWriter();

    /// Mètode que crea i escriu un fitxer XML per guardar els custom window levels
    void write();
        
private:
    
    // Retorna el directori on s'escriurà el fitxer XML (customwindowlevels.xml)
    QString getPath();


private:

};

}

#endif 
