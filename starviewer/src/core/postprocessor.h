#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

namespace udg {

class Volume;

/**
    Classe base per tots els postprocessadors que s'executen després de la lectura del volum.
 */
class Postprocessor {

public:

    /// Aplica el postprocés sobre el volum donat.
    virtual void postprocess(Volume *volume) = 0;

};

} // namespace udg

#endif // POSTPROCESSOR_H
