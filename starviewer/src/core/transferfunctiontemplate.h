#ifndef TRANSFERFUNCTIONTEMPLATE_H
#define TRANSFERFUNCTIONTEMPLATE_H


#include "logging.h"
#include "mathtools.h"

#include <QMap>
#include <QString>


namespace udg {


/// Retorna la interpolació lineal entre a i b avaluant "a + alpha * (b - a)", que és equivalent a "a * (1 - alpha) + b * alpha".
template <typename T>
T lerp(const T &a, const T &b, double alpha)
{
    Q_ASSERT(!MathTools::isNaN(alpha));
    return a + alpha * (b - a);
}


/**
    Representa una funció de transferència bàsica f: X -> Y, on X és el conjunt de valors de propietat (reals) i Y un conjunt de valors de tipus T.
    Aquesta funció té uns quants punts definits explícitament i la resta s'obtenen per interpolació lineal o extrapolació del veí més proper. La funció de transferència també té un nom.

    Aquesta template té el constructor protegit, de manera que no es pot instanciar directament, sinó que se n'ha de fer una subclasse per cada tipus T que es vulgui.
 */
template <typename T>
class TransferFunctionTemplate {

public:

    /// Retorna cert si els noms i les funcions són iguals.
    /// \note Les funcions poden ser diferents però equivalents. Són equivalents si un cop simplificades són iguals.
    bool operator ==(const TransferFunctionTemplate<T> &transferFunction) const;

    /// Retorna el nom.
    const QString& name() const;
    /// Assigna el nom.
    void setName(const QString &name);

    /// Retorna la y corresponent al valor de propietat x.
    T operator ()(double x) const;
    /// Retorna la y corresponent al valor de propietat x.
    T get(double x) const;
    /// Defineix explícitament el punt (x,y).
    void operator [](double x);
    /// Defineix explícitament el punt (x,y).
    void set(double x, const T &y);
    /// Esborra la definició explícita del punt (x,y) si existeix.
    void unset(double x);
    /// Esborra tots els punts definits explícitament.
    void clear();

    /// Retorna la llista valors de propietat x de tots els punts (x,y) definits explícitament.
    QList<double> keys() const;

    /// Retalla la funció de manera que només tingui punts explícits en el rang [x1, x2] i tingui punts explícits a x1 i x2.
    void trim(double x1, double x2);
    /// Simplifica la funció esborrant els punts (x,y) que es poden obtenir per interpolació o extrapolació.
    void simplify();

protected:

    /// Crea una funció de transferència buida, sense cap punt i sense nom.
    TransferFunctionTemplate();

protected:

    /// Nom de la funció de transferència.
    QString m_name;
    /// Mapa amb els punts (x,y) definits explícitament.
    QMap<double, T> m_map;

};


template <typename T>
bool TransferFunctionTemplate<T>::operator ==(const TransferFunctionTemplate<T> &transferFunction) const
{
    return m_name == transferFunction.m_name && m_map == transferFunction.m_map;
}


template <typename T>
const QString& TransferFunctionTemplate<T>::name() const
{
    return m_name;
}


template <typename T>
void TransferFunctionTemplate<T>::setName(const QString &name)
{
    m_name = name;
}


template <typename T>
T TransferFunctionTemplate<T>::operator ()(double x) const
{
    return get(x);
}


template <typename T>
T TransferFunctionTemplate<T>::get(double x) const
{
    Q_ASSERT(!MathTools::isNaN(x));

    if (m_map.isEmpty())
    {
        DEBUG_LOG("La funció de transferència és buida. Retornem un valor per defecte.");
        WARN_LOG("La funció de transferència és buida. Retornem un valor per defecte.");
        return T();
    }

    typename QMap<double, T>::const_iterator lowerBound = m_map.lowerBound(x);

    if (lowerBound == m_map.end())  // > últim
        return (--lowerBound).value();

    if (lowerBound.key() == x || lowerBound == m_map.begin())   // exacte o < primer
        return lowerBound.value();

    typename QMap<double, T>::const_iterator a = lowerBound - 1, b = lowerBound;
    double alpha = (x - a.key()) / (b.key() - a.key());

    return lerp(a.value(), b.value(), alpha);
}


template <typename T>
void TransferFunctionTemplate<T>::operator [](double x)
{
    set(x);
}


template <typename T>
void TransferFunctionTemplate<T>::set(double x, const T &y)
{
    //Q_ASSERT(!MathTools::isNaN(x));   // problema amb QTransferFunctionEditorByGradient
    m_map[x] = y;
}


template <typename T>
void TransferFunctionTemplate<T>::unset(double x)
{
    Q_ASSERT(!MathTools::isNaN(x));
    m_map.remove(x);
}


template <typename T>
void TransferFunctionTemplate<T>::clear()
{
    m_map.clear();
}


template <typename T>
QList<double> TransferFunctionTemplate<T>::keys() const
{
    return m_map.keys();
}


template <typename T>
void TransferFunctionTemplate<T>::trim(double x1, double x2)
{
    Q_ASSERT(!MathTools::isNaN(x1));
    Q_ASSERT(!MathTools::isNaN(x2));
    Q_ASSERT(x1 <= x2);

    if (m_map.isEmpty())
    {
        DEBUG_LOG("La funció de transferència és buida.");
        WARN_LOG("La funció de transferència és buida.");
        return;
    }

    // Primer afegim els extrems. Si ja existeixen es quedarà igual.
    set(x1, get(x1));
    set(x2, get(x2));

    // Després eliminem els punts fora del rang.
    QMutableMapIterator<double, T> it(m_map);
    while (it.hasNext())
    {
        it.next();
        if (it.key() < x1) it.remove();
        else break;
    }
    it.toBack();
    while (it.hasPrevious())
    {
        it.previous();
        if (it.key() > x2) it.remove();
        else break;
    }
}


template <typename T>
void TransferFunctionTemplate<T>::simplify()
{
    QList<double> keys = m_map.keys();

    foreach (double x, keys)
    {
        T y = get(x);
        unset(x);
        if (get(x) != y) set(x, y);
    }
}


template <typename T>
TransferFunctionTemplate<T>::TransferFunctionTemplate()
{
}


} // namespace udg


#endif // TRANSFERFUNCTIONTEMPLATE_H
