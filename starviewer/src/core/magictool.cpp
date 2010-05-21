/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "magictool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawer.h"
#include "drawertext.h"
#include "drawerpolygon.h"

#include <QPointer>
#include <QMessageBox>


//vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>

namespace udg {

MagicTool::MagicTool( QViewer *viewer, QObject *parent )
: Tool(viewer,parent)
{
    m_magicSize  = 3;
	m_magicFactor = 3.0;
	m_lowerlevel = 0;
	m_upperlevel = 0;
    m_toolName = "MagicTool";

	//En aquest text hi visualitzem el magic factor, si cal
	m_text = new DrawerText;
	m_text->setVisibility(false);

	m_mainPolygon = NULL;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    // ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT( m_2DViewer );

	//TODO: Buscar una icona nova!!
    m_2DViewer->setCursor( QCursor( QPixmap(":/images/repeat.png") ) );
    this->initialize();
}

MagicTool::~MagicTool()
{
	//no fem el delete perquè si no destruiríem la línia
    //if ( !m_mainPolygon.isNull() )
        //delete m_mainPolygon;

	m_2DViewer->setCursor(Qt::ArrowCursor);
}

void MagicTool::initialize()
{
}

void MagicTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        if(m_2DViewer->getInput()!=0)
        {
			if(m_2DViewer->getView() != Q2DViewer::Axial)
			{
				DEBUG_LOG("ERROR: This tool can only be used in the acquisition direction");
			}
			else
			{
				this->setMagicPoint(  );
			}
        }
    break;

    case vtkCommand::MouseMoveEvent:
		if(m_text->isVisible() && m_2DViewer->getInput()!=0)
        {
			m_2DViewer->getDrawer()->erasePrimitive( m_text );
        }
    break;

    case vtkCommand::KeyPressEvent:
    {
        int key = m_viewer->getInteractor()->GetKeyCode();
        // '+' = key code 43
        // '-' = key code 45
        switch( key )
        {
        case 43: // '+'
            this->increaseMagicSize();
        break;

        case 45: // '-'
            this->decreaseMagicSize();
        break;
        }
    }
    break;

    default:
    break;
    }
}

void MagicTool::setMagicPoint(  )
{
    double pos[3];
    if( m_2DViewer->getCurrentCursorImageCoordinate(pos) )
    {
	    int index[3];
	    double origin[3];
		double spacing[3];
	    m_2DViewer->getInput()->getSpacing(spacing);
		m_2DViewer->getInput()->getOrigin(origin);
	    index[0]=(int)((((double)pos[0]-origin[0])/spacing[0])+0.5);
	    index[1]=(int)((((double)pos[1]-origin[1])/spacing[1])+0.5);
	    index[2]=m_2DViewer->getCurrentSlice();
	    
		//Calculem la desviació estàndard dins la finestra que ens marca la magic size
		double stdv = getStandardDeviation(index[0],index[1],index[2]);

		
		//Calculem els llindars com el valor en el pixel +/- la desviació estàndard * magic factor
		Volume::VoxelType *value = m_2DViewer->getInput()->getScalarPointer(index[0],index[1],index[2]);
		m_lowerlevel = (*value) - m_magicFactor*stdv;
		m_upperlevel = (*value) + m_magicFactor*stdv;

		//Creem la màscara
	    int ext[6];
		m_2DViewer->getInput()->getWholeExtent(ext);
		if(ext[0]==0 && ext[2]==0)
		{
			m_mask=QVector<bool>((ext[1]+1)*(ext[3]+1),false);
		}else{
			DEBUG_LOG("ERROR: extension no comença a 0");
		}

		//Posem a true els punts on la imatge està dins els llindard i connectat amb la llavor (region growing)
		//this->paintRegionMask();
		this->paintRegionMask();

		//Trobem els punts frontera i creem el polígon
		this->detectBorder();

		//Refresquem el viewer (no cal)
        //m_2DViewer->refresh();
    }
}

