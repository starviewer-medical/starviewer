#ifndef UDGLAYOUTMANAGER_H
#define UDGLAYOUTMANAGER_H

#include <QObject>
#include <QSet>

namespace udg {

class Patient;
class Study;
class ViewersLayout;
class HangingProtocolManager;
class RelatedStudiesManager;
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

public slots:
    /// Searches and adds suitable hanging protocols for the given previous studies
    void addHangingProtocolsWithPrevious(QList<Study*> studies);

signals:
    /// Emits this signal when new hanging protocols are found for the current patient
    void hangingProtocolCandidatesFound(QList<HangingProtocol*> candidates);

    /// Emitted when previous studies search is ended. This signal is a hack and should be replaced by a proper solution.
    void previousStudiesSearchEnded();

private:
    /// Applies hanging protocols or automatic layouts depending on the user settings and the availability of the former
    void applyProperLayoutChoice();
    
    /// Hanging Protocols
    
    /// Sets up hanging protocols environment
    void setupHangingProtocols();

    /// Applies the best matching hanging protocol from the available candidates found with searchHangingProtocols
    /// If there are no candidates to apply, false is returned, true otherwise.
    bool applyBestHangingProtocol();

    /// Auto Layouts

    /// Returns a list of StudyLayoutConfig corresponding to the given Patient
    QList<StudyLayoutConfig> getLayoutCandidates(Patient *patient);

    /// Applies the proper layout candidate from the list for the given Patient
    void applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Patient *patient);

private slots:
    /// Sets and applies the hanging protocol with the given identifier
    void setHangingProtocol(int hangingProtocolNumber);

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

    /// Hanging Protocols and related studies managers
    HangingProtocolManager *m_hangingProtocolManager;
    RelatedStudiesManager *m_relatedStudiesManager;

    /// Hanging protocol candidates for the current input
    QList<HangingProtocol*> m_hangingProtocolCandidates;

    /// Set of study UIDs of studies that should be ignored (no action on layout) when they are added
    QSet<QString> m_studiesToIgnoreWhenAdded;
};

} // end namespace udg

#endif
