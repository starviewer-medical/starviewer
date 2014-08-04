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

#ifndef UDGLAYOUTMANAGER_H
#define UDGLAYOUTMANAGER_H

#include <QObject>
#include <QSet>

namespace udg {

class Patient;
class Study;
class ViewersLayout;
class HangingProtocolManager;
class StudyLayoutConfig;
class HangingProtocol;

/**
    Manages hanging prootocols and automatic layouts for a given patient in a viewer's layout object
 */
class LayoutManager : public QObject {
Q_OBJECT
public:
    LayoutManager(Patient *patient, ViewersLayout *layout, QObject *parent = 0);
    ~LayoutManager();

    /// Sets up and initizalizes the layout for the current input
    void initialize();

    /// Cancels started operations such as downloading previous studies for an ordered hanging protocol
    void cancelOngoingOperations();

    /// Applies hanging protocols or automatic layouts depending on the user settings and the availability of the former
    void applyProperLayoutChoice();

public slots:
    /// Searches and adds suitable hanging protocols for the given previous studies
    void addHangingProtocolsWithPrevious(QList<Study*> studies);

signals:
    /// Emits this signal when new hanging protocols are found for the current patient
    void hangingProtocolCandidatesFound(QList<HangingProtocol*> candidates);

    /// Emitted when previous studies search is ended. This signal is a hack and should be replaced by a proper solution.
    void previousStudiesSearchEnded();

private:
    /// True if current patient has at least one modality with hanging protocol priority configured over automatic layouts, false otherwise.
    bool hasCurrentPatientAnyModalityWithHangingProtocolPriority();
    
    /// Hanging Protocols

    /// Applies the best matching hanging protocol from the available candidates found with searchHangingProtocols
    /// If there are no candidates to apply, false is returned, true otherwise.
    bool applyBestHangingProtocol();

    /// Auto Layouts

    /// Returns a list of StudyLayoutConfig corresponding to the given Patient
    QList<StudyLayoutConfig> getLayoutCandidates(Patient *patient);

    /// Applies the proper layout candidate from the list for the given Patient
    void applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Patient *patient);

    /// Returns the layout config that best suits for the given candidates and patient
    StudyLayoutConfig getBestLayoutCandidate(const QList<StudyLayoutConfig> &candidates, Patient *patient);

    /// Merges the StudyLayoutConfig items from the list into a single one. Criteria for merge will be as follows
    /// In case UnfoldType are different, LeftToRightFirst will prevail
    /// In case UnfoldDirectionType are different, UnfoldSeries will prevail
    /// If any exclusion criteria is included in one of the configurations, it will be included in the merged as well
    /// The maximum number of viewers will be the smallest value among all configurations
    StudyLayoutConfig getMergedStudyLayoutConfig(const QList<StudyLayoutConfig> &configurations);

private slots:
    /// Sets and applies the hanging protocol with the given identifier or object
    void setHangingProtocol(int hangingProtocolNumber);
    void setHangingProtocol(HangingProtocol *hangingProtocol);

    /// Searches for hanging protocols for the current patient
    void searchHangingProtocols();

    /// Called when a new study has been added to the current patient applying the corresponding layout actions
    void onStudyAdded(Study *study);

    /// Adds the study UID to the ignore set when added
    void addStudyToIgnore(const QString &uid);

private:
    /// Patient for the layout
    Patient *m_patient;

    /// Layout object
    ViewersLayout *m_layout;

    /// Hanging Protocols manager
    HangingProtocolManager *m_hangingProtocolManager;

    /// Hanging protocol candidates for the current input
    QList<HangingProtocol*> m_hangingProtocolCandidates;

    /// Set of study UIDs of studies that should be ignored (no action on layout) when they are added
    QSet<QString> m_studiesToIgnoreWhenAdded;
};

} // end namespace udg

#endif
