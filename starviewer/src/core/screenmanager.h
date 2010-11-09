/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGSCREENMANAGER_H
#define UDGSCREENMANAGER_H

#include "dynamicmatrix.h"

class QWidget;
class QDesktopWidget;

namespace udg {

/** Aquesta classe s'utilitza per dues funcionalitats principals, maximitzar una finestra quan hi ha múltiples pantalles,
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
    /// Retrona quantes pantalles hi ha.
    int getNumberOfScreens();

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
    /// Retorna la posició X que ha de tenir la finestra per tal que quedi centrada en la pantalla IdDesktop.
    int centerWidthInto(QWidget *window, int IdDesktop);
    /// Retorna la posició Y que ha de tenir la finestra per tal que quedi centrada en la pantalla IdDesktop.
    int centerHeightInto(QWidget *window, int IdDesktop);

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
    /// Marc superior de la finestra (barra de títols)
    const int TopBorderSize;
    /// Marc lateral i inferior de la finestra
    const int BorderSize;
    /// Quan dues finestres estan a menys de SamePosition de distància es consideren a la mateixa posició
    const int SamePosition;
};

} // end namespace udg

#endif // UDGSCREENMANAGER_H
