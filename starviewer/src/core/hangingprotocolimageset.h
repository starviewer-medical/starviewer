#ifndef UDGHANGINGPROTOCOLIMAGESET_H
#define UDGHANGINGPROTOCOLIMAGESET_H

#include <QObject>

namespace udg {

class Series;
class Study;
class HangingProtocol;

class HangingProtocolImageSet : public QObject {
Q_OBJECT
public:
    HangingProtocolImageSet(QObject *parent = 0);
    ~HangingProtocolImageSet();

    enum SelectorUsageFlag { Match, NoMatch };

    struct Restriction
    {
        // Match o NoMatch
        SelectorUsageFlag usageFlag;
        // TAG
        QString selectorAttribute;
        // Valor del TAG
        QString valueRepresentation;
        // Només si el TAG és multivalor
        int selectorValueNumber;
    };

    /// Identificador de l'Image Set
    void setIdentifier(int identifier);

    /// Obtenir l'identificador de l'Image Set
    int getIdentifier() const;

    /// Assignar el hanging protocol al que pertany
    void setHangingProtocol(HangingProtocol *hangingProtocol);

    /// Obtenir el hanging protocol al que pertany
    HangingProtocol* getHangingProtocol() const;

    /// Afegir una restricció
    void addRestriction(Restriction restriction);

    /// Assignar la llista de restriccions
    void setRestrictions(const QList<Restriction> &restrictions);

    /// Obtenir les restriccions
    QList<Restriction> getRestrictions() const;

    /// Posar el tipus d'element
    void setTypeOfItem(QString);

    /// Obtenir el tipus d'element
    QString getTypeOfItem() const;

    /// Posar el número de la imatge a mostrar
    void setImageToDisplay(int imageNumber);

    /// Obtenir el número de la imatge a mostrar
    int getImageToDisplay() const;

    /// Assignar la sèrie que es representa
    void setSeriesToDisplay(Series *series);

    /// Obtenir la sèrie que es representa
    Series* getSeriesToDisplay() const;

    /// Mètode per mostrar els valors
    void show();

    /// Metode per indicar si es un estudi previ o no
    void setIsPreviousStudy(bool hasPreviousStudy);

    /// Mètode per saber si te previs o no
    bool isPreviousStudy();

    /// Mètode per posar si esta descarregat o no
    void setDownloaded(bool option);

    /// Mètode per saber si està descarregat o no
    bool isDownloaded();

    /// Posa quin és l'estudi previ
    void setPreviousStudyToDisplay(Study *study);

    /// Obte l'estudi previ a mostrar. Pot estar descarregat o no.
    Study* getPreviousStudyToDisplay();

    /// Guarda el pacs on es troba l'estudi
    void setPreviousStudyPacs(QString pacs);

    /// Obté el pacs on es troba l'estudi
    QString getPreviousStudyPacs();

    /// Posa l'ImageSet del qual és previ
    void setPreviousImageSetReference(int imageSetNumber);

    /// Obté l'ImageSet del qual és previ
    int getPreviousImageSetReference();

    /// Obté l'índex de la imatge a mostrar dins la serie
    int getImageNumberInPatientModality();

    /// Posa l'índex de la imatge a mostar del pacient
    void setImageNumberInPatientModality(int imageNumberInPatientModality);

private:
    /// Llista de restriccions que ha de complir l'Image Set
    QList<Restriction> m_listOfRestrictions;

    /// Identificador únic de l'Image Set
    int m_identifier;

    /// Hanging Protocol al que pertany
    HangingProtocol *m_hangingProtocol;

    /// Per saber si s'ha de tractar a nivell d'imatge o de sèrie.
    QString m_typeOfItem;

    /// Serie que s'ajusta a les restriccions
    Series *m_serieToDisplay;

    /// Número d'imatge a mostrar, es coneix aquest número un cop
    /// trobada la imatge que compleix les restriccions
    int m_imageToDisplay;

    /// Índex de la imatge dins l'estudi. Es coneix aquest número
    /// ja quan es crea el hanging protocol
    int m_imageNumberInPatientModality;

    /// Indica si conte un estudi previ
    bool m_isPreviousStudy;

    /// Indica si esta o no descarregat
    bool m_downloaded;

    /// Estudi previ a l'image set, sense descarregar
    Study *m_previousStudyToDisplay;

    /// Pacs origen on es troba l'estdi
    QString m_previousStudyPacs;

    /// Posa l'image set que té de referencia i del qual ha de ser previ
    int m_previousImageSetReference;

};

}

#endif
