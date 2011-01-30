#ifndef UDGQVIEWERWORKINPROGRESSWIDGET_H
#define UDGQVIEWERWORKINPROGRESSWIDGET_H

#include "ui_qviewerworkinprogresswidget.h"

class QMovie;

namespace udg {

/**
  Widget que s'encarrega de visualitzar el progrés d'alguna tasca que hi pugui haver en un QViewer.
  Per exemple, el progrés de la càrrega d'un volum asíncrona, descàrrega de prèvies...
  En el cas que alguna d'aquestes tasques doni error, dona l'opció de mostrar aquests errors en el mateix
  widget.
  */

class QViewerWorkInProgressWidget : public QWidget, private Ui::QViewerWorkInProgressWidget
{
    Q_OBJECT
public:
    explicit QViewerWorkInProgressWidget(QWidget *parent = 0);

    /// Especifica quin és el títol que es posa a la tasca que s'estigui fent
    void setTitle(const QString &text);

    /// Mostra l'error que se li passi. Útil per mostrar a l'usuari si es produeix un error al fer el "work in progress".
    void showError(const QString &errorText);

    /// Reinicia el widget.
    void reset();

public slots:
    /// Actualitza el progrés del "work in progress".
    void updateProgress(int progress);

private:
    /// Activa/desactiva la progressBar depenent de si s'està al mig d'una operació o no
    void startAnimationByProgress(int progress);

    /// Reseteja els widgets involucrats en mostrar el progrés
    void resetProgressWidgets();

private:
    QMovie *m_progressBarAnimation;
};

} // End namespace udg

#endif // UDGQVIEWERWORKINPROGRESSWIDGET_H
