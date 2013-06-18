#ifndef UDGQQNOTIFICATIONPOPUP_H
#define UDGQQNOTIFICATIONPOPUP_H

#include <QDialog>
#include <QPropertyAnimation>
#include "ui_qnotificationpopupbase.h"

namespace udg {
/**
    Generic popup notification dialog
  */
class QNotificationPopup : public QDialog, private Ui::QNotificationPopupBase {
Q_OBJECT
public:
    QNotificationPopup(QWidget *parent = 0);
    ~QNotificationPopup();

    /// Sets the text for the headline and ongoing operation labels
    void setHeadline(const QString &text);
    void setOngoingOperationText(const QString &text);

    /// Shows/hides an animation for giving feedback on ongoing operations
    void showOngoingOperationAnimation();
    void hideOngoingOperationAnimation();
    
    /// Cancels any triggered animation (hide or move)
    void cancelTriggeredAnimations();
    
    /// Sets the delay in milliseconds before the popup is hidden when hideWithDelay() is called. The default value is 5000ms.
    /// Values < 0 are rejected
    void setHideDelay(int delayInMs);

    /// Hides de popup with the given delay in setHideDelay(). A default value of 5000ms is used if none is set.
    void hideWithDelay();

    /// Starts the move animation. Currently it only moves to the bottom right corner of the screen.
    /// If delayed is true, it applies a delay before the animation starts
    void startMoveAnimation(bool delayed = false);
    
    /// True if the popup has been told to make a move animation but is still in the delay period, false otherwise
    bool isMoveAnimationOnDelayPeriod();

protected:
    /// When dialog is shown, move animation with delay is triggered
    void showEvent(QShowEvent *);

    /// If a click event is received, the popup is hidden
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void setupOngoingOperationAnimation();
    void setupTimers();
    void setupHideAnimation();
    void setupMoveAnimation();

private slots:
    /// Starts animation to hide the popup with a vanish effect
    void runSmoothHideAnimation();
    
    /// Starts the move animation
    void runMoveAnimation();

    /// Completes the hide animation started by runSmoothHideAnimation()
    void finishHideAnimation();

private:
    /// Timers users to delay the hiding or move animations of the popup
    QTimer *m_hideDelayTimer;
    QTimer *m_moveAnimationDelayTimer;
    
    /// Delay to apply before the popup is hidden, in milliseconds
    int m_hideDelayInMilliSeconds;
    
    /// Delay to apply before the popup is moved, in milliseconds
    static const int MoveAnimationDelayInMilliSeconds;

    /// Animations used for hiding and moving the popup
    QPropertyAnimation m_hideAnimation;
    QPropertyAnimation m_moveAnimation;

    /// Movie to use an animated gif to give feedback of an ongoing operation
    QMovie *m_ongoingOperationAnimationMovie;
};

} // End namespace udg

#endif
