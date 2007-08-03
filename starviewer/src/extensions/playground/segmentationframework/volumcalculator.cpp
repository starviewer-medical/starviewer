
#include "volumcalculator.h"
namespace udg {


///Constructor. Calcula (VolumTotal,VolumSegmentat,AreaLlesques..)
VolumCalculator::VolumCalculator(ImageType* rea, int proj){
	
	inputImage = rea;
	
	//atencio amb la direcció
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
	float x=0,y=0;
	int slice=0;
	while( !inputIt.IsAtEnd() )
	{
		while ( !inputIt.IsAtEndOfSlice() )
		{
			while ( !inputIt.IsAtEndOfLine() )
			{
			total++;
			if(inputIt.Get()==255){
				//std::cout<<"pixel["<<x<<","<<y<<","<<slice<<"]:"<<inputIt.Get()<<":"<<total<<std::endl;
				pixelseg++;
				pixelsegsli++;
				}
			++inputIt;
			y++;
			}
		y=0;
		inputIt.NextLine();
		x++;
		}
		//std::cout<<"VolumCalculator::VolumCalculator-->llesca"<<slice<<std::endl;
	x=0;
	pSegSli[slice] = pixelsegsli;
	pixelsegsli = 0;
	inputIt.NextSlice();
	slice++;
	
	}
	pTotals = total;
	pSegmen = pixelseg;
	maxSlices = slice;
	//std::cout<<"ENtraconstructor"<<maxSlices<<std::endl;
	
}

///Destructor
VolumCalculator::~ VolumCalculator(){


}
///Retorna el volum total del model
float VolumCalculator::GetPixelT(){
	//std::cout<<"VolumCalculator::GetPixelT-->Totals:"<<pTotals<<std::endl;
	return pTotals;
}
///Retorna el volum segmentat del model
float VolumCalculator::GetPixelS(){
	return pSegmen;
}
///Retorna l'àrea de la llesca (ind).
float VolumCalculator::GetPixelSSli(int ind){
	if(ind<maxSlices)
		return pSegSli[ind];
	else
		return -1;
		
	
}
};