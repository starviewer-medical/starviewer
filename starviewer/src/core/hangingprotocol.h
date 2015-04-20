/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGHANGINGPROTOCOL_H
#define UDGHANGINGPROTOCOL_H

#include <QMap>
#include <QRegExp>

namespace udg {

class HangingProtocolLayout;
class HangingProtocolMask;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;

class HangingProtocol {

public:
    HangingProtocol();

    /// Creates a deep copy of the given hanging protocol.
    HangingProtocol(const HangingProtocol &hangingProtocol);

    ~HangingProtocol();

    /// Posar el nom al hanging protocol
    void setName(const QString &name);

    /// Definició dels diferents nivells que pot tenir un Hanging Protocol
    enum HangingProtocolLevel { Manufacturer, Site, UserGroup, SingleUser };

    /// Obtenir el layout
    HangingProtocolLayout* getHangingProtocolLayout() const;

    /// Obtenir la mascara
    HangingProtocolMask* getHangingProtocolMask() const;

    /// Assigna el nombre de screens
    void setNumberOfScreens(int screens);

    /// Assigna els protocols que tracta el hanging protocol
    void setProtocolsList(const QStringList &protocols);

    /// Assigna les posicions de les finestres
    void setDisplayEnvironmentSpatialPositionList(const QStringList &positions);

    /// Afegeix un image set
    void addImageSet(HangingProtocolImageSet *imageSet);

    /// Afegeix un display set
    void addDisplaySet(HangingProtocolDisplaySet *displaySet);

    /// Obtenir el nom del hanging protocol
    QString getName() const;

    /// Obté el nombre total d'image sets
    int getNumberOfImageSets() const;

    /// Obté el nombre total d'image sets
    int getNumberOfDisplaySets() const;

    /// Obté la llista d'image sets
    QList<HangingProtocolImageSet*> getImageSets() const;

    /// Obté la llista de display sets
    QList<HangingProtocolDisplaySet*> getDisplaySets() const;

    /// Obté l'image set amb identificador "identifier"
    HangingProtocolImageSet* getImageSet(int identifier) const;

    /// Obté el display set amb identificador "identifier"
    HangingProtocolDisplaySet* getDisplaySet(int identifier) const;

    /// Assigna/Obté l'expressió regular del nom de les institucions al que han de pertànyer les imatges
    void setInstitutionsRegularExpression(const QRegExp &institutionRegularExpression);
    QRegExp getInstitutionsRegularExpression() const;

    /// Mètode per mostrar els valors
    void show() const;

    /// Posar l'identificador al hanging protocol
    void setIdentifier(int id);

    /// Obtenir l'identificador del hanging protocol
    int getIdentifier() const;

    /// Mètode per comparar hanging protocols
    bool isBetterThan(const HangingProtocol *hangingToCompare) const;

    /// Retorna si el mètode és estricte o no ho hes
    bool isStrict() const;

    /// Assigna si el mètode és estricte o no ho hes
    void setStrictness(bool strictness);

    /// Retorna si el hanging protocol ha de tenir totes les series diferents
    bool getAllDifferent() const;

    /// Assigna si el hanging protocol ha de tenir totes les series diferents
    void setAllDifferent(bool allDifferent);

    /// Assigna el tipus d'icona per representar-lo
    void setIconType(const QString &iconType);

    /// Obté el tipus d'icona per representar-lo
    QString getIconType() const;

    /// Sets the number of priors.
    void setNumberOfPriors(int numberOfPriors);

    /// Returns the number of priors.
    int getNumberOfPriors() const;

    /// Assigna una prioritat al hanging protocol
    void setPriority(double priority);

    /// Retorna la prioritat del hanging protocol
    double getPriority() const;

    /// Compara si dos hanging protocols son iguals
    bool compareTo(const HangingProtocol &hangingProtocol) const;

    /// Retorna el número de ImageSets que tenen una sèrie assignada
    int countFilledImageSets() const;
    /// Return the number of filled ImageSets that its AbstractPriorValue is different to 0
    int countFilledImageSetsWithPriors() const;

    /// Retorna el número de DisplaySets que tenen una sèrie o imatge assignada
    int countFilledDisplaySets() const;
private:
    // Private copy assignment operator so it can't be used accidentally
    HangingProtocol& operator=(const HangingProtocol&);

private:
    /// Identificador
    int m_identifier;

    /// Nom del hanging protocol
    QString m_name;

    /// Definició de layouts
    HangingProtocolLayout *m_layout;

    /// Definició de la màscara
    HangingProtocolMask *m_mask;

    QRegExp m_institutionsRegularExpression;

    /// Map from identifier to image set.
    QMap<int, HangingProtocolImageSet*> m_imageSets;

    /// Map from identifier to display set.
    QMap<int, HangingProtocolDisplaySet*> m_displaySets;

    /// Boolea que indica si és estricte o no. Si és estricte vol dir que per ser correcte tots els image sets han d'estar assignats.
    bool m_strictness;

    /// Boolea que indica si les sèries han de ser totes diferents entre elles.
    bool m_allDifferent;

    /// Indica el tipus d'icona per representar el hanging protocol
    QString m_iconType;

    /// The number of priors in this hanging protocol (based on DICOM Number of Priors Referenced (0072,0014)).
    int m_numberOfPriors;

    /// Priority of this hanging protocol (default: 1).
    double m_priority;
};

}

#endif
