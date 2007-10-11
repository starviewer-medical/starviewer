/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "llescacontorn.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "logging.h"

namespace udg {

LlescaContorn::LlescaContorn(ImageType* rea, int proj)
{
        m_finish=false;
	inputImage = rea;

	RegionType reg = inputImage->GetLargestPossibleRegion();
	SizeType   siz   = reg.GetSize();

	c = new contorn;

	pSegSli = new float[siz[0]];
	//atencio amb la direcci�
	projectionDirection = static_cast<unsigned int>( proj );
	for (i = 0, j = 0; i < 3; ++i )
	{

		if (i != projectionDirection)
		{
		direction[j] = i;
		j++;
		}
	}


	requestedRegion = inputImage->GetRequestedRegion();

        //std::cout<<"1"<<std::endl;


	index[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
	index[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
	size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
	size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

	region.SetSize( size );
	region.SetIndex( index );

	//std::cout<<"2"<<std::endl;

	SliceIteratorType  inputIt(  inputImage,  inputImage->GetRequestedRegion() );
	inputIt.SetFirstDirection(  direction[1] );
	inputIt.SetSecondDirection( direction[0] );

	inputIt.GoToBegin();

	float total=0,pixelseg=0,pixelsegsli=0;
	int x=0,y=0;
	int slice=0;
        maxX=0;
        maxY=0;

	//std::cout<<"3"<<std::endl;
	while( !inputIt.IsAtEnd() )
	{
            //std::cout<<"1. Slice= "<<slice<<std::endl;

		while ( !inputIt.IsAtEndOfSlice() )
		{

                   // std::cout<<"2. X= "<<x<<std::endl;
			while ( !inputIt.IsAtEndOfLine() )
			{
                        //std::cout<<"3. Y= "<<y<<std::endl;
			total++;

			if(inputIt.Get()==255){//255

				matriu[x][y][slice]=1;
				pixelseg++;
				pixelsegsli++;
				}
			else{matriu[x][y][slice]=0;
			}
			//std::cout<<"pixel["<<x<<","<<y<<","<<slice<<"]:"<<inputIt.Get();
			++inputIt;
			y++;
			}
		maxY=y;
		y=0;
		inputIt.NextLine();
		x++;
		}
		//std::cout<<"DeterminateContour::DeterminateContour-->llesca"<<slice<<std::endl;
	maxX=x;
	x=0;
	pSegSli[slice] = pixelsegsli;
	pixelsegsli = 0;
	inputIt.NextSlice();
	slice++;

	}

        pTotals = total;
	pSegmen = pixelseg;
	maxSlices = slice;
	//std::cout<<"DeterminateContour::Entraconstructor,maxslices"<<maxSlices<<"PixelsSeg:"<<pSegmen<<std::endl;

}
LlescaContorn::~ LlescaContorn(){

///destruim la matriu
/*	for (i=0; i<siz[1]+1; i++){
              for(j=0;j<siz[2]+1;j++){
		   delete matriu[i][j];
		   }
		delete matriu[i];
	}
	delete matriu;
*/


}

void LlescaContorn::ObtenirContorn(int llesca, int x, int y){


//int y=0;
//int x=0;
bool trobat=false;
                std::cout<<"punt inici: "<<x<<" , "<<y<<std::endl;

                if(matriu[x][y][llesca]==1)///Podem buscar el contorn. el punt era dins una zona segmentada
                {


                                //while((x<=maxX)&&(!trobat))
                                while((x>0)&&(!trobat))
                                {

				if(matriu[x-1][y][llesca]==0)
                                {
					trobat=true;
				}
                                if(!trobat){
			         x--;
                                }
                                }
                                ///SI SORTIM d'aqui vol dir que ja hem trobat el punt, però ho comrobem.


		}else{DEBUG_LOG("NO ES UN PUNT INTERIOR");}
                if(!trobat){
                DEBUG_LOG("NO HEM TROBAT CONTORN");
                }
	        else{
                    std::cout<<"punt final: "<<x<<" , "<<y<<std::endl;
                    ferreseguit(x,y,llesca);

                trobat=false;
		}
}

void LlescaContorn::ferreseguit(int x, int y, int slice){

	int direccio=2;
	int movant[2]={-1,0};
	int inicial[2]={x,y};
	int actual[2]={x,y};
	int evitabucleinf[2]={x,y};
	int anterior=1;
	int aux=0,npunts=0;
	int comencament=0;
        npuntstot=0;
	int prova=0;

	do{
		if(anterior==1){//dins

			if(movant[0]==1){
				movant[1]++;
			}
			else{
				movant[0]=1;
				movant[1]=0;
			}
			if(movant[1]==2){

				switch(direccio){
				case 1:
					actual[1]=actual[1]-1;

				break;
				case 2:
					actual[0]=actual[0]+1;

				break;
				case 3:
					actual[1]=actual[1]+1;

				break;
				case 4:
					actual[0]=actual[0]-1;

				break;
				}
				movant[1]=0;
			}
			else{


			switch(direccio){
				case 1:
					actual[0]=actual[0]-1;
					direccio=4;
				break;
				case 2:
					actual[1]=actual[1]-1;
					direccio=1;
				break;
				case 3:
					actual[0]=actual[0]+1;
					direccio=2;
				break;
				case 4:
					actual[1]=actual[1]+1;
					direccio=3;
				break;
				}
			}
		}
		else{


			if(movant[0]==0){
				movant[1]++;
			}
			else{
				movant[0]=0;
				movant[1]=0;
			}
			if(movant[1]==2){

				switch(direccio){
				case 1:
					actual[1]=actual[1]-1;

				break;
				case 2:
					actual[0]=actual[0]+1;

				break;
				case 3:
					actual[1]=actual[1]+1;

				break;
				case 4:
					actual[0]=actual[0]-1;

				break;
				}
				movant[1]=0;
			}
			else{

			switch(direccio){
					case 1:
						actual[0]=actual[0]+1;
						direccio=2;
					break;
					case 2:
						actual[1]=actual[1]+1;
						direccio=3;
					break;
					case 3:
						actual[0]=actual[0]-1;
						direccio=4;
					break;
					case 4:
						actual[1]=actual[1]-1;
						direccio=1;
					break;
				}
			}


		}

                if(((anterior==0)&&(matriu[actual[0]][actual[1]][slice]==1))||((anterior==1)&&(matriu[actual[0]][actual[1]][slice]==1))){
		        c->x[npuntstot]=actual[0];
			c->y[npuntstot]=actual[1];
			c->z[npuntstot]=slice;
			npuntstot++;
			npunts++;

		}
		anterior=matriu[actual[0]][actual[1]][slice];

		aux++;
		if(aux==2){
			evitabucleinf[0]=x;
			evitabucleinf[1]=y-1;
		}


        }while((((actual[0]!=inicial[0])||(actual[1]!=inicial[1]))&&((actual[0]!=evitabucleinf[0])||(actual[1]!=evitabucleinf[1]))));
        c->puntsContorn=npunts;

	m_finish=true;

}

bool LlescaContorn::ItsFinish(){return m_finish;}

};
