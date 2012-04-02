#ifndef UDGSCREENMANAGER_H
#define UDGSCREENMANAGER_H

#include "dynamicmatrix.h"

class QWidget;
class QDesktopWidget;
class QRect;

namespace udg {

/**
    Aquesta classe s'utilitza per dues funcionalitats principals, maximitzar una finestra quan hi ha múltiples pantalles,
    i moure una finestra d'una pantalla a una altra.
    El seu funcionament bàsic és a través dels mètodes maximize i moveToDesktop, als que se li passa la finestra a tractar.
  */
class ScreenManager {
public:
    /// Constructor per defecte.
    ScreenManager();
    /// Mètode que maximitza la finestra passada per paràmetres a tantes pantalles com sigui possile per tal de que es vegi bé.
    void maximize(QWidget *window);
    /// Mètode per canviar la finestra especificada per el primer paràmetre a la pantalla amb l'identificador del segon paràmetre.
    void moveToDesktop(QWidget *window, int idDesktop);
    /// Mètode per moure la finestra a la pantalla anterior segons l'identificador de pantalla
    void moveToPreviousDesktop(QWidget *window);
    /// Mètode per moure la finestra a la següent pantalla segons l'identificador de pantalla
    void moveToNextDesktop(QWidget *window);
    ///Restaura la finestra a l'estat d'abans d'estar minimitzada
    void restoreFromMinimized(QWidget *window);

    /// Retrona quantes pantalles hi ha.
    int getNumberOfScreens();
    /// Retorna a quina pantalla està la finestra
    int getIdOfScreen(QWidget *window);

    /// Retorna l'identificador de pantalla segons una posició global
    int getScreenID(const QPoint &point) const;

    /// Retorna la geometria disponible de la pantalla indicada. Depenent de la plataforma el que es considera
    /// dins del disponible pot variar, com per exemple a windows on s'exclouria de l'espai total el que ocupa la barra de tasques
    /// Resultat indefinit si l'identificador de pantalla no es correspon amb cap id vàlid
    QRect getAvailableScreenGeometry(int screenID) const;

    /// Retorna la geometria total de la pantalla indicada.
    QRect getScreenGeometry(int screenID) const;

    /// Retorna la geometria total de la pantalla indicada.
    int getPrimaryScreenID() const;

private:
    /// Calcula la matriu de distribució de les pantalles.
    /// Utilitza la classe DynamicMatrix per crear-se l'estructura de dades per representar la distribució de
    /// les pantalles a l'espai.
    DynamicMatrix computeScreenMatrix(QWidget *window);
    /// Retorna si la finestra passada per paràmetre cap dins la pantalla.
    bool doesItFitInto(QWidget *window, int IdDesktop);
    /// Modifica el tamany de la finestra passada per paràmtre per tal de que càpiga a la pantalla amb id IdDesktop.
    /// A més, centra la finestra.
    void fitInto(QWidget *window, int IdDesktop);

    /// Indica si la primera pantalla està sobre de la segona. Per que sigui cert han de ser de la mateixa amplada
    /// i el top de la primera ha de ser igual al bottom de la segona
    bool isTop(int desktop1, int desktop2);
    /// Indica si la primera pantalla està sota de la segona. Per que sigui cert han de ser de la mateixa amplada
    /// i el bottom de la primera ha de ser igual al top de la segona
    bool isBottom(int desktop1, int desktop2);
    /// Indica si la primera pantalla està a l'esquerra de la segona. Per que sigui cert han de ser de la mateixa alçada
    /// i el right de la primera ha de ser igual al left de la segona
    bool isLeft(int desktop1, int desktop2);
    /// Indica si la primera pantalla està a la dreta de la segona. Per que sigui cert han de ser de la mateixa alçada
    /// i el left de la primera ha de ser igual al right de la segona
    bool isRight(int desktop1, int desktop2);
    /// Indica si la primera pantalla està en diagonal a sobre a l'esquerra de la segona. Per que sigui cert
    /// el punt bottomRight de la primera ha de ser igual al punt topLeft de la segona
    bool isTopLeft(int desktop1, int desktop2);
    /// Indica si la primera pantalla està en diagonal a sobre a la dreta de la segona. Per que sigui cert
    /// el punt bottomLeft de la primera ha de ser igual al punt topRight de la segona
    bool isTopRight(int desktop1, int desktop2);
    /// Indica si la primera pantalla està en diagonal a sota a l'esquerra de la segona. Per que sigui cert
    /// el punt topRight de la primera ha de ser igual al punt bottomLeft de la segona
    bool isBottomLeft(int desktop1, int desktop2);
    /// Indica si la primera pantalla està en diagonal a sota a la dreta de la segona. Per que sigui cert
    /// el punt topLeft de la primera ha de ser igual al punt bottomRight de la segona
    bool isBottomRight(int desktop1, int desktop2);

