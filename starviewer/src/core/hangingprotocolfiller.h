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

#ifndef UDG_HANGINGPROTOCOLFILLER_H
#define UDG_HANGINGPROTOCOLFILLER_H

#include <QSet>

namespace udg {

class HangingProtocol;
class HangingProtocolImageSet;
class Series;
class Study;

/**
 * @brief The HangingProtocolFiller class provides the methods needed to assign images to hanging protocol image sets according to their restrictions.
 */
class HangingProtocolFiller
{
public:
    /// Fills the given hanging protocol with images and series from the given current study and prior studies.
    void fill(HangingProtocol *hangingProtocol, Study *currentStudy, const QList<Study*> &priorStudies);
    /// Fills the given image set with images and series from the given study. It's intended to be used when a prior study has been downloaded.
    void fillImageSetWithStudy(HangingProtocolImageSet *imageSet, const Study *study);

private:
    /// Fills the given image set with images and series from the given current study and prior studies.
    void fillImageSet(HangingProtocolImageSet *imageSet, Study *currentStudy, const QList<Study*> &priorStudies);
    /// Fills the given image set with images and series from the given study.
    void fillImageSetWithStudyPrivate(HangingProtocolImageSet *imageSet, const Study *study);
    /// Tries to fill the given image set with the given series. Returns true if successful and false otherwise.
    bool fillImageSetWithSeries(HangingProtocolImageSet *imageSet, Series *series);

    /// Finds and saves in m_usedSeries all the series used by the given hanging protocol.
    void findUsedSeries(HangingProtocol *hangingProtocol);

private:
    /// Set of all the series used by the hanging protocol that is currently being filled. It's used to satisfy the allDifferent property of an image set.
    QSet<const Series*> m_usedSeries;

};

} // namespace udg

#endif // UDG_HANGINGPROTOCOLFILLER_H
