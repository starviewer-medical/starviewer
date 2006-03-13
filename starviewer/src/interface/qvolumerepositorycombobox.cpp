/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qvolumerepositorycombobox.h"
#include "volumerepository.h"

namespace udg {

QVolumeRepositoryComboBox::QVolumeRepositoryComboBox( QWidget *parent )
 : QComboBox( parent )
{
    m_volumeRepository = VolumeRepository::getRepository();
    connect( m_volumeRepository , SIGNAL( itemAdded(Identifier) ) , this , SLOT( insertVolume(Identifier) ) );
    connect( m_volumeRepository , SIGNAL( itemRemoved(Identifier) ) , this , SLOT( removeVolume(Identifier) ) );
    // pot ser que aquest combo es creï després que el repositori, per tant li ha de preguntar pels seus volums per poder-se omplir i estar actualitzat
    updateMapping();
}


QVolumeRepositoryComboBox::~QVolumeRepositoryComboBox()
{
}

Identifier QVolumeRepositoryComboBox::getVolumeID() const
{
    return m_idFromIndex[ this->currentIndex() ];
}

void QVolumeRepositoryComboBox::setVolumeID( Identifier id )
{
    if( m_indexFromID.contains( id ) ) 
        this->setCurrentIndex( m_indexFromID[id] );
}

void QVolumeRepositoryComboBox::addNewItem( const QString &name, Identifier id )
{
    this->addItem( name ); // aquí estem cridant al mètode de QComboBox
    // podríem fer l'update mapping, és la manera més senzilla, tot i que no del tot eficient
    m_idFromIndex[ this->count()-1 ] = id;
    m_indexFromID[ id ] = this->count()-1;

}

void QVolumeRepositoryComboBox::deleteItem( Identifier id )
{
// mooolt al tanto amb això, els combo han d'estar ben sincronitzats sinó pot sre que el map d'un id amb un index sigui diferent en cada finestra
// problemes:!!! quan esborrem un element de la llista s'haurien d'actualitzar els associacions dels map. Una solució possible seria fer un map identifier-string i eliminar els elements per nom
// sinó s'hauria de fer un re-map dels valors cada cop que eliminem algun element

    this->removeItem( m_indexFromID[id] );
    updateMapping();
    
}

void QVolumeRepositoryComboBox::removeVolume( Identifier id )
{
    deleteItem( id );   
}

void QVolumeRepositoryComboBox::insertVolume( Identifier id )
{
    QString name;
    name.sprintf( "%d-Volume" , id.getValue() );
    addNewItem( name , id );
}

void QVolumeRepositoryComboBox::updateMapping()
{
    m_indexFromID.clear();
    m_idFromIndex.clear();
    
    this->clear();
      
    Identifier *idList = m_volumeRepository->getIdentifierList();
    if( idList )
    {
        int i = 0;
        while( i < m_volumeRepository->getNumberOfVolumes() )
        {
            insertVolume( idList[ i ] );
            i++;
        }
        
    }
}

};  // end namespace udg 
