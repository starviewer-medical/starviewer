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

class HangingProtocol;
class HangingProtocolImageSet;

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

    /// Assignar el seu image set
    void setImageSet(HangingProtocolImageSet *imageSet);

    /// Obtenir el seu image set
    HangingProtocolImageSet* getImageSet() const ;

    /// Assignar el hanging protocol al que pertany
    void setHangingProtocol(HangingProtocol *hangingProtocol);

    /// Obtenir el hanging protocol al que pertany
    HangingProtocol* getHangingProtocol() const ;

    /// Posar la posició del visualitzador
    void setPosition( QString position );

    /// Posar la posició del pacient
    void setPatientOrientation( QString string );

    /// Posar la reconstruccio (axial, sagital, coronal)
    void setReconstruction( QString reconstruction );

    /// Posar la fase
    void setPhase( int phase );

    /// Obtenir l'identificador
    int getIdentifier() const;

    /// Obtenir la descripcio
    QString getDescription() const;

    /// Obtenir la posició del visualitzador
    QString getPosition() const;

    /// Obtenir la posició del pacient
    QString getPatientOrientation() const;

    /// Obtenir la reconstruccio
    QString getReconstruction() const;

    /// Obtenir la fase
    int getPhase() const;

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

    /// Hanging Protocol al que pertany
    HangingProtocol *m_hangingProtocol;

    /// Punter al seu image set.
    HangingProtocolImageSet *m_imageSet;

    /// Posició a on es troba
    QString m_position;

    /// Orientacio del pacient
    QString m_patientOrientation;

    /// Reconstruccio
    QString m_reconstruction;

    /// Fase
    int m_phase;

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
