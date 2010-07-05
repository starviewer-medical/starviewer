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
    enum ObserverType {Person, Device, NoneObserverType};

    /// Tipus de Document Title definits per DICOM al DCID 7010
    enum DocumentTitle {OfInterest, RejectedForQualityReasonsDocumentTitle, 
                        ForReferringProvider, ForSurgery, 
                        ForTeaching, ForConference, 
                        ForTherapy, ForPatient,
                        ForPeerReview, ForResearch, 
                        QualityIssue, NoneDocumentTitle};

    /// Tipus de Rejected For Quality Reasons definits per DICOM al DCID 7011
    enum RejectedForQualityReasons {ImageArtifacts, GridArtifacts,
                                    Positioning, MotionBlur, 
                                    UnderExposed, OverExposed,
                                    NoImage, DetectorArtifacts,
                                    ArtifactsOtherThanGridOrDetectorArtifact, MechanicalFailure,
                                    ElectricalFailure, SoftwareFailure,
                                    InappropiateImageProcessing, OtherFailure,
                                    UnknownFailure, DoubleExposure,
                                    NoneRejectedForQualityReasons};
    KeyImageNote();

    ~KeyImageNote();
    
    /// Obtenir/Assignar l'identificador universal de la sèrie
    QString getInstanceUID() const;
    void setInstanceUID(const QString &uid);

    /// Obtenir/Assignar el numero d'instancia
    void setInstanceNumber(const QString &instanceNumber);
    QString getInstanceNumber() const;

    /// Obtenir/Assignar el Content Date ( data de creació de les dades )
    QString getContentDate() const;
    void setContentDate(const QString &contentDate);
    
    /// Obtenir/Assignar el Content Time ( moment de creació de les dades )
    void setContentTime(const QString &contentTime);
    QString getContentTime() const;
    
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
    void setReferencedSOPInstancesUID(QList <DICOMReferencedImage*> &referencedImageList);

    /// Obtenir les imatges referenciades al Key Image Note
    QList<Image*> getReferencedImages();

    /// Obtenir/Assignar la data en que la sèrie s'ha descarregat a la base de dades Local
    QDate getRetrievedDate() const;
    void setRetrievedDate(QDate date);

    /// Obtenir/Assignar la hora en que la sèrie s'ha descarregat a la base de dades Local
    QTime getRetrievedTime() const;
    void setRetrievedTime(QTime time);

    /// Retorna Observer Type de Key Image Note com a un QString
    QString getObserverTypeAsString(KeyImageNote::ObserverType observerType) const;

    /// Retorna Document Title de Key Image Note com a un QString
    QString getDocumentTitleAsString(KeyImageNote::DocumentTitle documentTitle) const;

    /// Retorna Rejected For Quality Reasons de Key Image Note com a un QString
    QString getRejectedForQualityReasonsAsString(KeyImageNote::RejectedForQualityReasons rejectedForQualityReasons) const;

private:
    /// Obte la imatge que tenim referenciada al Key Image Note a partir de la seva referencia 
    Image* getImage(DICOMReferencedImage *referencedImage);

private:
    /// SOP Instance UID del Key Image Note
    QString m_SOPInstanceUID;

    /// Data de creacio de les dades
    QString m_contentDate;

    /// Temps de creacio de les dades
    QString m_contentTime;

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
