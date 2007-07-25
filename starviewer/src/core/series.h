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
#include <QDateTime>
#include <QImage>
#include "identifier.h"

namespace udg {

class Volume;
class Image;
class Study;

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
    QString getInstanceUID() const { return m_seriesInstanceUID; };

    /// assigna l'estudi pare de la sèrie
    void setParentStudy( Study *study ){ m_parentStudy = study; };

    /// afegeix un objecte imatge
    void addImage( Image *image );

    /// Assignar/Obtenir la modalitat de la sèrie
    void setModality( QString modality );
    QString getModality() const { return m_modality; };

    /// Assignar/Obtenir la descripció de la sèrie
    void setDescription( QString description );
    QString getDescription() const { return m_description; };

    /// Assignar/Obtenir la posició del pacient relativa a la màquina
    void setPatientPosition( QString position ){ m_patientPosition = position; }
    QString getPatientPosition() const { return m_patientPosition; }

    /// Assignar/Obtenir el protocol de la sèrie
    void setProtocolName( QString protocolName );
    QString getProtocolName() const { return m_protocolName; };

    /// Assignar/Obtenir el path de les imatges de la sèrie
    void setImagesPath( QString imagesPath );
    QString getImagesPath() const { return m_imagesPath; };

    /// Assignar/Obtenir la data i hora d'adquisició de la sèrie en format DD/MM/AAAA HH:MM. Retorna fals si hi ha algun error en el format
    bool setDateTime( int day , int month , int year , int hour , int minute );
    bool setDateTime( QString date , QString time );
    bool setDateTime( QString dateTime );
    QDateTime getDateTime() const { return m_dateTime; };
    QString getDateTimeAsString();
    bool setDate( int day , int month , int year );
    bool setDate( QString date );
    bool setTime( int hour , int minute );
    bool setTime( QString time );
    QDate getDate();
    QString getDateAsString();
    QTime getTime();
    QString getTimeAsString();

    /// Assignar/Obtenir identificador del volum al repositori corresponent a la sèrie \TODO estem assumint que un volum = una sèrie i això no és del tot cert. L'id, en tot cas, hauria d'anar relacionat amb el subvolum
    void setVolumeIdentifier( Identifier id );
    Identifier getVolumeIdentifier() const { return m_volumeID; }

    /// Retorna el nombre de volums dels que es composa la sèrie.
    int getNumberOfVolumes();

    /// Retorna el subvolum amb índex 'index', per defecte, el 0
    Volume *getVolume( int index = 0 );

    /// Llista dels noms de fitxer de les imatges
    QStringList getImagesPathList();

    /// Ens diu si la sèrie està marcada com a seleccionada o no
    inline bool isSelected() const { return m_selected; }

public slots:
    /// Selecciona/deselecciona aquesta sèrie
    void select(){ setSelectStatus( true ); };
    void unSelect(){ setSelectStatus( false ); };
    void setSelectStatus( bool select ){ m_selected = select; };

private:
    /// Informació comuna de la sèrie. C.7.3.1 General Series Module - PS 3.3.

    /// Tipus d'equipament que originalment va adquirir les dades per crear les imatges creades en aquesta sèrie. Veure C.7.3.1.1.1 pels termes definits. (0008,0060) Tipus 1
    QString m_modality;

    /// Identidicador universal de la sèrie. (0020,000E) Tipus 1
    QString m_seriesInstanceUID;

    /// Nombre que identifica la sèrie. (0020,0011) Tipus 2
    QString m_seriesNumber;

    /// Data i hora en la que s'ha començat la sèrie. (0008,0021),(0008,0031) Tipus 3. \TODO Tractar per separat o conjuntament?
    QDateTime m_dateTime;

    /// Descriptor de la posició del pacient relativa a la màquina. Requerit per imatges CT i MR. No hi ha d'estar present si Patient Orientation Code Sequence (0054,0410) està present. Veure C.7.3.1.1.2 per termes definits i una explicació més profunda. (0018,5100) Tipus 2C
    QString m_patientPosition;

    /// Protocol que s'ha aplicat per obtenir les imatges definit per l'usuari. (0018,1030) Tipus 3
    QString m_protocolName;

    /// Descripció de la sèrie definida per l'usuari. (0008,103E) Tipus 3
    QString m_description;

    // FRAME OF REFERENCE MODULE. C.7.4 \TODO En principi no cal implementar-ho com a una entitat per separat i incloent-ho dins de sèries n'hauria d'haver prou per poder tractar el que necessitem
    /// Identifica el frame of reference universalment. Veure C.7.4.1.1.1 per una explicació més profunda. (0020,0052) Tipus 1
    QString m_frameOfReferenceUID;

    /// Part de l'anatomia del pacient usat com a referència. Veure C.7.4.1.1.2, només per propòsits d'annotació. (0020,1040) Tipus 2
    QString m_positionReferenceIndicator;


    /// Indica si la sèrie està marcada com a seleccionada o no
    bool m_selected;

    /// Directori sota el qual es trobem les imatges
    QString m_imagesPath;

    /// Imatge de previsualització associada a la sèrie
    QImage m_previewImage;

    /// Identificador del volum al repositori
    Identifier m_volumeID;

    /// Llista de volums que composen la sèrie. La sèrie es pot separar en diversos volums per diverses raons, com pot ser mides d'imatge diferent, sèries amb dinàmics o fases, stacks, etc.
    QList<Volume *> m_volumesList;

    /// llista d'imatges que composen la sèrie
    QList< Image *> m_imageList;

    /// Estudi pare
    Study *m_parentStudy;
};

}

#endif
