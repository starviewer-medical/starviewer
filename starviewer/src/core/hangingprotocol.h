/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOL_H
#define UDGHANGINGPROTOCOL_H

#include <QObject>

namespace udg {

class HangingProtocolLayout;
class HangingProtocolMask;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocol : public QObject
{
Q_OBJECT
public:
    HangingProtocol( QObject *parent = 0 );

    ~HangingProtocol();

    /// Posar el nom al hanging protocol
    void setName( QString name );

    /// Definició dels diferents nivells que pot tenir un Hanging Protocol
    enum HangingProtocolLevel { Manufacturer, Site, UserGroup, SingleUser };

    /// Obtenir el layout
    HangingProtocolLayout * getHangingProtocolLayout();

    ///Obtenir la mascara
    HangingProtocolMask * getHangingProtocolMask();

    /// Assigna el nombre de screens
    void setNumberOfScreens( int screens );

    /// Assigna els protocols que tracta el hanging protocol
    void setProtocolsList( QList<QString> protocols );

    /// Assigna les posicions de les finestres
    void setDisplayEnvironmentSpatialPositionList( QList<QString> positions );

    /// Afegeix un image set
    void addImageSet ( HangingProtocolImageSet * imageSet );

    /// Afegeix un display set
    void addDisplaySet ( HangingProtocolDisplaySet * displaySet );

    /// Obtenir el nom del hanging protocol
    QString getName() const;

    /// Obté el nombre total d'image sets
    int getNumberOfImageSets() const;

    /// Obté el nombre total d'image sets
    int getNumberOfDisplaySets() const;

    /// Obté l'image set amb identificador "identifier"
    HangingProtocolImageSet * getImageSet( int identifier );

    /// Obté el display set amb identificador "identifier"
    HangingProtocolDisplaySet * getDisplaySet( int identifier ) const;

    /// Obté el display set corresponent a l'image set amb l'identificador detallat
    HangingProtocolDisplaySet * getDisplaySetOfImageSet( int numberOfImageSet );

    /// Mètode per mostrar els valors
    void show();

    /// Posar l'identificador al hanging protocol
    void setIdentifier( int id );

    /// Obtenir l'identificador del hanging protocol
    int getIdentifier() const;

    /// Mètode per comparar hanging protocols
    bool gratherThan( HangingProtocol * hangingToCompare );

    /// Retorna si el mètode és estricte o no ho hes
    bool getStrictness() const;

    /// Assigna si el mètode és estricte o no ho hes
    void setStrictness( bool strictness );

    /// Retorna si el hanging protocol ha de tenir totes les series diferents
    bool getAllDiferent() const;

    /// Assigna si el hanging protocol ha de tenir totes les series diferents
    void setAllDiferent( bool allDiferent );

    /// Assigna el tipus d'icona per representar-lo
    void setIconType( QString iconType );
    
    /// Obté el tipus d'icona per representar-lo
    QString getIconType() const;

    /// Posa si el hanging protocol és de previes o no
    void setPrevious( bool isPrevious );

    /// retorna si el hanging protocol te previes o no
    bool isPrevious();

    /// Posa si te estudis per descarregar
    void setHasStudiesToDownload( bool hasStudiesToDownload );

    /// Obté si el hanging protocol té estudis pendents de descarregar
    bool hasStudiesToDownload();

    /// Assigna una prioritat al hanging protocol
    void setPriority( double priority );

    /// Retorna la prioritat del hanging protocol
    double getPriority();

private:

    /// Identificador
    int m_identifier;

    /// Nom del hanging protocol
    QString m_name;

    /// Descripció del hanging protocol
    QString m_description;

    /// Nivell del hanging protocol
    HangingProtocolLevel m_level;

    /// Creador del hanging protocol
    QString m_creator;

    /// Data i hora de creació del hanging protocol
    QString m_dateTime;

    /// Saber si es candidat
    bool m_candidate;

    /// Definició de layouts
    HangingProtocolLayout * m_layout;

    /// Definició de la màscara
    HangingProtocolMask * m_mask;

    /// Llista d'image sets
    QList< HangingProtocolImageSet * > m_listOfImageSets;

    /// Llista de displays sets
    QList< HangingProtocolDisplaySet * > m_listOfDisplaySets;

    /// Boolea que indica si és estricte o no. Si és estricte vol dir que per ser correcte tots els image sets han d'estar assignats.
    bool m_strictness;

    /// Boolea que indica si les sèries han de ser totes diferents entre elles.
    bool m_allDiferent;

    /// Indica el tipus d'icona per representar el hanging protocol
    QString m_iconType;

    /// Informa si es un hanging protocol amb previes o no
    bool m_hasPrevious;

    /// Informa si conté estudis per descarregar
    bool m_hasStudiesToDownload;

    /// Prioritat del hanging protocol (per defecte -1)
    double m_priority;
};

}

#endif
