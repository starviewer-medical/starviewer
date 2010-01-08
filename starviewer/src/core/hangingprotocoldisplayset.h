/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLDISPLAYSET_H
#define UDGHANGINGPROTOCOLDISPLAYSET_H

#include <QObject>

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolDisplaySet : public QObject
{
Q_OBJECT
public:
    HangingProtocolDisplaySet(QObject *parent = 0);

    ~HangingProtocolDisplaySet();

    /// Posar l'identificador
    void setIdentifier( int identifier );

    /// Posar la descripcio
    void setDescription( QString description );

    /// Posar l'image set a que referencia
    void setImageSetNumber( int number );

    /// Posar la posició del visualitzador
    void setPosition( QString position );

    /// Posar la posició del pacient
    void setPatientOrientation( QString string );

    /// Posar la reconstruccio (axial, sagital, coronal)
    void setReconstruction( QString reconstruction );

    /// Posar la fase
    void setPhase( QString phase );

    /// Obtenir l'identificador
    int getIdentifier() const;

    /// Obtenir la descripcio
    QString getDescription() const;

    /// Obtenir l'image set a que referencia
    int getImageSetNumber() const;

    /// Obtenir la posició del visualitzador
    QString getPosition() const;

    /// Obtenir la posició del pacient
    QString getPatientOrientation() const;

    /// Obtenir la reconstruccio
    QString getReconstruction() const;

    /// Obtenir la fase
    QString getPhase() const;

    /// Mètode per mostrar els valors
    void show();
    
    /// Posar la llesca a mostrar
    void setSlice( int sliceNumber );

    /// Obtenir la llesca a mostrar
    int getSlice();
    
    /// Assigna el tipus d'icona per representar-lo
    void setIconType( QString iconType );
    
    /// Obté el tipus d'icona per representar-lo
    QString getIconType() const;

    /// Assigna cap a quin costat ha d'estar alineada la imatge
    void setAlignment( QString alignment );
    
    /// Obté el costat que s'ha d'alinear la imatge
    QString getAlignment() const;

    /// Assigna l'eina a activar al visualitzador
    void setToolActivation( QString toolActive );
    
    /// Obté l'eina a activar al visualitzador
    QString getToolActivation();

private:

    /// Identificador de la seqüència
    int m_identifier;

    /// Descripció
    QString m_description;

    /// Image set que referencia
    int m_imageSetNumber;

    /// Posició a on es troba
    QString m_position;

    /// Orientacio del pacient
    QString m_patientOrientation;

    /// Reconstruccio
    QString m_reconstruction;

    /// Fase
    QString m_phase;

    /// Llesca
    int m_sliceNumber;

    /// Indica el tipus d'icona per representar el hanging protocol
    QString m_iconType;

    /// Indica si la imatge ha d'estar alineada en algun costat
    QString m_alignment;

    /// Indica la tool a activar
    QString m_activateTool;
};

}

#endif
