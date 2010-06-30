/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSERIES_H
#define UDGSERIES_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QDateTime>
#include <QPixmap>
#include "identifier.h"

namespace udg {

class Volume;
class Image;
class Study;
class KeyImageNote;

/**
Classe que encapsula la sèrie d'un pacient.

La classe conté tot tipu d'informació relacionada amb la sèrie d'un pacient. Una sèrie pot equivaler a un o més volums, per tant tindrem la llista de Volums corresponents a la sèrie.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Series : public QObject
{
Q_OBJECT
public:
    Series(QObject *parent = 0);

    ~Series();

    /// Assignar/Obtenir l'identificador universal de la sèrie
    void setInstanceUID( QString uid );
    QString getInstanceUID() const;

    /// Assignar/Obtenir el SOP Class UID
    void setSOPClassUID( QString sopClassUID );
    QString getSOPClassUID() const;

    /// Assignar/Obtenir la modalitat de la sèrie
    void setModality( QString modality );
    QString getModality() const;

    /// Assignar/Obtenir el número de la sèrie
    void setSeriesNumber( QString number );
    QString getSeriesNumber() const;

    /// Assignar/Obtenir el FrameOfReferenceUID
    void setFrameOfReferenceUID( QString uid );
    QString getFrameOfReferenceUID() const;

    /// Assignar/Obtenir el PositionReferenceIndicator
    void setPositionReferenceIndicator( QString position );
    QString getPositionReferenceIndicator() const;

    /// Assignar/Obtenir la descripció de la sèrie
    void setDescription( QString description );
    QString getDescription() const;

    /**
     * Assignar/Obtenir la posició del pacient relativa a la màquina
     * Tenim diversos termes definits que ens diuen en quina posició entra el pacient a la màquina
     * - Head First vol dir que el pacient entra de cap
     * - Feet First vol dir que el pacient entra de peus
     * - Prone vol dir que el pacient mira cap avall
     * - Supine vol dir que el pacient mira cap amunt
     * - Decubitus Right vol dir que el pacient està recolzat sobre el seu costat esquerre
     * - Decubitus Left vol dir que el pacient està recolzat sobre el seu costat dret
     *
     * Aquests són els termes disponibles
     * HFP: Head First-Prone                HFS: Head First-Supine
     * HFDR: Head First-Decubitus Right     HFDL: Head First-Decubitus Left
     * FFDR: Feet First-Decubitus Right     FFDL: Feet First-Decubitus Left
     * FFP: Feet First-Prone                FFS: Feet First-Supine
     *
     * @param position String amb un dels valors esmentats
     */
    void setPatientPosition( QString position );
    QString getPatientPosition() const;

    /// Assignar/Obtenir el protocol de la sèrie
    void setProtocolName( QString protocolName );
    QString getProtocolName() const;

    /// Assignar/Obtenir la data i hora d'adquisició de la sèrie. El format de la data serà YYYYMMDD i el del
    /// time hhmmss.frac on frac és una fracció de segon de rang 000000-999999
    ///  Retorna fals si hi ha algun error en el format
    bool setDateTime( int day , int month , int year , int hour , int minute, int second = 0 );
    bool setDateTime( QString date , QString time );
    bool setDate( int day , int month , int year );
    bool setDate( QString date );
    bool setDate( QDate date );
    bool setTime( int hour , int minute, int second = 0 );
    bool setTime( QString time );
    bool setTime( QTime time );
    QDate getDate();
    QString getDateAsString();
    QTime getTime();
    QString getTimeAsString();

    /// Assignar/Obtenir la institució on s'ha realitzat l'estudi
    void setInstitutionName( QString institutionName );
    QString getInstitutionName() const;

    /**
     * Assigna/Retorna la part del cos examinada
     */
    void setBodyPartExamined( QString bodyPart );
    QString getBodyPartExamined() const;

    /**
     * Assigna/Retorna la vista radiogràfica associada amb la posició del pacient
     */
    void setViewPosition( QString viewPosition );
    QString getViewPosition() const;

    ///Assigna/Obté el nom del fabricat de l'equip amb el que s'ha n capturar les imatges de la sèrie
    void setManufacturer( QString manufactures );
    QString getManufacturer() const;

    ///Assignar/Obtenir la data i hora en que la sèrie s'ha descarregat a la base de dades Local
    void setRetrievedDate(QDate date);
    void setRetrievedTime(QTime time);
    QDate getRetrievedDate();
    QTime getRetrievedTime();

    /// assigna l'estudi pare de la sèrie
    void setParentStudy( Study *study );
    Study *getParentStudy() const;

    /// afegeix un objecte imatge a la sèrie i li assigna com a parent aquest objecte series.
    /// Si la imatge ja existeix al conjunt retorna fals, cert altrament
    bool addImage( Image *image );

