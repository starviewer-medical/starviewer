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

#ifndef UDGHANGINGPROTOCOLIMAGESET_H
#define UDGHANGINGPROTOCOLIMAGESET_H

#include "hangingprotocolimagesetrestrictionexpression.h"

#include <QVector>

namespace udg {

class Series;
class Study;
class HangingProtocol;

class HangingProtocolImageSet {

public:
    /// Possible content types of an image set.
    enum class Type { Series, Image, Fusion };

    HangingProtocolImageSet();
    ~HangingProtocolImageSet();

    /// Identificador de l'Image Set
    void setIdentifier(int identifier);

    /// Obtenir l'identificador de l'Image Set
    int getIdentifier() const;

    /// Assignar el hanging protocol al que pertany
    void setHangingProtocol(HangingProtocol *hangingProtocol);

    /// Obtenir el hanging protocol al que pertany
    HangingProtocol* getHangingProtocol() const;

    /// Returns the restriction expressions of this image set.
    const QVector<HangingProtocolImageSetRestrictionExpression>& getRestrictionExpressions() const;
    /// Replaces the current restriction expressions with the given ones.
    void setRestrictionExpressions(const QVector<HangingProtocolImageSetRestrictionExpression> &restrictionExpressions);
    /// Adds a restriction expression to this image set.
    void addRestrictionExpression(const HangingProtocolImageSetRestrictionExpression &restrictionExpression);
    /// Returns the number of restriction expression in this image set.
    int getNumberOfRestrictionExpressions() const;

    /// Sets the type of image set.
    void setType(Type type);
    /// Returns the type of image set.
    Type getType() const;

    /// Posar el número de la imatge a mostrar
    void setImageToDisplay(int imageNumber);

    /// Obtenir el número de la imatge a mostrar
    int getImageToDisplay() const;

    /// Adds a series to display to this image set.
    void addSeriesToDisplay(Series *series);
    /// Removes all the series from the series to display.
    void clearSeriesToDisplay();
    /// Returns all the series to display of this image set.
    const QVector<Series*>& getSeriesToDisplay() const;
    /// Returns the number of series to display in this image set.
    int getNumberOfSeriesToDisplay() const;

    /// Mètode per mostrar els valors
    void show();

    /// Mètode per posar si esta descarregat o no
    void setDownloaded(bool option);

    /// Mètode per saber si està descarregat o no
    bool isDownloaded();

    /// Posa quin és l'estudi previ
    void setPreviousStudyToDisplay(Study *study);

    /// Obte l'estudi previ a mostrar. Pot estar descarregat o no.
    Study* getPreviousStudyToDisplay();

    /// Returns the abstract prior value of this image set.
    int getAbstractPriorValue() const;
    /// Sets the abstract prior value of this image set.
    void setAbstractPriorValue(int value);

    /// Obté l'índex de la imatge a mostrar dins la serie
    int getImageNumberInStudyModality();

    /// Posa l'índex de la imatge a mostar del pacient
    void setImageNumberInStudyModality(int imageNumberInStudyModality);

    /// Returns true if the image set is filled and false otherwise. It is considered filled if it has a series to display for each restriction.
    bool isFilled() const;

private:
    /// The restriction expressions that this image set must satisfy, one for each selected series or volume.
    QVector<HangingProtocolImageSetRestrictionExpression> m_restrictionExpressions;

    /// Identificador únic de l'Image Set
    int m_identifier;

    /// Hanging Protocol al que pertany
    HangingProtocol *m_hangingProtocol;

    /// Type of this image set.
    Type m_type;

    /// Series to display in this image set, one for each restriction expression.
    QVector<Series*> m_seriesToDisplay;

    /// Número d'imatge a mostrar, es coneix aquest número un cop
    /// trobada la imatge que compleix les restriccions
    int m_imageToDisplay;

    /// Índex de la imatge dins l'estudi. Es coneix aquest número
    /// ja quan es crea el hanging protocol
    int m_imageNumberInStudyModality;

    /// Indica si esta o no descarregat
    bool m_downloaded;

    /// Estudi previ a l'image set, sense descarregar
    Study *m_previousStudyToDisplay;

    /// Identifies a prior image set in abstract terms. The value 0 shall indicate a current image set, 1 indicates the most recent prior and higher values
    /// indicate successively older priors. The special value -1 shall indicate the oldest prior.
    /// It's loosely based on the DICOM Abstract Prior Value (0072,003C).
    int m_abstractPriorValue;

};

}

#endif
