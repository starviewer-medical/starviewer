#ifndef UDGPORTINUSE_H
#define UDGPORTINUSE_H

#include <QString>
#include <QAbstractSocket>

namespace udg {

class PortInUseByAnotherApplication;

/**
    Classe que s'encarrega de comprovar si un port està en ús.
  */
class PortInUse {
public:
    enum PortInUseStatus { PortIsAvailable, PortIsInUse, PortCheckError, PortUnknownStatus };
    enum PortInUseOwner { PortUsedByUnknown, PortUsedByStarviewer, PortUsedByOther };

    PortInUse();

    /// Indica si el port passat per paràmetre està en ús (cert) o lliure (fals)
    bool isPortInUse(int port);

    /// Indica si el port passat per paràmetre està en ús per una aplicació diferent de starviewer
    PortInUse::PortInUseOwner getOwner();

    /// Retorna l'estat de l'últim port que s'ha comprovat
    PortInUse::PortInUseStatus getStatus();

    /// Retorna l'string corresponent a l'error. El seu valor només serà vàlid quan m_status valgui PortCheckError.
    QString getErrorString();

protected:
    /// Retorna si el port passat per paràmetre està lliure (cert) o en ús (fals)
    /// @param serverError: indica l'error del servidor
    /// @param errorString: descripcio de l'error.
    virtual bool isPortAvailable(int port, QAbstractSocket::SocketError &serverError, QString &errorString);
    /// Mètode per aplicar testing, crea un objecte portInUseByAnotherApplication
    virtual PortInUseByAnotherApplication* createPortInUseByAnotherApplication();

protected:
    /// Últim port que s'ha comprovat, quan fem un getOwner, es farà d'aquest port
    int m_lastPortChecked;
    /// Estat del port
    PortInUse::PortInUseStatus m_status;
    /// String amb la descripció de l'error en cas que se n'hagi produït algun
    QString m_errorString;

};

}  // end namespace udg

#endif