void MagicTool::paintRegionMask()
{
    double pos[3];
    double origin[3];
    double spacing[3];
    int index[3];
    int ext[6];
    int i;
    bool trobat;
    QVector<int> movements;
    m_2DViewer->getInput()->getWholeExtent(ext);

	m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getInput()->getSpacing(spacing);
    m_2DViewer->getInput()->getOrigin(origin);
    index[0]=(int)((((double)pos[0]-origin[0])/spacing[0])+0.5);
    index[1]=(int)((((double)pos[1]-origin[1])/spacing[1])+0.5);
    index[2]=m_2DViewer->getCurrentSlice();

	int a=index[0];
	int b=index[1];
	int c=m_2DViewer->getCurrentSlice();
    //m_2DViewer->getInput()->getWholeExtent(m_ext);
	m_input = m_2DViewer->getInput()->getVtkData();
	//m_i=0;

    //Initial voxel
	Volume::VoxelType *value = (Volume::VoxelType *)m_input->GetScalarPointer(a,b,c);
    if (((*value) >= m_lowerlevel)&&((*value) <= m_upperlevel)){
        m_mask[b*ext[1]+a] = true;
    }else{
        return;
    }
    //First movement
    i = 0;
    trobat = false;
    while(i<4 && !(trobat))
    {
        this->doMovement(a,b,i);
    	value = (Volume::VoxelType *)m_input->GetScalarPointer(a,b,c);
        if (((*value) >= m_lowerlevel)&&((*value) <= m_upperlevel)){
            m_mask[b*ext[1]+a] = true;
            trobat = true;
            movements.push_back(i);
        }
        if(!trobat) this->undoMovement(a,b,i);
        i++;
    }

    //main loop
    i = 0;
    while(movements.size()>0)
    {
        trobat = false;
        while(i<4 && !(trobat))
        {
            this->doMovement(a,b,i);
            if((a > ext[0])&&(a < ext[1])&&(b > ext[2])&&(b < ext[3]))
            {
    	        value = (Volume::VoxelType *)m_input->GetScalarPointer(a,b,c);
                if (((*value) >= m_lowerlevel)&&((*value) <= m_upperlevel)&&(!m_mask[b*ext[1]+a])){
                    m_mask[b*ext[1]+a] = true;
                    trobat = true;
                    movements.push_back(i);
                    i = 0;
                }
            }
            if(!trobat){
                this->undoMovement(a,b,i);
                i++;
            }
        }
        if(!trobat){
            this->undoMovement(a,b,movements.back());
            i=movements.back();
            i++;
            movements.pop_back();
        }
    }
}

