#ifndef UDGTRANSFERFUNCTION_H
#define UDGTRANSFERFUNCTION_H


#include "colortransferfunction.h"
#include "opacitytransferfunction.h"


class vtkLookupTable;


namespace udg {


/**
    Representa una funció de transferència f: X -> (C,O), on X és el conjunt de valors de propietat (reals) i (C,O) el conjunt de parelles de color (QColor) i opacitat (real en el rang [0,1]).
    En realitat consisteix en una funció de transferència de color fc: X -> C i una d'opacitat fo: X -> O.
    Hi ha uns quants punts definits explícitament i la resta s'obtenen per interpolació lineal o extrapolació del veí més proper.
    Es pot treballar amb el color i l'opacitat per separat (opció recomanada) o bé junts. La funció de transferència també té un nom.
 */
class TransferFunction {

public:

    /// Construeix una funció de transferència buida, sense cap punt i sense nom.
    TransferFunction();
    /// Construeix una funció de transferència a partir d'un objecte vtkLookupTable.
    TransferFunction(vtkLookupTable *lookupTable);

    bool operator ==(const TransferFunction &transferFunction) const;

    /// Retorna el nom.
    const QString& name() const;
    /// Assigna el nom.
    void setName(const QString &name);

    /// Retorna la parella de color i opacitat corresponent al valor de propietat x.
    /// \note L'opacitat queda encapsulada al canal alfa del QColor i amb això perd precisió. Per evitar això es recomana accedir al color i l'opacitat per separat.
    QColor get(double x) const;
    /// Retorna el color corresponent al valor de propietat x.
    QColor getColor(double x) const;
    /// Retorna l'opacitat corresponent al valor de propietat x.
    double getOpacity(double x) const;
    /// Defineix explícitament el punt (x,(color,opacity)).
    void set(double x, const QColor &color, double opacity);
    /// Defineix explícitament el punt (x,((red,green,blue),opacity)).
    void set(double x, int red, int green, int blue, double opacity);
    /// Defineix explícitament el punt (x,((red,green,blue),opacity)).
    void set(double x, double red, double green, double blue, double opacity);
    /// Defineix explícitament el punt de color (x,color).
    void setColor(double x, const QColor &color);
    /// Defineix explícitament el punt de color (x,(red,green,blue)).
    void setColor(double x, int red, int green, int blue);
    /// Defineix explícitament el punt de color (x,(red,green,blue)).
    void setColor(double x, double red, double green, double blue);
    /// Defineix explícitament el punt d'opacitat (x,opacity).
    void setOpacity(double x, double opacity);
    /// Esborra la definició explícita dels punts de color (x,c) i opacitat (x,o) si existeixen.
    void unset(double x);
    /// Esborra la definició explícita del punt de color (x,c) si existeix.
    void unsetColor(double x);
    /// Esborra la definició explícita del punt d'opacitat (x,o) si existeix.
    void unsetOpacity(double x);
    /// Esborra tots els punts definits explícitament.
    void clear();
    /// Esborra tots els punts de color definits explícitament.
    void clearColor();
    /// Esborra tots els punts d'opacitat definits explícitament.
    void clearOpacity();

    /// Retorna la llista de valors de propietat x de tots els punts de color (x,c) i opacitat (x,o) definits explícitament.
    QList<double>& keys() const;
    /// Retorna la llista de valors de propietat x dels punts de color (x,c) i opacitat (x,o) definits explícitament dins de l'interval [begin, end].
    QList<double> keys(double begin, double end) const;
    /// Retorna la llista de valors de propietat x dels punts de color (x,c) i opacitat (x,o) definits explícitament dins de l'interval [x-distance, x+distance].
    QList<double> keysNear(double x, double distance) const;
    /// Retorna la llista de valors de propietat x de tots els punts de color (x,c) definits explícitament.
    QList<double> colorKeys() const;
    /// Retorna la llista de valors de propietat x de tots els punts d'opacitat (x,o) definits explícitament.
    QList<double> opacityKeys() const;

    /// Retalla la funció de manera que només tingui punts explícits en el rang [x1, x2] i tingui punts explícits a x1 i x2.
    void trim(double x1, double x2);
    /// Retorna una nova funció de transferència resultat d'escalar i desplaçar aquesta de manera que el rang [x1, x2] passi a ser [0, 1].
    TransferFunction to01(double x1, double x2) const;
    /// Retorna una versió simplificada i equivalent de la funció esborrant els punts de color (x,c) i opacitat (x,o) que es poden obtenir per interpolació o extrapolació.
    TransferFunction simplify() const;
    /// Retorna una versió equivalent de la funció on tots els punts definits explícitament són ho són per al color i l'opacitat, és a dir, tots són (x,(c,o)).
    TransferFunction normalize() const;

    /// Retorna la funció de transferència de color.
    const ColorTransferFunction& colorTransferFunction() const;
    /// Assigna la funció de transferència de color.
    void setColorTransferFunction(const ColorTransferFunction &colorTransferFunction);
    /// Retorna la funció de transferència d'opacitat.
    const OpacityTransferFunction& opacityTransferFunction() const;
    /// Assigna la funció de transferència d'opacitat.
    void setOpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction);

    /// Retorna la funció de transferència de color en format VTK.
    ::vtkColorTransferFunction* vtkColorTransferFunction() const;
    /// Retorna la funció de transferència d'opacitat en format VTK.
    vtkPiecewiseFunction* vtkOpacityTransferFunction() const;

    /// Retorna la funció representada en forma d'string.
    QString toString() const;

    /// Retorna la funció representada en forma de QVariant.
    QVariant toVariant() const;
    /// Retorna la funció representada per variant.
    static TransferFunction fromVariant(const QVariant &variant);

private:

    /// Actualitza m_keys si hi ha hagut canvis a la funció.
    void updateKeys() const;

private:

    /// Funció de transferència de color.
    ColorTransferFunction m_color;
    /// Funció de transferència d'opacitat.
    OpacityTransferFunction m_opacity;
    /// Llista de valors de propietat x de tots els punts de color (x,c) i opacitat (x,o) definits explícitament.
    mutable QList<double> m_keys;
    /// Indica si hi ha hagut canvis a la funció des de l'últim cop que s'ha actualitzat m_keys.
    mutable bool m_changed;

};


}


#endif
