#ifndef UDGTRANSFERFUNCTION_H
#define UDGTRANSFERFUNCTION_H

#include "colortransferfunction.h"
#include "opacitytransferfunction.h"

class vtkLookupTable;

namespace udg {

/**
    Representa una funció de transferència f: (X,Y) -> (C,O), on X és el conjunt de valors de propietat (reals), Y el conjunt de magnituds del gradient (reals positius),
    C el conjunt de colors (QColors) i O el conjunt d'opacitats (reals en el rang [0,1]).
    En realitat consisteix en una funció de transferència de color fc: X -> C, una d'opacitat escalar fo: X -> O, i una d'opacitat del gradient fg: Y -> O.
    Hi ha uns quants punts definits explícitament i la resta s'obtenen per interpolació lineal o extrapolació del veí més proper.
    Es pot treballar amb el color, l'opacitat escalar i l'opacitat del gradient per separat (opció recomanada) o bé junts. La funció de transferència també té un nom.
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

    /// Retorna la parella de color i opacitat corresponent al valor de propietat x. No té en compte l'opacitat del gradient.
    /// \note L'opacitat queda encapsulada al canal alfa del QColor i amb això perd precisió. Per evitar això es recomana accedir al color i l'opacitat per separat.
    QColor get(double x) const;
    /// Retorna el color corresponent al valor de propietat x.
    QColor getColor(double x) const;
    /// Retorna l'opacitat escalar corresponent al valor de propietat x.
    /// Equivalent a getScalarOpacity(x).
    double getOpacity(double x) const;
    /// Retorna l'opacitat escalar corresponent al valor de propietat x.
    double getScalarOpacity(double x) const;
    /// Retorna l'opacitat del gradient corresponent a la magnitud de gradient y.
    double getGradientOpacity(double y) const;
    /// Retorna el producte de l'opacitat escalar corresponent al valor de propietat x i l'opacitat del gradient corresponent a la magnitud del gradient y.
    double getOpacity(double x, double y) const;
    /// Defineix explícitament el punt (x,(color,opacity)).
    void set(double x, const QColor &color, double opacity);
    /// Defineix explícitament el punt (x,((red,green,blue),opacity)).
    void set(double x, int red, int green, int blue, double opacity);
    /// Defineix explícitament el punt (x,((red,green,blue),opacity)).
    void set(double x, double red, double green, double blue, double opacity);
    /// Defineix explícitament el punt ((x,y),(color,scalarOpacity,gradientOpacity)).
    void set(double x, double y, const QColor &color, double scalarOpacity, double gradientOpacity);
    /// Defineix explícitament el punt ((x,y),((red,green,blue),scalarOpacity,gradientOpacity)).
    void set(double x, double y, int red, int green, int blue, double scalarOpacity, double gradientOpacity);
    /// Defineix explícitament el punt ((x,y),((red,green,blue),scalarOpacity,gradientOpacity)).
    void set(double x, double y, double red, double green, double blue, double scalarOpacity, double gradientOpacity);
    /// Defineix explícitament el punt de color (x,color).
    void setColor(double x, const QColor &color);
    /// Defineix explícitament el punt de color (x,(red,green,blue)).
    void setColor(double x, int red, int green, int blue);
    /// Defineix explícitament el punt de color (x,(red,green,blue)).
    void setColor(double x, double red, double green, double blue);
    /// Defineix explícitament el punt d'opacitat escalar (x,opacity).
    /// Equivalent a setScalarOpacity(x, opacity).
    void setOpacity(double x, double opacity);
    /// Defineix explícitament el punt d'opacitat escalar (x,opacity).
    void setScalarOpacity(double x, double opacity);
    /// Defineix explícitament el punt d'opacitat del gradient (y,opacity).
    void setGradientOpacity(double y, double opacity);
    /// Esborra la definició explícita dels punts de color (x,c) i opacitat escalar (x,o) si existeixen.
    void unset(double x);
    /// Esborra la definició explícita del punt de color (x,c) si existeix.
    void unsetColor(double x);
    /// Esborra la definició explícita del punt d'opacitat escalar (x,o) si existeix.
    /// Equivalent a unsetScalarOpacity(x).
    void unsetOpacity(double x);
    /// Esborra la definició explícita del punt d'opacitat escalar (x,o) si existeix.
    void unsetScalarOpacity(double x);
    /// Esborra la definició explícita del punt d'opacitat del gradient (y,o) si existeix.
    void unsetGradientOpacity(double y);
    /// Esborra tots els punts definits explícitament.
    void clear();
    /// Esborra tots els punts de color definits explícitament.
    void clearColor();
    /// Esborra tots els punts d'opacitat escalar definits explícitament.
    /// Equivalent a clearScalarOpacity().
    void clearOpacity();
    /// Esborra tots els punts d'opacitat escalar definits explícitament.
    void clearScalarOpacity();
    /// Esborra tots els punts d'opacitat del gradient definits explícitament.
    void clearGradientOpacity();
    /// Retorna cert si hi ha un punt de color (x,c) definit explícitament, i fals altrament.
    bool isSetColor(double x) const;
    /// Retorna cert si hi ha un punt d'opacitat escalar (x,o) definit explícitament, i fals altrament.
    /// Equivalent a isSetScalarOpacity(x).
    bool isSetOpacity(double x) const;
    /// Retorna cert si hi ha un punt d'opacitat escalar (x,o) definit explícitament, i fals altrament.
    bool isSetScalarOpacity(double x) const;
    /// Retorna cert si hi ha un punt d'opacitat del gradient (y,o) definit explícitament, i fals altrament.
    bool isSetGradientOpacity(double y) const;

    /// Retorna la llista de valors de propietat x de tots els punts de color (x,c) i opacitat escalar (x,o) definits explícitament.
    const QList<double>& keys() const;
    /// Retorna la llista de valors de propietat x dels punts de color (x,c) i opacitat escalar (x,o) definits explícitament dins de l'interval [begin, end].
    QList<double> keys(double begin, double end) const;
    /// Retorna la llista de valors de propietat x dels punts de color (x,c) i opacitat escalar (x,o) definits explícitament dins de l'interval [x-distance, x+distance].
    QList<double> keysNear(double x, double distance) const;
    /// Retorna la llista de valors de propietat x de tots els punts de color (x,c) definits explícitament.
    QList<double> colorKeys() const;
    /// Retorna la llista de valors de propietat x de tots els punts d'opacitat scalar (x,o) definits explícitament.
    /// Equivalent a scalarOpacityKeys().
    QList<double> opacityKeys() const;
    /// Retorna la llista de valors de propietat x de tots els punts d'opacitat scalar (x,o) definits explícitament.
    QList<double> scalarOpacityKeys() const;
    /// Retorna la llista de valors de propietat y de tots els punts d'opacitat del gradient (y,o) definits explícitament.
    QList<double> gradientOpacityKeys() const;

    /// Retalla la funció de manera que només tingui punts explícits en el rang [x1, x2] i fornçant que en tingui a x1 i x2.
    /// La funció de transferència d'opacitat del gradient es manté.
    void trim(double x1, double x2);
    /// Retorna una nova funció de transferència resultat d'escalar i desplaçar aquesta de manera que el rang [x1, x2] passi a ser [0, 1].
    /// La funció de transferència d'opacitat del gradient es copia directament.
    TransferFunction to01(double x1, double x2) const;
    /// Retorna una versió simplificada i equivalent de la funció esborrant els punts de color (x,c), d'opacitat escalar (x,o) i d'opacitat del gradient (y,o)
    /// que es poden obtenir per interpolació o extrapolació.
    TransferFunction simplify() const;
    /// Retorna una versió equivalent de la funció on tots els punts x definits explícitament són ho són per al color i l'opacitat escalar, és a dir, tots són (x,(c,o)).
    /// La funció de transferència d'opacitat del gradient es copia directament.
    TransferFunction normalize() const;

    /// Retorna la funció de transferència de color.
    const ColorTransferFunction& colorTransferFunction() const;
    /// Assigna la funció de transferència de color.
    void setColorTransferFunction(const ColorTransferFunction &colorTransferFunction);
    /// Retorna la funció de transferència d'opacitat escalar.
    /// Equivalent a scalarOpacityTransferFunction.
    const OpacityTransferFunction& opacityTransferFunction() const;
    /// Assigna la funció de transferència d'opacitat escalar.
    /// Equivalent a setScalarOpacityTransferFunction(opacityTransferFunction).
    void setOpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction);
    /// Retorna la funció de transferència d'opacitat escalar.
    const OpacityTransferFunction& scalarOpacityTransferFunction() const;
    /// Assigna la funció de transferència d'opacitat escalar.
    void setScalarOpacityTransferFunction(const OpacityTransferFunction &scalarOpacityTransferFunction);
    /// Retorna la funció de transferència d'opacitat del gradient.
    const OpacityTransferFunction& gradientOpacityTransferFunction() const;
    /// Assigna la funció de transferència d'opacitat del gradient.
    void setGradientOpacityTransferFunction(const OpacityTransferFunction &gradientOpacityTransferFunction);

    /// Retorna la funció de transferència de color en format VTK.
    ::vtkColorTransferFunction* vtkColorTransferFunction() const;
    /// Retorna la funció de transferència d'opacitat escalar en format VTK.
    /// Equivalent a vtkScalarOpacityTransferFunction().
    vtkPiecewiseFunction* vtkOpacityTransferFunction() const;
    /// Retorna la funció de transferència d'opacitat escalar en format VTK.
    vtkPiecewiseFunction* vtkScalarOpacityTransferFunction() const;
    /// Retorna la funció de transferència d'opacitat del gradient en format VTK.
    vtkPiecewiseFunction* vtkGradientOpacityTransferFunction() const;

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
    /// Funció de transferència d'opacitat escalar.
    OpacityTransferFunction m_scalarOpacity;
    /// Funció de transferència d'opacitat del gradient.
    OpacityTransferFunction m_gradientOpacity;
    /// Llista de valors de propietat x de tots els punts de color (x,c) i opacitat escalar (x,o) definits explícitament.
    mutable QList<double> m_keys;
    /// Indica si hi ha hagut canvis a la funció des de l'últim cop que s'ha actualitzat m_keys.
    mutable bool m_changed;

};

}

#endif
