/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOL_H
#define UDGROITOOL_H

#include "tool.h"

#include <QColor>
#include <QMultiMap>
#include <QList>
#include <vtkPolyData.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkCoordinate.h>

class vtkRenderWindowInteractor;
class vtkPoints;
class vtkCellArray;
class vtkLineSource;
class vtkPropPicker;

namespace udg {

class Q2DViewer;
class ROIAssembly;
class CaptionData;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ROITool : public Tool
{
    Q_OBJECT
public:

    ///rois que podem representar
    enum ROIType{ RECTANGLE , CIRCLE , ELLIPSE , POLYLINE , NO_SPECIFIED };
    
    /// estats de la tool
    enum { STOPPED, ANNOTATION_STARTED, SIMULATING_ANNOTATION };
    
    /// enumeració per saber quin dels dos punts de la roi seleccionada és el més proper
    enum { NOTHINGSELECTED, FIRST, SECOND };
    
    ROITool( Q2DViewer *viewer , QObject *parent = 0 , const char *name = 0 );

    ~ROITool();

    ///ens permet assignar el tipus de ROI que volem crear
    void changeROIType( ROIType type )
    { m_ROIType = type; };
    
    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );
    
    ///anota el següent punt de la polilínia
    void annotateNextPolylinePoint();
    
    ///ens permet simular el darrer segment de la polilínia
    void simulationOfNewPolyLinePoint( double *point1, double *point2 );
    
    ///crea l'etiqueta amb la informació de la ROI
    vtkCaptionActor2D* createCaption( double *point, double area/*, double mean*/ );
    
    ///calcula el punt on ha d'anar situada l'etiqueta de la ROI
    double* calculateCaptionPosition(  vtkPolyData* );
    
    ///ens retorna el valor de gris d'un píxel determinat amb coordenades de món.
    double getGrayValue( double* ); 

private:
    
    ///calcula la mitjana dels nivells de gris que tenen els vòxels que estan dins de la ROI
    double computeMean( ROIAssembly* );
    
    ///calcula els punts de la ROI que s'està simulant
    void calculatePointsAccordingSelectedROI(double firstPoint[3], double second_point[3] );
    
    /// Ressalta l'actor 2D que estigui més a prop de la posició actual del cursor
    void highlightNearestROI();
    
    ///ens retorna la ROIAssembly més propera al punt que li passem per paràmetre
    ROIAssembly* getNearestROI( double[3] );

    /// Pinta una ROI del color que li diem
    void setColor( ROIAssembly* roi, QColor color );

    /**
    map de rois:
        Utilitzarem un QMultiMap per guardar les rois. La diferència entre QMap i QMultiMap és que aquest últim permet
        guardar més d'un objecte amb la mateixa clau, és a dir, tenir més d'una entrada per a una mateixa clau. Utilitzarem
        la llesca on s'ha anotat la roi com a clau del QMultiMap i la roi anotada serà la dada. Així, quan volguem
        totes les rois de la llesca "x", crearem una llista amb totes les files que tenen per clau a "x".

        Representació:

        [nº de llesca][   ROI   ]
        [     1      ][   r1    ]
        [     1      ][   r2    ]
        [     2      ][   r3    ]
        [     2      ][   r4    ]
        ...
    */
    QMultiMap<int, ROIAssembly*> m_ROIsOfAxialViewMap;
    QMultiMap<int, ROIAssembly*> m_ROIsOfCoronalViewMap;
    QMultiMap<int, ROIAssembly*> m_ROIsOfSagittalViewMap;

    ///ens determina el tipus de ROI que estem utilitzant
    ROIType m_ROIType;
    
    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;
    
    ///Punts de la roi que anotem
    vtkPoints *m_points;
    
    ///vèrtexs de la ROI
    vtkCellArray *m_vertexs;
    
    ///primer punt de cada nova ROI
    double m_firstPoint[3];

    ///segon punt de cada nova ROI de tipus rectangle, cercle o el·lipse. Aquest atribut no serveix per a les polilínies
    double m_secondPoint[3];
    
    ///objecte per a dibuixar la línia que ens determinarà la roi que estem anotant
    vtkPolyData *m_ROI;

    ///assemblatge que conté la roi que hem seleccionat amb la seva corresponent etiqueta d'anotació
    ROIAssembly *m_selectedROI;

    ///actor per a representar la línia de la roi
    vtkActor2D *m_ROIActor;

    ///mapejador de la línia de roi
    vtkPolyDataMapper2D *m_ROIMapper;
    
    ///mapejador de segments de la polilínia durant la simulació
    vtkPolyDataMapper2D *m_polyLineMapper;
    
    ///actor per a representar segments de la simulació de la polilínia
    vtkActor2D *m_polyLineActor;
    
    ///vtkLineSource per simular el segment de la polilínia que estem fent.
    vtkLineSource *m_segment;

    /// Últim actor que s'ha ressaltat
    ROIAssembly *m_previousHighlightedROI;

    /// Llesca actual on estem pintant les rois. Ens serveix per controlar els actors que s'han de dibuixar o no al canviar de llesca
    int m_currentSlice;
    
