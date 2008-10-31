#include "obscurance.h"

#include <QDataStream>
#include <QFile>

#include "logging.h"
#include "vector3.h"


namespace udg {


Obscurance::Obscurance( unsigned int size, bool color, bool doublePrecision )
 : m_size( size ), m_color( color ), m_doublePrecision( doublePrecision ),
   m_floatObscurance( 0 ), m_doubleObscurance( 0 ), m_floatColorBleeding( 0 ), m_doubleColorBleeding( 0 )
{
    if ( !m_color && !m_doublePrecision ) m_floatObscurance = new float[m_size];
    if ( !m_color && m_doublePrecision ) m_doubleObscurance = new double[m_size];
    if ( m_color && !m_doublePrecision ) m_floatColorBleeding = new Vector3Float[m_size];
    if ( m_color && m_doublePrecision ) m_doubleColorBleeding = new Vector3Double[m_size];
}


Obscurance::~Obscurance()
{
    delete[] m_floatObscurance;
    delete[] m_doubleObscurance;
    delete[] m_floatColorBleeding;
    delete[] m_doubleColorBleeding;
}


void Obscurance::normalize()
{
    if ( !m_color )
    {
        double max = 0.0;

        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( obscurance( i ) > max ) max = obscurance( i );
        }

        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( m_doublePrecision ) m_doubleObscurance[i] /= max;
            else m_floatObscurance[i] /= max;
        }
    }
    else
    {
        double max = 0.0;

        for ( unsigned int i = 0; i < m_size; i++ )
        {
            Vector3 colorBleeding = this->colorBleeding( i );
            if ( colorBleeding.x > max ) max = colorBleeding.x;
            if ( colorBleeding.y > max ) max = colorBleeding.y;
            if ( colorBleeding.z > max ) max = colorBleeding.z;
        }

        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( m_doublePrecision ) m_doubleColorBleeding[i] /= max;
            else m_floatColorBleeding[i] /= max;
        }
    }
}


bool Obscurance::load( const QString &fileName )
{
    QFile file( fileName );

    if ( !file.open( QFile::ReadOnly ) )
    {
        ERROR_LOG( "No es pot llegir el fitxer " + fileName );
        return false;
    }

    QDataStream in( &file );

    if ( !m_color )
    {
        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( in.atEnd() )
            {
                ERROR_LOG( "No hi ha prou dades al fitxer " + fileName );
                file.close();
                return false;
            }
            else if ( m_doublePrecision ) in >> m_doubleObscurance[i];
            else in >> m_floatObscurance[i];
        }

        if ( !in.atEnd() ) WARN_LOG( "Hi ha més dades del compte al fitxer " + fileName );
    }
    else
    {
        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( in.atEnd() )
            {
                ERROR_LOG( "No hi ha prou dades al fitxer " + fileName );
                file.close();
                return false;
            }
            else if ( m_doublePrecision ) in >> m_doubleColorBleeding[i].x;
            else in >> m_floatColorBleeding[i].x;

            if ( in.atEnd() )
            {
                ERROR_LOG( "No hi ha prou dades al fitxer " + fileName );
                file.close();
                return false;
            }
            else if ( m_doublePrecision ) in >> m_doubleColorBleeding[i].y;
            else in >> m_floatColorBleeding[i].y;

            if ( in.atEnd() )
            {
                ERROR_LOG( "No hi ha prou dades al fitxer " + fileName );
                file.close();
                return false;
            }
            else if ( m_doublePrecision ) in >> m_doubleColorBleeding[i].z;
            else in >> m_floatColorBleeding[i].z;
        }

        if ( !in.atEnd() ) WARN_LOG( "Hi ha més dades del compte al fitxer " + fileName );
    }

    file.close();

    return true;
}


bool Obscurance::save( const QString &fileName ) const
{
    QFile file( fileName );

    if ( !file.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        ERROR_LOG( "No es pot escriure el fitxer " + fileName );
        return false;
    }

    QDataStream out( &file );

    if ( !m_color )
    {
        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( m_doublePrecision ) out << m_doubleObscurance[i];
            else out << m_floatObscurance[i];
        }
    }
    else
    {
        for ( unsigned int i = 0; i < m_size; i++ )
        {
            if ( m_doublePrecision ) out << m_doubleColorBleeding[i].x << m_doubleColorBleeding[i].y << m_doubleColorBleeding[i].z;
            else out << m_floatColorBleeding[i].x << m_floatColorBleeding[i].y << m_floatColorBleeding[i].z;
        }
    }

    file.close();

    return true;
}


}