void MagicTool::doMovement(int &a, int &b, int movement)
{
    switch(movement)
    {
        case 0: //up
            a++;
            break;
        case 1://down
            a--;
            break;
        case 2:
            b++;
            break;
        case 3:
            b--;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicTool::undoMovement(int &a, int &b, int movement)
{
    switch(movement)
    {
        case 0: //up
            a--;
            break;
        case 1://down
            a++;
            break;
        case 2:
            b--;
            break;
        case 3:
            b++;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}



void MagicTool::detectBorder()
{
    int i,j;
    int index[3];
    int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
    index[2]=m_2DViewer->getCurrentSlice();

	//Busquem el primer punt
	bool trobat = false;
	i=ext[0];
	while((i<=ext[1])&&!(trobat))
    {
		j=ext[2];
		while((j<=ext[3])&&!(trobat))
        {
            if(m_mask[j*ext[1]+i]) trobat=true;
			j++;
        }
		i++;
    }
	//L'índex és -1 pq els hem incrementat una vegada més
	index[0]=i-1;
	index[1]=j-1;
    
	m_mainPolygon = new DrawerPolygon;
    double point[3];
    double initialPoint[3];

	int initialIndex[3];
    int nextIndex[3];
	int direction=0;
	initialIndex[0]=index[0];
	initialIndex[1]=index[1];
	initialIndex[2]=index[2];
	nextIndex[2]=index[2];

    double origin[3];
	double spacing[3];
    m_2DViewer->getInput()->getSpacing(spacing);
	m_2DViewer->getInput()->getOrigin(origin);
	point[0]=(index[0]-0.5)*spacing[0]+origin[0];
	point[1]=(index[1])*spacing[1]+origin[1];
	point[2]=(index[2])*spacing[2]+origin[2];
	initialPoint[0]=point[0];
	initialPoint[1]=point[1];

	m_mainPolygon->addVertix(point);
	point[0]=(index[0])*spacing[0]+origin[0];
	point[1]=(index[1]-0.5)*spacing[1]+origin[1];
	m_mainPolygon->addVertix(point);

	bool loop = false;
	bool next = false;
	while(!loop){
		this->getNextIndex(direction,index[0],index[1],nextIndex[0],nextIndex[1]);
		next = m_mask[nextIndex[1]*ext[1]+nextIndex[0]];
		while((!next) && (!loop)){
			if(((direction%2)!=0)&&(!next))
			{
				this->addPoint(direction,index[0],index[1],point[2]);
				loop = this->isLoopReached();
			}
			direction = this->getNextDirection(direction);
			this->getNextIndex(direction,index[0],index[1],nextIndex[0],nextIndex[1]);
			next = m_mask[nextIndex[1]*ext[1]+nextIndex[0]];
		}	
		index[0]=nextIndex[0];
		index[1]=nextIndex[1];
		direction = this->getInverseDirection(direction);
		direction = this->getNextDirection(direction);
	}

	m_2DViewer->getDrawer()->draw( m_mainPolygon , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

	DrawerText * text = new DrawerText;

    QString areaUnits;
    if ( spacing[0] == 0.0 && spacing[1] == 0.0 )
        areaUnits = "px2";
    else        
        areaUnits = "mm2";

	float area = m_mainPolygon->computeArea( m_2DViewer->getView() );
    text->setText( tr("Area: %1 %2").arg( area, 0, 'f', 0 ).arg(areaUnits) );

	double pos[3];
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    text->setAttachmentPoint( pos );
    m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );


}

void MagicTool::getNextIndex(int direction,int x1,int y1,int &x2,int &y2)
{
	switch(direction)
	{
	case 0: 
		x2=x1-1;
		y2=y1-1;
		break;
	case 1: 
		x2=x1;
		y2=y1-1;
		break;
	case 2: 
		x2=x1+1;
		y2=y1-1;
		break;
	case 3: 
		x2=x1+1;
		y2=y1;
		break;
	case 4: 
		x2=x1+1;
		y2=y1+1;
		break;
	case 5: 
		x2=x1;
		y2=y1+1;
		break;
	case 6: 
		x2=x1-1;
		y2=y1+1;
		break;
	case 7: 
		x2=x1-1;
		y2=y1;
		break;
	default:
		DEBUG_LOG("ERROR: This direction doesn't exist");
	}
			
}
int MagicTool::getNextDirection(int direction)
{
	int a = direction + 1;
	if(a==8) return 0;
	return a;
}

int MagicTool::getInverseDirection(int direction)
{
	return (direction + 4) % 8;
}

void MagicTool::addPoint(int direction,int x1,int y1, double z1)
{
    double point[3];
	point[2]=z1;
    double origin[3];
	double spacing[3];
    m_2DViewer->getInput()->getSpacing(spacing);
	m_2DViewer->getInput()->getOrigin(origin);

	switch(direction)
	{
	case 1: 
		point[0]=(x1)*spacing[0]+origin[0];
		point[1]=(y1-0.5)*spacing[1]+origin[1];
		break;
	case 3: 
		point[0]=(x1+0.5)*spacing[0]+origin[0];
		point[1]=(y1)*spacing[1]+origin[1];
		break;
	case 5: 
		point[0]=(x1)*spacing[0]+origin[0];
		point[1]=(y1+0.5)*spacing[1]+origin[1];
		break;
	case 7: 
		point[0]=(x1-0.5)*spacing[0]+origin[0];
		point[1]=(y1)*spacing[1]+origin[1];
		break;
	default:
		DEBUG_LOG("ERROR: This direction doesn't exist");
	}
	m_mainPolygon->addVertix(point);
}


bool MagicTool::isLoopReached()
{
	const double* pini; //= new double[2];
	const double* pend; //= new double[2];
	pini = this->m_mainPolygon->getVertix( 0 );
	pend = this->m_mainPolygon->getVertix( m_mainPolygon->getNumberOfPoints()-1 );
	return ((fabs(pini[0]-pend[0])<0.0001)&&(fabs(pini[1]-pend[1])<0.0001));
}



double MagicTool::getStandardDeviation(int a, int b, int c)
{
	int ext[6];
    m_2DViewer->getInput()->getWholeExtent(ext);
	Volume::VoxelType *value;
    int index[3];
	index[2]=c;
	double mean = 0.0;
	double deviation = 0.0;

	int minx,maxx,miny,maxy;

	if(a-m_magicSize > ext[0])
	{
		minx = a-m_magicSize;
	}else{
		minx = ext[0];
	}
	if(a+m_magicSize < ext[1])
	{
		maxx = a+m_magicSize;
	}else{
		maxx = ext[1];
	}
	if(b-m_magicSize > ext[2])
	{
		miny = b-m_magicSize;
	}else{
		miny = ext[2];
	}
	if(b+m_magicSize < ext[3])
	{
		maxy = b+m_magicSize;
	}else{
		maxy = ext[3];
	}


	int i,j;	
    for(i=minx;i<=maxx;i++)
    {
        for(j=miny;j<=maxy;j++)
        {
            index[0]=i;
            index[1]=j;
            value = m_2DViewer->getInput()->getScalarPointer(index);
			mean+=(*value);
        }
    }
	int numberofsamples = (maxx - minx + 1)*(maxy - miny + 1);
	mean = mean/(double)numberofsamples;
    for(i=minx;i<=maxx;i++)
    {
        for(j=miny;j<=maxy;j++)
        {
            index[0]=i;
            index[1]=j;
            value = m_2DViewer->getInput()->getScalarPointer(index);
			deviation+=((double)(*value)-mean)*((double)(*value)-mean);
        }
    }
	deviation = sqrt(deviation/(double)numberofsamples);
	return deviation;
}

void MagicTool::increaseMagicSize()
{
    m_magicFactor += 0.1;
	//L'esborrem si ja l'havíem pintat
	if(m_text->isVisible() && m_2DViewer->getInput()!=0)
    {
		m_2DViewer->getDrawer()->erasePrimitive( m_text );
    }
    m_text->setText( tr("Magic Factor: %1").arg( m_magicFactor ) );
	double pos[3];
	m_2DViewer->getCurrentCursorImageCoordinate(pos);
	m_text->setAttachmentPoint( pos );
	m_text->setVisibility(true);
    m_2DViewer->getDrawer()->draw( m_text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

void MagicTool::decreaseMagicSize()
{
    if(m_magicSize > 0)
    {
	    m_magicFactor -= 0.1;
    }
	//L'esborrem si ja l'havíem pintat
	if(m_text->isVisible() && m_2DViewer->getInput()!=0)
    {
		m_2DViewer->getDrawer()->erasePrimitive( m_text );
    }
    m_text->setText( tr("Magic Factor: %1").arg( m_magicFactor ) );
	double pos[3];
	m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_text->setAttachmentPoint( pos );
	m_text->setVisibility(true);
    m_2DViewer->getDrawer()->draw( m_text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

}

