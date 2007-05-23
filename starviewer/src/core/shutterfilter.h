/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSHUTTERFILTER_H
#define UDGSHUTTERFILTER_H

#include <vector>

class vtkImageData;
class vtkPolyDataToImageStencil;
class vtkAtamaiPolyDataToImageStencil2;

namespace udg {

/**
Filtre per aplicar diversos tipus de shutter a les imatges. Segueix el que es deineix a l'estàndar DICOM.
Poden haver-hi fins a tres shutters superposats ( Cercle, Rectangular i Poligonal ). També es poden aplicar bitmap shutters que hi ha en els overlays.

La manera d'operar és senzilla, primer es dóna un input ( \code vtkImageData ). Després definim els diferents shutters ( SetRectangular/Circular/PolygonalShutter() ) i finalment obtenim el resultat amb \code getOutput().

Cada cop que donem un input s'esborren els shutters definits. També podem esborrar tots els shutters definits amb \code clearAllShutters() o un de concret amb els mètodes més específics clearRectangular/Circular/PolygonalShutter()

De manera alternativa podem fer que s'apliquin els shutters que hi hagi en un presentation state amb \CODE setPresentationStateShutter()

Les coordenades del shutter sempre són referents a l'espai de l'input, per tant coordenades de món.

El shutter s'aplica a totes les imatges del volum !!

\TODO queda per definir com tractar els bitmap shutter

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Volume;

class ShutterFilter{
public:
    ///\TODO Queda pendent determinar correctament l'extrusió. Per una part, la coordenada Z dels punts que formen els polígons de l'extrusió hauria d'estar una llesca més per sota de l'origen i l'scale factor de l'extrusion hauria d'assegurar que el polígon atravessarà totes les llesques del volum
    ShutterFilter();

    ~ShutterFilter();

    enum { CircularShutter, PolygonalShutter, RectangularShutter, BitmapShutter };

    /// Li assignem les dades d'entrada
    void setInput( Volume *volume );

    /// Aplica els shutters definits en l'arxiu de presentation state donat
    void setPresentationStateShutters( const char *presentationStateFile );

    /// Defineix el shutter rectangular
    void setRectangularShutter( double leftVertical, double rightVertical, double upperHorizontal, double lowerHorizontal );

    /// Defineix el shutter poligonal
    void setPolygonalShutter( std::vector< double[2] > vertexs );

    /// Defineix el shutter circular
    void setCircularShutter( double center[2], double radius );

    /// Crea un shutter a partir d'un vtkImageData/array de dades
    void setBitmapShutter( vtkImageData *bitmap );
    void setBitmapShutter( unsigned char *data, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int foreground );

    /// Reinicia tots els shutters
    void clearAllShutters();

    /// Reinicia un shuter en concret
    void clearShutter( int shutterID );
    void clearCircularShutter();
    void clearPolygonalShutter();
    void clearRectangularShutter();
    void clearBitmapShutter();

    /// assigna/obté el color amb que es pinta la part que queda retallada
    void setBackground( double background ){ m_background = background; }
    double getBackground() const { return m_background; }

    /// Obtenim les dades d'input després d'aplicar els shutters definits
    vtkImageData *getOutput();

private:
    /// stencils per retallar les imatges
    vtkPolyDataToImageStencil *m_rectangularPolyDataStencil, *m_circularPolyDataStencil;
    /// Pel cas poligonal fem servir un altre tipu d'stencil ja que si tenim puntes l'anterior es pot comportar malament
    vtkAtamaiPolyDataToImageStencil2 *m_polygonalPolyDataStencil;

    /// Les dades d'input
    Volume *m_inputData;

    /// color amb que es pinta la part que no es veu
    double m_background;
};

}

#endif