    /**
     * Ens diu si existeix una imatge amb aquest identificador a la llista
     * @param identifier identificador d'imatge que busquem
     * @return Cert si existeix, fals altrament
     */
    bool imageExists( const QString &identifier );

    /// Retorna una llista de totes les imatges de la sèrie
    QList<Image *> getImages() const;
    void setImages( QList<Image *> imageSet );

    /// Afegeix un Key Image Note a la serie
    void addKeyImageNote(KeyImageNote *keyImageNote);

    /// Obte els Key Image Notes de la serie
    QList<KeyImageNote*> getKeyImageNotes() const;

    /**
     * Ens diu quantes imatges té aquesta sèrie
     * @return El nombre d'imatges. 0 en cas que no sigui una sèrie d'imatges o no en contingui
     */
    int getNumberOfImages();
    
    /// Ens retorna el nombre d'ítems que té la sèrie, sense diferenciar si són imatges o no. 
    /// Equival al nombre d'arxius que conté la sèrie
    int getNumberOfItems();
    
    ///Especificia el número d'imatges: necessari per quan omplim desde la bdd   	 	 
    void setNumberOfImages(int numberOfImages);
    
    /// Indica si una sèrie té imatges
    bool hasImages() const;

    /// Assignar/Obtenir el path de les imatges de la sèrie
    void setImagesPath( QString imagesPath );
    QString getImagesPath() const;

    /// Assignar/Obtenir el requeste procedure ID
    void setRequestedProcedureID( const QString &requestedProcedureID );
    QString getRequestedProcedureID() const;

    /// Assignar/Obtenir el scheduled procedure step ID
    void setScheduledProcedureStepID( const QString &scheduledProcedureStepID );
    QString getScheduledProcedureStepID() const;

    /// Assignar/Obtenir el Performed Procedure Step Start Date
    void setPerformedProcedureStepStartDate( const QString &startDate );
    QString getPerformedProcedureStepStartDate() const;

    /// Assigna/Obté el Perfomed Procedure Step Start Time
    void setPerformedProcedureStepStartTime( const QString &startTime );
    QString getPerformedProcedureStepStartTime() const;

    /// Assigna/Obté la lateralitat de la sèrie
    void setLaterality( const QChar &laterality );
    QChar getLaterality() const;

    /// Retorna el nombre de volums dels que es composa la sèrie.
    int getNumberOfVolumes();

    /// Retorna el Volume amb identificador id
    /// Retorna NULL en cas que no hi hagi cap volum amb aquest id.
    Volume *getVolume(Identifier id);

    /// Mètode per conveniència que serveix per retornar el "primer" volum. En el 90% dels casos (de moment el 100%)
    /// tindrem que per cada sèrie només hi haurà un sol volum. Aquest mètode retorna aquest o, en cas de més d'un, el primer.
    /// Retorna NULL en cas que no hi hagi cap volum.
    Volume *getFirstVolume();

    /// Mètode per afegir un sol volum a la llista de volums de la serie. Retorna l'id amb el que s'ha guardat al repositori
    /// de volums.
    Identifier addVolume(Volume *volume);

    /// Retorna una llista amb tots els volums de la sèrie.
    QList<Volume*> getVolumesList();

    /// Retorna la llista d'identificadors de volum
    QList<Identifier> getVolumesIDList() const;

    /// Llista dels noms de fitxer de les imatges
    QStringList getImagesPathList();

    /// Ens diu si la sèrie està marcada com a seleccionada o no
    bool isSelected() const;

    QString toString(bool verbose = false);

    /// Obté la imatge de previsualització de la sèrie. Serà la imatge del mig.
    QPixmap getThumbnail();

	/// Mètode temporal per obtenir la Image segons com està ordenada a la llista
	/// Si l'índex que donem està fora de rang, es retorna una imatge nul·la
	/// TODO això no ens assegura que obtenim la imatge que suposadament volem
	Image *getImageByIndex( int index );

    ///Permet establir quin és el thumbnail de la sèrie
    void setThumbnail(QPixmap seriesThumbnail);

    /// Aquest mètode comprova si aquesta sèrie és visualitzable
    /// és a dir que es pot visualitzar en un Q*Viewer
    /// Les comprovacions que fa són mirar que no es tracti d'una modalitat
    /// que no sigui d'imatge (PR,KIN,SR...) i/o que contingui realment imatges a l'image set
    /// La comprovació es fa "al vol", és a dir, sota demanda
    /// TODO queda pendent fer altres comprovacions més acurades com 
    /// comprovar que es tracti d'un SOP Class suportat per l'aplicació
    bool isViewable() const;

public slots:
    /// Selecciona/deselecciona aquesta sèrie
    void select();
    void unSelect();
    void setSelectStatus(bool select);

private:
    /**
     * Troba l'índex de la imatge amb el identificadir donat a la llista d'imatges
     * @param identifier L'identificador de la imatge que volem trobar
     * @return L'índex d'aquella imatge dins de la llista, -1 si no existeix la imatge amb aquell identificador.
     */
    int findImageIndex( const QString &identifier );

private:
    /// Identificació única del tipus de SOP. Veure PS 3.4 per conèixer el possibles valors que pot tenir.
    QString m_sopClassUID;

