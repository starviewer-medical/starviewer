/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEPLANE_H
#define UDGIMAGEPLANE_H

#include <QList>
#include <QVector>

namespace udg {

class Image;
/**
Classe que defineix un pla d'imatge. El pla consta de vectors de direcció de X i Y, normal, origen, espaiat i nombre de files i columnes per definir un pla tancat i finit com és el de la imatge. Està definit en coordenades de món.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ImagePlane{
public:
    ImagePlane();

    ~ImagePlane();

    void setRowDirectionVector( const double vector[3] );
    void setRowDirectionVector( double x, double y, double z );
    void setColumnDirectionVector( const double vector[3] );
    void setColumnDirectionVector( double x, double y, double z );

    void getRowDirectionVector( double vector[3] );
    void getColumnDirectionVector( double vector[3] );
    void getNormalVector( double vector[3] );

    void setOrigin( double origin[3] );
    void setOrigin( double x, double y, double z );
    void getOrigin( double origin[3] );

    void setSpacing( double spacing[2] );
    void setSpacing( double x, double y );
    void getSpacing( double spacing[2] );

    void setThickness( double thickness );
    double getThickness() const;

    void setRows( int rows );
    void setColumns( int columns );

    int getRows() const;
    int getColumns() const;

    double getRowLength() const;
    double getColumnLength() const;

    void setSliceLocation( double location );
    double getSliceLocation() const;
    /**
     * Omple les dades del pla a partir d'un objecte Image
     * @param image objecte Image
     * @return Cert si l'objecte Image és vàlid per omplir les dades, fals altrament
     */
    bool fillFromImage( const Image *image );

    bool operator ==(const ImagePlane &imagePlane);
    bool operator !=(const ImagePlane &imagePlane);

    /**
     * Ens retorna una llista amb 4 punts que defineix els bounds del pla.
     * Tenint en compte que la coordenada d'origen s'assumeix que és al centre de la llesca ( és a dir, enmig +/- thickness )
     * Podem obtenir els bounds respecte el centre, thickness per amunt o thickness per avall
     * L'ordre dels punts retornats és el següent:
     * TLHC, TRHC, BRHC, BLHC,
     * On:
     * TLHC == TopLeftHandCorner == Origen
     * TRHC == TopRightHandCorner
     * BRHC == BottomRightHandCorner
     * BLHC == BottomLeftHandCorner
     * @param location defineix quins bounds volem, 0: Central, 1: Upper (+thickness/2), 2: Lower (-thickness/2)
     */
    QList< QVector<double> > getBounds( int location );
    QList< QVector<double> > getCentralBounds();
    QList< QVector<double> > getUpperBounds();
    QList< QVector<double> > getLowerBounds();

    /**
     * Retorna un volcat d'informació de l'objecte en format d'string
     */
    QString toString( bool verbose = false );

private:
    /// Vectors que defineixen el pla
    double m_rowDirectionVector[3], m_columnDirectionVector[3], m_normal[3];

    /// Origen del pla
    double m_origin[3];

    /// Espaiat de les X i les Y
    double m_spacing[2];

    /// Files i columnes
    int m_rows, m_columns;

    /// Gruix del pla
    double m_thickness;

    /// Posició relativa del pla
    double m_sliceLocation;
};

}

#endif
