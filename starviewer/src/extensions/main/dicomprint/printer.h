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
    virtual void setName( const QString& name )=0;

    /// Retorna el nom de la impressora.
    virtual QString getName() const =0;

    /// Posa el tipus de la impressora.
    virtual void setType( const QString& type )=0;

    /// Retorna el tipus de la impressora.
    virtual QString getType() const =0;

    /// Posa la descripció de la impressora.
    virtual void setDescription( const QString& description )=0;

    /// Retorna la descripció de la impressora.
    virtual QString getDescription() const=0;

    /// Posa si és la impressora per defecte
    virtual void setIsDefault( bool isDefault )=0;

    // Retorna si és la impressora per defecte.
    virtual bool getIsDefault() const=0;
    
protected:    
    
    QString m_name;
    QString m_type;
    QString m_description;
    bool m_isDefaultPrinter;
    
};
} 

#endif
