#ifndef UDGPATIENTBROWSERMENUEXTENDEDINFO_H
#define UDGPATIENTBROWSERMENUEXTENDEDINFO_H

#include <QFrame>

class QVBoxLayout;

namespace udg {

class PatientBrowserMenuExtendedItem;

/**
    Class to show additional information for the patient browser menu.
  */
class PatientBrowserMenuExtendedInfo : public QFrame {
Q_OBJECT
public:
    PatientBrowserMenuExtendedInfo(QWidget *parent = 0);
    ~PatientBrowserMenuExtendedInfo();

    /// Event handler
    bool event(QEvent *event);

    /// Sets the items to be shown
    void setItems(const QList<PatientBrowserMenuExtendedItem *> &items);

signals:
    /// Signal emitted when a close event is received
    void closed();

private:
    /// Helper method to delete all layout's children
    void clearLayout(QLayout *layout, bool deleteWidgets = true);

private:
    /// Layout
    QVBoxLayout *m_layout;
};

}

#endif
