#ifndef UDGQMESSAGEBOXAUTOCLOSE_H
#define UDGQMESSAGEBOXAUTOCLOSE_H

#include <QMessageBox>
#include <QTimer>
#include <QPushButton>

namespace udg {

/**
  Classe que afegeix la funcionalitat a QMessageBox d'autoclose, que si amb un determinat temps l'usuari no ha premut cap botó del QMessageBox aquest automàticament es tanca.
  Simplement el que s'ha de fer és indicar-li els segons que ha de tardar a tancar-se i a quin botó s'ha de posar el número de segons que queden per tancar-se.
*/

class QMessageBoxAutoClose: public QMessageBox
{
Q_OBJECT
public:
    QMessageBoxAutoClose(int secondsToAutoClose, QWidget *parent = 0);
    QMessageBoxAutoClose(int secondsToAutoClose, Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton, QWidget * parent = 0,
                         Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


    /// Especifiquem en quin botó s'ha de mostrar el comptador endarrera indicant el temps que queda perquè es tanqui el QMessageBox.
    /// Aquest botó quedarà marcat per defecte i serà el que es retornarà com a clickat si l'usuari no prem cap botó
    void setButtonToShowAutoCloseTimer(QPushButton *buttonToShowTimer);

protected:
    /// Activa els timers necessaris per tancar automàticament el QMessageBox i mostra el temps que queda perquè es tanqui al botó especificat
    void showEvent(QShowEvent *event);

    /// Para els timers quan s'amaga el QMessageBox
    void hideEvent(QHideEvent *event);

private:
    /// Inicialitza variables i fa connects
    void initialize(int secondsToAutoClose);

    /// Updata el text del botó especificat a través del mètode setButtonToShwowAutoCloseTimer amb el temps restant per tancar el QMessageBox
    void updateButtonTextWithSecondsToClose();

private slots:
    /// Tanca el QMessageBox quan s'ha esgotat el temps, al tancar-se es click el botó que s'ha passat a través del mètode setButtonToShwowAutoCloseTimer
    void closeQMessageBox();

    /// Refresca el botó especificat amb el mètode setButtonToShwowAutoCloseTimer amb el temps restant per tancar el QMessageBox
    void refreshTimerButtonText();

private:
    /// Conté els segons que es tardarà a tancar el QMessageBox des de que es mostri
    int m_sencondsToAutoClose;
    /// Una vegada s'hagin mostrar el QMessageBox conté els segons que queden per tancar-lo
    int m_secondsLeftToShowInButton;

    //Botó on es mostrarà el temps que queda per tancar el QMessageBox
    QAbstractButton *m_buttonToShowTimer;

    //Conté el text original que tenia el botó on es mostra el temps que queda per tancar el QMessageBox
    QString m_originalTextButtonToShowTimer;

    //Timer que tancarà el QMessageBox
    QTimer m_timerToCloseQMessageBox;

    //Timer que actualitzarà el text del botó cada segon
    QTimer m_timerToRefreshButtonText;
};
}

#endif // QMESSAGEBOXAUTOCLOSE_H
