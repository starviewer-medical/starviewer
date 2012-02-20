#ifndef UDGPATIENTBROWSERMENUEXTENDEDITEM_H
#define UDGPATIENTBROWSERMENUEXTENDEDITEM_H

#include <QWidget>
#include <QLabel>

namespace udg {

/**
    Classe per mostrar la informació addicional referent a una serie d'un estudi d'un pacient.
  */
class PatientBrowserMenuExtendedItem : public QFrame {
Q_OBJECT
public:
    PatientBrowserMenuExtendedItem(QWidget *parent = 0);
    ~PatientBrowserMenuExtendedItem();

    // Tractament dels events
    bool event(QEvent *event);

    void setPixmap(const QPixmap &pixmap);
    void setText(const QString &text);

signals:
    /// Signal que s'emet quan es rep l'event de tancar el menu
    void closed();

private:
    /// Crea el widget inicial sense cap informació
    void createInitialWidget();

private:
    /// Label que conté el text addicional a mostrar
    QLabel *m_text;

    /// Label que conté la icona a mostrar
    QLabel *m_icon;
};

}

#endif
