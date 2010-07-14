/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTE_H
#define UDGKEYIMAGENOTE_H

#include <QObject>
#include <QDateTime>
#include <QString>

namespace udg {

class DICOMReferencedImage;
class Series;
class Image;

/**
Classe que encapsula les propietats d'un Key Image Note

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNote : public QObject
{
Q_OBJECT
public:
    /// Tipus d'Observador del Key Image Note
    enum ObserverType {Person = 121006, Device = 121007, NoneObserverType = 0};

    /// Tipus de Document Title definits per DICOM al DCID 7010
    enum DocumentTitle {OfInterest = 113000, RejectedForQualityReasonsDocumentTitle =113001, 
                        ForReferringProvider = 113002, ForSurgery = 113003, 
                        ForTeaching = 113004, ForConference = 113005, 
                        ForTherapy = 113006, ForPatient = 113007,
                        ForPeerReview = 113008, ForResearch = 113009, 
                        QualityIssue = 113010, NoneDocumentTitle = 0};

    /// Tipus de Rejected For Quality Reasons definits per DICOM al DCID 7011
    enum RejectedForQualityReasons {ImageArtifacts = 111207, GridArtifacts = 111208,
                                    Positioning = 111209, MotionBlur = 111210, 
                                    UnderExposed = 111211, OverExposed = 111212,
                                    NoImage = 111213, DetectorArtifacts = 111214,
                                    ArtifactsOtherThanGridOrDetectorArtifact = 111215, MechanicalFailure = 111216,
                                    ElectricalFailure = 111217, SoftwareFailure = 111218,
                                    InappropiateImageProcessing = 111219, OtherFailure = 111220,
                                    UnknownFailure = 111221, DoubleExposure = 113026,
                                    NoneRejectedForQualityReasons = 0};
    KeyImageNote();

    ~KeyImageNote();
    
    /// Obtenir/Assignar l'identificador universal de la sèrie
    QString getInstanceUID() const;
    void setInstanceUID(const QString &uid);

    /// Obtenir/Assignar el numero d'instancia
    void setInstanceNumber(const QString &instanceNumber);
    QString getInstanceNumber() const;
    
    /// Assignar/Obtenir la data i hora d'adquisició de la sèrie. El format de la data serà YYYYMMDD i el del
    /// time hhmmss.frac on frac és una fracció de segon de rang 000000-999999
    ///  Retorna fals si hi ha algun error en el format
    bool setContentDate(QString date);
    bool setContentDate(QDate date);
    bool setContentTime(QString time);
    bool setContentTime(QTime time);
    QDate getContentDate();
    QTime getContentTime();
    
    /// Obtenir/Assignar la sèrie pare a la qual pertany
    Series* getParentSeries() const;
    void setParentSeries(Series *series);
    
    /// Obtenir/Assignar el tipus de Document Title 
    KeyImageNote::DocumentTitle getDocumentTitle() const;
    void setDocumentTitle(KeyImageNote::DocumentTitle documentTitle);

    /// Obtenir/Assignar el Rejected For Quality Reasons
    KeyImageNote::RejectedForQualityReasons getRejectedForQualityReasons() const;
    void setRejectedForQualityReasons(KeyImageNote::RejectedForQualityReasons rejectedForQualityReasons);

    /// Obtenir/Assignar la descripcio del Key Image Note
    QString getKeyObjectDescription() const;
    void setKeyObjectDescription(const QString &description);
    
    /// Obtenir/Assignar l'Observer Type
    KeyImageNote::ObserverType getObserverContextType() const;
    void setObserverContextType(KeyImageNote::ObserverType contextType);

    /// Obtenir/Assignar el nom de l'observador
    QString getObserverContextName() const;
    void setObserverContextName(const QString &contextName);
    
    /// Obtenir/Assignar els UID referenciats al Key Image Note
    QList<DICOMReferencedImage*> getDICOMReferencedImages() const;
    void setDICOMReferencedImages(QList <DICOMReferencedImage*> &referencedImageList);

    /// Obtenir les imatges referenciades al Key Image Note
    QList<Image*> getReferencedImages();

    /// Obtenir/Assignar la data en que la sèrie s'ha descarregat a la base de dades Local
    QDate getRetrievedDate() const;
    void setRetrievedDate(QDate date);

    /// Obtenir/Assignar la hora en que la sèrie s'ha descarregat a la base de dades Local
    QTime getRetrievedTime() const;
    void setRetrievedTime(QTime time);

    /// Retorna Observer Type de Key Image Note com a un QString
    QString getObserverTypeAsString() const;

    /// Retorna Document Title de Key Image Note com a un QString
    QString getDocumentTitleAsString() const;

    /// Retorna Rejected For Quality Reasons de Key Image Note com a un QString
    QString getRejectedForQualityReasonsAsString() const;

private:
    /// Obte la imatge que tenim referenciada al Key Image Note a partir de la seva referencia 
    Image* getImage(DICOMReferencedImage *referencedImage);

    /// Retorna una imatge si la troba a la serie altrament retorna NULL
    Image* getDICOMReferencedImagesFromSeries(Series *serie, DICOMReferencedImage *referencedImage);

private:
    /// SOP Instance UID del Key Image Note
    QString m_SOPInstanceUID;

    /// Data de creacio de les dades
    QDate m_contentDate;

    /// Temps de creacio de les dades
    QTime m_contentTime;

    /// Descripcio del Key Image Note
    QString m_keyObjectDescription;

    /// Tipus d'Observador
    KeyImageNote::ObserverType m_observerContextType;

    /// Nom de l'observador
    QString m_observerContextName;
    
    /// Llistat de referencies d'imatges
    QList <DICOMReferencedImage*> m_DICOMReferencedImages;

    /// La serie pare
    Series *m_parentSeries;

    /// Numero d'instancia
    QString m_instanceNumber;

    /// Data en la que s'ha descarregat de la base de dades
    QDate m_retrievedDate;

    /// Hora en la que s'ha descarregat de la base de dades
    QTime m_retrieveTime;

    /// Titol del document
    KeyImageNote::DocumentTitle m_documentTitle;

    /// Titol del document per motius de qualitat
    KeyImageNote::RejectedForQualityReasons m_rejectedForQualityReasons;
};

}
#endif