    /// Indica si la primera pantalla està per sobre de la segona. Comprova que el bottom de la primera estigui
    /// per sobre del top de la segona.
    bool isOver(int desktop1, int desktop2);
    /// Indica si la primera pantalla està per sota de la segona. Comprova que el top de la primera estigui
    /// per sota del bottom de la segona.
    bool isUnder(int desktop1, int desktop2);
    /// Indica si la primera pantalla està a l'esquerra de la segona. Comprova que el right de la primera sigui
    /// menor que el left de la segona.
    bool isOnLeft(int desktop1, int desktop2);
    /// Indica si la primera pantalla està a la dreta de la segona. Comprova que el left de la primera sigui
    /// major que el right de la segona.
    bool isOnRight(int desktop1, int desktop2);

    /// Indica si la primera pantalla està més amunt que la segona. Comprova que el top de la primera estigui
    /// per sobre del top de la segona.
    bool isHigher(int desktop1, int desktop2);
    /// Indica si la primera pantalla està més avall que la segona. Comprova que el top de la primera estigui
    /// per sota del top de la segona.
    bool isLower(int desktop1, int desktop2);
    /// Indica si la primera pantalla està més a l'esquerra que la segona. Comprova que el left de la primera estigui
    /// per l'esquerra del left de la segona.
    bool isMoreOnLeft(int desktop1, int desktop2);
    /// Indica si la primera pantalla està més a la dreta que la segona. Comprova que el left de la primera estigui
    /// per la dreta del left de la segona.
    bool isMoreOnRight(int desktop1, int desktop2);

    /// Retorna l'identificador de la pantalla que hi ha a l'esquerra de la que es passa per paràmetres.
    int whoIsLeft(int desktopIAm);
    /// Retorna l'identificador de la pantalla que hi ha a la dreta de la que es passa per paràmetres.
    int whoIsRight(int desktopIAm);
    /// Retorna l'identificador de la pantalla que hi ha a sobre de la que es passa per paràmetres.
    int whoIsTop(int desktopIAm);
    /// Retorna l'identificador de la pantalla que hi ha a sota de la que es passa per paràmetres.
    int whoIsBottom(int desktopIAm);

    /// Retorna el punt de més adalt a l'esquerra segons la geometria disponible.
    /// (tenint en compte la barra de tasques)
    QPoint getTopLeft(const DynamicMatrix &dynamicMatrix) const;
    /// Retorna el punt de més aball a la dreta segons la geometria disponible.
    /// (tenint en compte la barra de tasques)
    QPoint getBottomRight(const DynamicMatrix &dynamicMatrix) const;

private:
    /// Gestor d'escriptori on es fan les crides referents al múltiples escriptoris o pantalles
    QDesktopWidget *m_applicationDesktop;
    /// Quan dues finestres estan a menys de SamePosition de distància es consideren a la mateixa posició
    const int SamePosition;
};

} // End namespace udg

#endif // UDGSCREENMANAGER_H
