
#include "determinatecontour.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

namespace udg {

DeterminateContour::DeterminateContour(ImageType* rea, int proj){


	inputImage = rea;

	RegionType reg = inputImage->GetLargestPossibleRegion();
	SizeType   siz   = reg.GetSize();


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


	index[ direction[0] ]    = requestedRegion.GetIndex()[ direction[0] ];
	index[ 1- direction[0] ] = requestedRegion.GetIndex()[ direction[1] ];
	size[ direction[0] ]     = requestedRegion.GetSize()[  direction[0] ];
	size[ 1- direction[0] ]  = requestedRegion.GetSize()[  direction[1] ];

	region.SetSize( size );
	region.SetIndex( index );

	SliceIteratorType  inputIt(  inputImage,  inputImage->GetRequestedRegion() );
	inputIt.SetFirstDirection(  direction[1] );
	inputIt.SetSecondDirection( direction[0] );

	inputIt.GoToBegin();

	float total=0,pixelseg=0,pixelsegsli=0;
	int x=0,y=0;
	int slice=0;
        maxX=0;
        maxY=0;

	while( !inputIt.IsAtEnd() )
	{


		while ( !inputIt.IsAtEndOfSlice() )
		{


			while ( !inputIt.IsAtEndOfLine() )
			{

			total++;

			if(inputIt.Get()==255){//255

				matriu[x][y][slice]=1;
				pixelseg++;
				pixelsegsli++;
				}
			else{matriu[x][y][slice]=0;
			}

			++inputIt;
			y++;
			}
		maxY=y;
		y=0;
		inputIt.NextLine();
		x++;
		}

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

	slice=0;
	y=0;
	x=0;
	bool trobat=false;
	quanteslices=0;


	while(slice<maxSlices){
		while((y<=maxY)&&(!trobat)){

			while((x<=maxX)&&(!trobat)){

				if(matriu[x][y][slice]==1){
					trobat=true;
				}
			         x++;
			}

		if(!trobat){
			x=0;
		}

		y++;
	        }
	        if(!trobat)y=0;
	        else{
                    ferreseguit(x-1,y-1,slice);
                    quanteslices++;

                x=0;
                y=0;
                trobat=false;
		}
	        slice++;
	}


	npuntstot=0;
	c->np=quanteslices;


}
DeterminateContour::~ DeterminateContour(){
}
float DeterminateContour::GetPixelT(){

	return pTotals;
}

float DeterminateContour::GetPixelS(){
	return pSegmen;
}

float DeterminateContour::GetPixelSSli(int ind){
	if(ind<maxSlices)
		return pSegSli[ind];
	else
		return -1;


}
void DeterminateContour::ferreseguit(int x, int y, int slice){

	int direccio=2;
	int movant[2]={-1,0};
	int inicial[2]={x,y};
	int actual[2]={x,y};
	int evitabucleinf[2]={x,y};
	int anterior=1;
	int aux=0,npunts=0;
	int comencament=npuntstot;
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

        prova++;




        }while((((actual[0]!=inicial[0])||(actual[1]!=inicial[1]))&&((actual[0]!=evitabucleinf[0])||(actual[1]!=evitabucleinf[1]))
        ));
	c->npslice[quanteslices][0]=npunts;
	c->npslice[quanteslices][1]=slice;
	c->npslice[quanteslices][2]=comencament;


}


}
