#ifndef UDGPORTINUSEBYANOTHERAPPLICATION_H
#define UDGPORTINUSEBYANOTHERAPPLICATION_H

namespace udg {

/**
    Classe que s'encarrega de comprovar si un port està en ús per una aplicació que no sigui starviewer.
  */
class PortInUseByAnotherApplication {
public:
    virtual ~PortInUseByAnotherApplication();

    /// Crea una nova instància d'alguna de les subclasses que implementa la interfície
    static PortInUseByAnotherApplication* newInstance();

    /// Comprova si el port està en ús per una aplicació que no sigui Starviewer
    virtual bool isPortInUseByAnotherApplication(int port, bool &error);

protected:
    // Constructor privat per obligar a utilitzar el mètode newInstance
    PortInUseByAnotherApplication();
};

}  // end namespace udg

#endif
