#ifndef UDGOBSCURANCE_H
#define UDGOBSCURANCE_H


#include "vector3.h"


namespace udg {


/**
 * Classe que encapsula les obscurances.
 */
class Obscurance {

public:

    Obscurance( unsigned int size, bool color = false, bool doublePrecision = true );
    ~Obscurance();

    /// Retorna la mida de les obscurances.
    unsigned int size() const;
    /// Retorna fals si són obscurances i cert si és color bleeding.
    bool hasColor() const;
    /// Retorna fals si són floats i cert si són doubles.
    bool isDoublePrecision() const;

    /// Normalitza les obscurances.
    void normalize();

    /// Retorna l'array d'obscurança amb floats (0 si no existeix).
    float* floatObscurance() const;
    /// Retorna l'array d'obscurança amb doubles (0 si no existeix).
    double* doubleObscurance() const;
    /// Retorna l'array de color bleeding amb floats (0 si no existeix).
    Vector3Float* floatColorBleeding() const;
    /// Retorna l'array de color bleeding amb doubles (0 si no existeix).
    Vector3Double* doubleColorBleeding() const;

    /// Retorna l'obscurança de la posició \a i (independent de la precisió).
    double obscurance( int i ) const;
    /// Assigna l'obscurança de la posició \a i (independent de la precisió).
    void setObscurance( int i, double obscurance );
    /// Afegeix obscurança a la posició \a i (independent de la precisió).
    void addObscurance( int i, double obscurance );
    /// Retorna el color bleeding de la posició \a i (independent de la precisió).
    Vector3 colorBleeding( int i ) const;
    /// Assigna el color bleeding de la posició \a i (independent de la precisió).
    void setColorBleeding( int i, const Vector3 &colorBleeding );
    /// Afegeix color bleeding a la posició \a i (independent de la precisió).
    void addColorBleeding( int i, const Vector3 &colorBleeding );

    /// Carrega les obscurances des d'un fitxer.
    void load( const QString &fileName );
    /// Desa les obscurances a un fitxer.
    void save( const QString &fileName ) const;

private:

    /// Mida de les obscurances.
    unsigned int m_size;
    /// Fals si són obscurances i cert si és color bleeding.
    bool m_color;
    /// Fals si són floats i cert si són doubles.
    bool m_doublePrecision;

    /// Obscurances floats.
    float *m_floatObscurance;
    /// Obscurances doubles.
    double *m_doubleObscurance;
    /// Color bleeding floats.
    Vector3Float *m_floatColorBleeding;
    /// Color bleeding doubles.
    Vector3Double *m_doubleColorBleeding;

};


inline unsigned int Obscurance::size() const
{
    return m_size;
}


inline bool Obscurance::hasColor() const
{
    return m_color;
}


inline bool Obscurance::isDoublePrecision() const
{
    return m_doublePrecision;
}


inline float* Obscurance::floatObscurance() const
{
    return m_floatObscurance;
}


inline double* Obscurance::doubleObscurance() const
{
    return m_doubleObscurance;
}


inline Vector3Float* Obscurance::floatColorBleeding() const
{
    return m_floatColorBleeding;
}


inline Vector3Double* Obscurance::doubleColorBleeding() const
{
    return m_doubleColorBleeding;
}


inline double Obscurance::obscurance( int i ) const
{
    if ( m_doublePrecision ) return m_doubleObscurance[i];
    else return m_floatObscurance[i];
}


inline void Obscurance::setObscurance( int i, double obscurance )
{
    if ( m_doublePrecision ) m_doubleObscurance[i] = obscurance;
    else m_floatObscurance[i] = obscurance;
}


inline void Obscurance::addObscurance( int i, double obscurance )
{
    if ( m_doublePrecision ) m_doubleObscurance[i] += obscurance;
    else m_floatObscurance[i] += obscurance;
}


inline Vector3 Obscurance::colorBleeding( int i ) const
{
    if ( m_doublePrecision ) return m_doubleColorBleeding[i];
    else return m_floatColorBleeding[i];
}


inline void Obscurance::setColorBleeding( int i, const Vector3 &colorBleeding )
{
    if ( m_doublePrecision ) m_doubleColorBleeding[i] = colorBleeding;
    else m_floatColorBleeding[i] = colorBleeding;
}


inline void Obscurance::addColorBleeding( int i, const Vector3 &colorBleeding )
{
    if ( m_doublePrecision ) m_doubleColorBleeding[i] += colorBleeding;
    else m_floatColorBleeding[i] += colorBleeding;
}


}


#endif
