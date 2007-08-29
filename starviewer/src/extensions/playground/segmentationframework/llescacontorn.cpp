/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "llescacontorn.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "iostream.h"

namespace udg {

LlescaContorn::LlescaContorn(ImageType* rea, int proj)
{
        m_finish=false;
	inputImage = rea;
	
	RegionType reg = inputImage->GetLargestPossibleRegion();
	SizeType   siz   = reg.GetSize();
	//std::cout<<"tamanyx,y,y"<<siz[0]<<"y"<<siz[1]<<"z"<<siz[2]<<std::endl;
	//std::cout<<"0"<<std::endl;
	
	/*matriu=(int ***)malloc(siz[1]*sizeof(int **)); 
          for (i=0; i<siz[1]+1; i++){ 
               matriu[i]=(int **)malloc(siz[2]*sizeof(int *)); 
		for(j=0;j<siz[2]+1;j++)
		   matriu[i][j]=(int *)malloc(siz[0]*sizeof(int));
		} 
	*/
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
                    

		}else{std::cout<<"NO ES UN PUNT INTERIOR"<<std::endl;}
                if(!trobat){
                std::cout<<"NO HEM TROBAT CONTORN"<<std::endl;
                }
	        else{
                    std::cout<<"punt final: "<<x<<" , "<<y<<std::endl;
                    ferreseguit(x,y,llesca);
                    //std::cout<<"acaba"<<std::endl;
                    /*for(int i=0;i<100;i++){
                            std::cout<<"deter3trobat["<<i<<"][0]:"<<c->npslice[i][0]<<std::endl;
                            std::cout<<"deter3trobat["<<i<<"][1]:"<<c->npslice[i][1]<<std::endl;
                    }*/
                    //quanteslices++;
                    //std::cout<<"QuantesSlice: "<<quanteslices<<std::endl;
                //}
                    
                //x=0;
                //y=0;
                trobat=false;
		}


                
                /*
		while((y<=maxY)&&(!trobat))  
                {
		
			while((x<=maxX)&&(!trobat))
                        {
			
				if(matriu[x][y][llesca]==1)
                                {
					trobat=true;
				}
			         x++;
                        }
		
                        if(!trobat)
                        {
                           x=0;
                        }	
		
		y++;
	        }
	        if(!trobat){
                y=0;
                std::cout<<"No hi ha píxels segmentats en aquesta slice"<<std::endl;
                }
	        else{
                
                    ferreseguit(x-1,y-1,llesca);
                   
                    //quanteslices++;
                    //std::cout<<"QuantesSlice: "<<quanteslices<<std::endl;
                //}
                    
                x=0;
                y=0;
                trobat=false;
		}*/

        

	       // slice++;
	//}

        //std::cout<<"5"<<std::endl;
	//npuntstot=0;
	//c->np=quanteslices;
	
		  
	



}





/*
float LlescaContorn::GetPixelT(){
	//std::cout<<"DeterminateContour::GetPixelT-->Totals:"<<pTotals<<std::endl;
	return pTotals;
}

float LlescaContorn::GetPixelS(){
	return pSegmen;
}
  
float LlescaContorn::GetPixelSSli(int ind){
	if(ind<maxSlices)
		return pSegSli[ind];
	else
		return -1;
		
	
}*/
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
	//int k,l;
	////////////////////////
	/*for(k=0;k<=maxY;k++){
		for(l=0;l<=maxX;l++){
			std::cout<<matriu[l][k][slice];
		if(matriu[l][k][slice]!=0){
			std::cout<<"x:"<<l<<"y:"<<k<<std::endl;
			}
		}
		std::cout<<std::endl;
	}*/
	
	///////////////////////////////
	//std::cout<<"inicial:0->"<<inicial[0]<<":1->"<<inicial[1]<<std::endl;
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
			      //std::cout<<"recte->"<<direccio<<std::endl;
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
			//std::cout<<"giresquerra"<<std::endl;
				
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
			//std::cout<<"recte"<<direccio<<std::endl;
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
			//std::cout<<"girdreta"<<std::endl;
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
		//std::cout<<"actual:0->"<<actual[0]<<":1->"<<actual[1]<<":slice"<<slice<<"valor:"<<matriu[actual[0]][actual[1]][slice]<<std::endl;
		
                if(((anterior==0)&&(matriu[actual[0]][actual[1]][slice]==1))||((anterior==1)&&(matriu[actual[0]][actual[1]][slice]==1))){
			//std::cout<<"afegirm actual["<<npuntstot<<"]-x:"<<actual[0]<<"y:"<<actual[1]<<"z:"<<slice<<std::endl;
		        c->x[npuntstot]=actual[0];
			c->y[npuntstot]=actual[1];
			c->z[npuntstot]=slice;
			npuntstot++;
			npunts++;
			
		}
		anterior=matriu[actual[0]][actual[1]][slice];
		//std::cout<<"Estat:"<<anterior<<"x:"<<actual[0]<<"y:"<<actual[1]<<std::endl;
		aux++;
		if(aux==2){
			evitabucleinf[0]=x;
			evitabucleinf[1]=y-1;
		}
	
        //prova++;
       /* if(slice>=82){
        std::cout<<"SLICE: "<<slice<<std::endl; 
        //std::cout<<"Prova: "<<prova<<std::endl;
        std::cout<<"actual: "<<actual[0]<<","<<actual[1]<<std::endl;
        }*/


        //}while((((actual[0]!=inicial[0])||(actual[1]!=inicial[1]))&&((actual[0]!=evitabucleinf[0])||(actual[1]!=evitabucleinf[1]))));//&&(npuntstot<20000)
        }while((((actual[0]!=inicial[0])||(actual[1]!=inicial[1]))&&((actual[0]!=evitabucleinf[0])||(actual[1]!=evitabucleinf[1]))/*&&((actual[0]<181)||(actual[1]<181)||(slice<83))*/));//&&(npuntstot<20000)
	/*c->npslice[quanteslices][0]=npunts;
	c->npslice[quanteslices][1]=slice;
	c->npslice[quanteslices][2]=comencament;*/
        c->puntsContorn=npunts;
	//std::cout<<"slice:"<<slice<<"punts:"<<npunts<<"quanteslices:"<<quanteslices<<std::endl;
	//std::cout<<"trobatslices"<<c->npslice[quanteslices][0]<<std::endl;
	m_finish=true;
	
}

bool LlescaContorn::ItsFinish(){return m_finish;}

};