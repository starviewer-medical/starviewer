/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEPLANE_H
#define UDGIMAGEPLANE_H

namespace udg {

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

    void setRows( int rows );
    void setColumns( int columns );

    double getRowLength() const;
    double getColumnLength() const;

    bool operator ==(const ImagePlane &imagePlane);

private:
    /// Vectors que defineixen el pla
    double m_rowDirectionVector[3], m_columnDirectionVector[3], m_normal[3];

    /// Origen del pla
    double m_origin[3];

    /// Espaiat de les X i les Y
    double m_spacing[2];

    /// Files i columnes
    int m_rows, m_columns;

};

}

#endif
