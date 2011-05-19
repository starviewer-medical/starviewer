#ifndef COLORTRANSFERFUNCTION_H
#define COLORTRANSFERFUNCTION_H

#include "transferfunctiontemplate.h"

class QColor;
class QVariant;

class vtkColorTransferFunction;

namespace udg {

/// Retorna la interpolació lineal entre a i b avaluant "a + alpha * (b - a)", que és equivalent a "a * (1 - alpha) + b * alpha".
template <>
QColor TransferFunctionTemplate<QColor>::linearInterpolation(const QColor &a, const QColor &b, double alpha);

/**
    Representa una funció de transferència de color f: X -> C, on X és el conjunt de valors de propietat (reals) i C el conjunt de colors (QColors).
    Aquesta funció té uns quants punts definits explícitament i la resta s'obtenen per interpolació lineal o extrapolació del veí més proper.
    La funció de transferència també té un nom.
 */
class ColorTransferFunction : public TransferFunctionTemplate<QColor> {

public:

    /// Crea una funció de transferència buida, sense cap punt i sense nom.
    ColorTransferFunction();
    ColorTransferFunction(const ColorTransferFunction &colorTransferFunction);
    ~ColorTransferFunction();

    ColorTransferFunction& operator =(const ColorTransferFunction &colorTransferFunction);

    /// Defineix explícitament el punt (x,color).
    // Cal redefinir-lo si es defineixen les altres variants
    void set(double x, const QColor &color);
    /// Defineix explícitament el punt (x,(red,green,blue)).
    void set(double x, int red, int green, int blue);
    /// Defineix explícitament el punt (x,(red,green,blue)).
    void set(double x, double red, double green, double blue);

    /// Retorna la funció de transferència de color en format VTK.
    ::vtkColorTransferFunction* vtkColorTransferFunction() const;

    /// Retorna la funció representada en forma d'string.
    QString toString() const;

    /// Retorna la funció representada en forma de QVariant.
    QVariant toVariant() const;
    /// Retorna la funció representada per variant.
    static ColorTransferFunction fromVariant(const QVariant &variant);

private:

    /// Funció de transferència de color en format VTK.
    // S'ha de guardar per poder fer el Delete() més tard
    mutable ::vtkColorTransferFunction *m_vtkColorTransferFunction;

};

} // End namespace udg

#endif // COLORTRANSFERFUNCTION_H
