#ifndef UDGLAYOUTMANAGER_H
#define UDGLAYOUTMANAGER_H

#include <QObject>

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

    /// Applies hanging protocols or automatic layouts depending on the user settings and the availability of the former
    void applyProperLayoutChoice();
    
    /// Cancels started operations such as downloading previous studies for an ordered hanging protocol
    void cancelOngoingOperations();

signals:
    /// Emits this signal when new hanging protocols are found for the current patient
    void hangingProtocolCandidatesFound(QList<HangingProtocol*> candidates);

    /// Emitted when previous studies are
    void previousStudiesSearchBegan();
    void previousStudiesSearchEnded();

private:
    /// Hanging Protocols
    
    /// Sets up hanging protocols environment
    void setupHangingProtocols();

    /// Searches previous studies and adds hanging protocols candidates which match them
    void searchPreviousStudiesWithHangingProtocols();

    /// Self-explanatory
    void searchAndApplyBestHangingProtocol();

    /// Auto Layouts

    /// Returns a list of StudyLayoutConfig corresponding to the given Patient
    QList<StudyLayoutConfig> getLayoutCandidates(Patient *patient);

    /// Applies the proper layout candidate from the list for the given Patient
    void applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Patient *patient);

private slots:
    /// Searches and adds suitable hanging protocols for the given previous studies
    void addHangingProtocolsWithPrevious(QList<Study*> studies);
    
    /// Sets and applies the hanging protocol with the given identifier
    void setHangingProtocol(int hangingProtocolNumber);

    ///  Searches for hanging protocols for the current patient
    void searchHangingProtocols();

private:
    /// Patient for the layout
    Patient *m_patient;

    /// Layout object
    ViewersLayout *m_layout;

    /// Hanging Protocols and related studies managers
    HangingProtocolManager *m_hangingProtocolManager;
    RelatedStudiesManager *m_relatedStudiesManager;
};

} // end namespace udg

#endif
