#ifndef UDGPRINTER_H
#define UDGPRINTER_H

#include <QString>

/**
* Classe abstracte que representa una Impressora.
* Tota impressora (Dicom, Paper...) ha d'heretà d'aquesta classe.
*/

namespace udg {

class Printer 
{	
public:
        
    /// Posa el nom de la impressora.
    virtual void setName(const QString &_name)=0;
    
    /// Posa el HostName (IP) de la impressora.
    virtual void setHostname(const QString &_hostname)=0;

    /// Posa el Port de la impressora.
    virtual void setPort(const int &_port)=0;

    /// Posa el tipus de la impressora.
    virtual void setType(const QString &_type)=0;
    
    /// Posa la descripció de la impressora.
    virtual void setDescription(const QString &_description)=0;

    /// Retorna el nom de la impressora.
    virtual QString getName() const =0;

    /// Retorna el HostName (IP) de la impressora.
    virtual QString getHostname() const =0;

    /// Retorna el port de la impressora.
    virtual int getPort() const =0;

    /// Retorna el tipus de la impressora.
    virtual QString getType() const =0;

    /// Retorna la descripció de la impressora.
    virtual QString getDescription() const=0;
    
protected:    
    
    QString m_name;
    QString m_hostname;
    int     m_port;
    QString m_type;
    QString m_description;
    
};
} 

#endif