    /// Informació comuna de la sèrie. C.7.3.1 General Series Module - PS 3.3.

    /// Tipus d'equipament que originalment va adquirir les dades per crear les imatges creades en aquesta sèrie.
    /// Veure C.7.3.1.1.1 pels termes definits. (0008,0060) Tipus 1
    QString m_modality;

    /// Identidicador universal de la sèrie. (0020,000E) Tipus 1
    QString m_seriesInstanceUID;

    /// Nombre que identifica la sèrie. (0020,0011) Tipus 2
    QString m_seriesNumber;

    /// Data i hora en la que s'ha començat la sèrie. (0008,0021),(0008,0031) Tipus 3.
    QDate m_date;
    QTime m_time;

    //\TODO aquest tag està dins de l'Equipment IE module. Se suposa que és mes correcte posar-ho a nivell de sèrie.
    //Llegir A.1.2.3 SERIES IE, apartat c., A.1.2.4 EQUIPMENT IE i C.7.5 Common Equipment IE Modules - PS 3.3
    /// Nom de l'institució en la que s'ha fet
    QString m_institutionName;

    /// Descriptor de la posició del pacient relativa a la màquina. Requerit per imatges CT i MR. No hi ha d'estar present
    /// si Patient Orientation Code Sequence (0054,0410) està present. Veure C.7.3.1.1.2 per termes definits i una explicació més profunda.
    /// (0018,5100) Tipus 2C
    QString m_patientPosition;

    /// Protocol que s'ha aplicat per obtenir les imatges definit per l'usuari. (0018,1030) Tipus 3
    QString m_protocolName;

    /// Descripció de la sèrie definida per l'usuari. (0008,103E) Tipus 3
    QString m_description;

    // FRAME OF REFERENCE MODULE. C.7.4
    //\TODO En principi no cal implementar-ho com a una entitat per separat i incloent-ho dins de sèries n'hauria d'haver
    //prou per poder tractar el que necessitem
    /// Identifica el frame of reference universalment. Veure C.7.4.1.1.1 per una explicació més profunda. (0020,0052) Tipus 1
    QString m_frameOfReferenceUID;

    /// Part de l'anatomia del pacient usat com a referència. Veure C.7.4.1.1.2, només per propòsits d'annotació. (0020,1040) Tipus 2
    QString m_positionReferenceIndicator;

    /** 
      Lateralitat de la part del cos examinada. Requerit si aquesta és aparellada i no tenim Image o Frame Laterality.
      Valors definits:
      R = right
      L = left
    */
    QChar m_laterality;

    /// Indica si la sèrie està marcada com a seleccionada o no
    bool m_selected;

    /// Directori sota el qual ens trobem les imatges
    QString m_imagesPath;

    /// Llista d'id's de volums que composen la sèrie. La sèrie es pot separar en diversos volums per diverses raons,
    /// com pot ser mides d'imatge diferent, sèries amb dinàmics o fases, stacks, etc.
    QList<Identifier> m_volumesList;

    /// Llista de les Image de la serie ordenades per criteris d'ordenació com SliceLocation,InstanceNumber, etc
    /// TODO falta definir quina és l'estrategia d'ordenació per defecte
    QList<Image *> m_imageSet;

    /// Estudi pare
    Study *m_parentStudy;

    /// Part del cos examinada. (0018,015). Tipus 2/3, segons modalitat
    QString m_bodyPartExamined;

    /// Vista radiogràfica associada amb la posició del pacient[(0018,5101)]. (0018,5101). Tipus 2/3, segons modalitat
    QString m_viewPosition;

    ///Obté el fabricant de l'aparell amb que s'ha obtingut la sèrie (0008,0070) Tipus 2.
    QString m_manufacturer;

    ///Data en que la sèrie s'ha descarregat a la base de dades local
    QDate m_retrievedDate;
    QTime m_retrieveTime;

    /// Número d'imatges de la sèrie. Fem el recompte manualment per quan consultem de la BDD
    int m_numberOfImages;
    
    ///Guarda el thumbnail de la sèrie
    QPixmap m_seriesThumbnail;

    ///Request Procedure ID (0040,1001) Tipus 1C/2 
    QString m_requestedProcedureID;
    ///Schedulet Procedure Step ID (0040,0253) Tipus 1C/3
    QString m_scheduledProcedureStepID;
    ///Perfomed procedure step start date (0040,0244) Tipus 3
    QString m_performedProcedureStepStartDate;
    ///Perfomed procedure step start time (0040,0345) Tipus 3
    QString m_performedProcedureStepStartTime;

    /// Llistat de Key Image Notes
    QList<KeyImageNote*> m_kinSet;
};

}

#endif
