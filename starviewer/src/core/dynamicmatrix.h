#ifndef UDGDYNAMICMATRIX_H
#define UDGDYNAMICMATRIX_H

#include <QList>

class QPoint;

namespace udg {

/** Aquesta classe representa una matriu dinàmica d'enters, la qual permet afegir i obtenir valors sense tenir en compte
    passar-se de rang. Està feta a partir d'una llista de llistes d'enters i dos punters, un per les files i un per
    les columnes, que indiquen a quina fila o columna es troba el 0.

    Així, si s'afegeix un element a la posició (fila -1, columna 0) el que es fara és afegir una QList a la primera posició
    de la llista, i incrementar el punter de files.

    Per afegir un element a la posició (fila x, columna -1), s'hi afegirà un -1 al principi de cada QList de la llista i modificarà
    el punter de les columnes.
 */
class DynamicMatrix {
public:
    /// Constructor per defecte.
    DynamicMatrix();
    /// Posa un valor a una fila i columna de la matriu, no sortirà mai de rang.
    void setValue(int row, int column, int value);
    /// Retorna el valor de la matriu de la posició fila i columna, si no existeix retorna un -1.
    int getValue(int row, int column) const;

    /// Mostra per debug el contingut de la matriu.
    void print();

    /// Retorna una llista amb tots els identificadors de les pantalles de la primera columna.
    QList<int> getLeftColumn() const;
    /// Retorna una llista amb tots els identificadors de les pantalles de la última columna.
    QList<int> getRightColumn() const;
    /// Retorna una llista amb tots els identificadors de les pantalles de la fila superior.
    QList<int> getTopRow() const;
    /// Retorna una llista amb tots els identificadors de les pantalles de la fila inferior.
    QList<int> getBottomRow() const;

    /// Mètode que indica si es pot maximitzar la finestra a més d'una pantalla (el crida ScreenManager).
    bool isMaximizable() const;
    /// Retorna el nombre de columnes que te la fila més llarga.
    int getNumberOfColumns() const;

private:
    /// La llista de llistes que guarda l'estructura de la matriu.
    QList<QList<int> > m_matrix;
    /// Indica on esta la posició de la fila 0 a la llista de llistes.
    int m_indexRow;
    /// Indica on esta la posició de la columna 0 a la llista de llistes.
    int m_indexColumn;
    /// Nombre de columnes que te la fila més llarga.
    int m_columnCount;
};

} // End namespace udg

#endif // UDGDYNAMICMATRIX_H
