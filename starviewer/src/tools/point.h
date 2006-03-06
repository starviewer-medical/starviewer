/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOINT_H
#define UDGPOINT_H

namespace udg {

/**
Classe que engloba un punt geomètric,\TODO fer template o fer que sempre per defecte sigui 3D-> fer 3D i ja està
\TODO fer servir vtkCoordinate, ens proporciona diverses tranformacions per als punts segons si referenciem global, viewport, etc...
vtkCoordinate
The coordinate systems in vtk are as follows:

  DISPLAY -             x-y pixel values in window
  NORMALIZED DISPLAY -  x-y (0,1) normalized values
  VIEWPORT -            x-y pixel values in viewport
  NORMALIZED VIEWPORT - x-y (0,1) normalized value in viewport
  VIEW -                x-y-z (-1,1) values in camera coordinates. (z is depth)
  WORLD -               x-y-z global coordinate values
  USERDEFINED -         x-y-z in User defined space

etc etc, per més mirar documentació de la classe...

@author Grup de Gràfics de Girona  ( GGG )
*/
class Point 
{
public:
    Point();
    Point( double points[3] );
    Point( double x , double y , double z );
    ~Point();

    void setX( double x ){ m_x = x; }
    void setY( double y ){ m_y = y; }
    void setZ( double z ){ m_z = z; }
    void setValues( double x , double y , double z );
    void setValues( double points[3] );
    
    double *getValues(){ double points[3]; points[0] = m_x; points[1] = m_y; points[2] = m_z; return points; }
    double getX(){ return m_x; }
    double getY(){ return m_y; }
    double getZ(){ return m_z; }
    double distance( Point p );
    
private:
    /// Coordenades
    double m_x, m_y, m_z;
};

};  //  end  namespace udg 

#endif