    /// atribut per saber quin dels dos punts de la roi seleccionada és el més proper
    int m_nearestPoint;

    /// Manté la última vista del 2DViewer. Ens servirà per controlar les rois visibles quan canviem de vista
    int m_lastView;
    
    ///nou objecte ROIAssembly que creem al començar l'anotació d'una ROI
    ROIAssembly *m_newROIAssembly;
    
    /// Colors dels diferents estat de les rois
    const QColor HighlightColor;
    const QColor NormalColor;
    const QColor SelectedColor;
    
    ///ens guarda el ROIAssembly a la llista que li correspon
    void saveIntoAList( ROIAssembly* );

private slots:
    /// Comença l'anotació de la roi
    void startROIAnnotation();

    /// simula la nova roi
    void doROISimulation();

    /// finalitza l'anotació de la ROI
    void stopROIAnnotation();

    /// Selecciona la roi sobre la que estem, com a efecte de la polsació del botó dret
    void selectROI();

    /// Dibuixa les rois de la llesca que demanem per paràmetre i fa invisibles les altres
    void drawROIsOfSlice( int );

    ///respon als events de teclat
    void answerToKeyEvent();
    
    /// deselecciona la roi que està seleccionada en el moment de cridar l'slot
    void unselectROI();
};


//CLASSE INTERNA DE ROITOOL

class ROIAssembly
{
    ///Ens permet fer agrupacions d'un vtkPolyData i l'actor associat per tenir accés més directe a les dades.
    public:
        
        ROIAssembly()
        {
            m_polyData = vtkPolyData::New(); 
            m_actor = vtkActor2D::New(); 
            m_actor->VisibilityOff();
            m_mapper = vtkPolyDataMapper2D::New();
            m_mapper->SetInput( m_polyData );
            m_actor->SetMapper( m_mapper );
            
            vtkCoordinate *coordinate = vtkCoordinate::New();
            coordinate->SetCoordinateSystemToWorld();
    
            m_mapper->SetTransformCoordinate( coordinate );
    
            coordinate->Delete();
        };
        
        ~ROIAssembly()
        {
            m_polyData->Delete(); 
            m_actor->Delete();
            m_mapper->Delete();
            
            if ( m_caption )
                m_caption->Delete();
        };
        
        //MÈTODES
        ///activa/desactiva la visibilitat de tots els actors de l'assembly
        void setVisible( bool visibility )
        {
            setVisibilityOfActor( visibility );
            setVisibilityOfCaption( visibility );
        }
        
        ///activa/desactiva la visibilitat l'actor
        void setVisibilityOfActor( bool visibility )
        {
            if ( visibility )
                m_actor->VisibilityOn();
            else
                m_actor->VisibilityOff();
        }
        
        ///activa/desactiva la visibilitat del caption
        void setVisibilityOfCaption( bool visibility )
        {
            if ( visibility )
                m_caption->VisibilityOn();
            else
                m_caption->VisibilityOff();
        }
        
        void setShapeType( ROITool::ROIType type )
        {m_shapeType = type;}
        
        ROITool::ROIType getShapeType()
        {return m_shapeType;}
        
        ///ens retorna el caption
        vtkCaptionActor2D* getCaption()
        {return m_caption;}
                
        ///ens retorna la posició del caption
        double* getCaptionPosition()
        {return m_captionPosition;}
        
        ///ens retorna l'actor
        vtkActor2D* getActor()
        {return m_actor;};
        
        ///ens retorna el mapper
        vtkPolyDataMapper2D* getMapper()
        {return m_mapper;};

        ///ens retorna el vtkPolyData
        vtkPolyData* getPolyData()
        {return m_polyData;};
        
        ///ens permet associar l'actor
        void setActor( vtkActor2D *actor )
        {m_actor = actor;};
        
        ///ens permet associar el mapper
        void setMapper( vtkPolyDataMapper2D *mapper )
        {m_mapper = mapper;};
        
        ///ens permet associar el vtkPolyData
        void setPolyData( vtkPolyData *poly )
        {m_polyData = poly;};
        
        ///ens permet assignar el caption
        void setCaption( vtkCaptionActor2D* caption )
        {m_caption = caption;m_captionPosition = caption->GetAttachmentPoint();}
        
        ///ens permet determinar la posició del caption
        void setCaptionPosition( double *position )
        {m_captionPosition = position;}
        
        ///atribut públic perquè d'aquesta manera no hem de tornar a implementar tots els mètodes get/set per accedir a la llista
        QList< double* > m_pointsList;
        
        ///ens capgira els elements de la llista
        void swap()
        {
            for (int i = 0; i < (int)(m_pointsList.count()/2); i++ )
                m_pointsList.swap( i, (m_pointsList.count()-1)-i );
        };
    
    private:

        double *m_captionPosition; 
        vtkActor2D *m_actor;
        vtkPolyDataMapper2D *m_mapper;
        vtkPolyData *m_polyData;
        ROITool::ROIType m_shapeType;
        vtkCaptionActor2D *m_caption;
};

}
#endif
