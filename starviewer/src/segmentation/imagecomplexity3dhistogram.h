/*
 *  ICHistogram.h
 *  Image Complexity
 *
 *  Created by Jaume Rigau on Sat Nov 15 2003.
 *  Copyright (c) 2003. All rights reserved.
 *  
 *  Modifications by Jesús Spínola on Summer 2005
 */


#ifndef FLAG_ImageComplexity3DHistogram_H
#define FLAG_ImageComplexity3DHistogram_H



#include <string> // c++ strings
#include <iostream> // couts

#include "mathtools.h"
// Design *********************************************************************
// ****************************************************************************

/*
- Simple histogram for real values.
- With #id for control.
- Buffer is a vector for fast acces.
- Minimum 2 bins.
- Intervals are [a,b[ for bin, except for the last, where b is close.
- Las test: OK.
*/

// Interface ******************************************************************
// ****************************************************************************


namespace udg{

class ImageComplexity3DHistogram {

public:


// Class Methods **************************************************************
// ****************************************************************************


ImageComplexity3DHistogram(const unsigned bins_ = 2,
          const double min_ = 0,
          const double max_ = 1);
ImageComplexity3DHistogram(const ImageComplexity3DHistogram &h);
ImageComplexity3DHistogram(const ImageComplexity3DHistogram &h,
		const unsigned from,
		const unsigned to);
~ImageComplexity3DHistogram();



// Instance Methods ***********************************************************
// ****************************************************************************


// Operators ******************************************************************


ImageComplexity3DHistogram& operator= (const ImageComplexity3DHistogram& h);
ImageComplexity3DHistogram& operator+= (const ImageComplexity3DHistogram& h);
ImageComplexity3DHistogram& operator-= (const ImageComplexity3DHistogram& h);
/* Binary operators are not ofered because there are non-commutative and 
   the erroneous interpretation well be easy. */


// Get Data *******************************************************************


inline unsigned get(const unsigned bin)
{
  if (!(1 <= bin && bin <= bins_)) 
      std::cerr << "ImageComplexity3DHistogram::get :: bin: " << bin << "Out of range [1.." << bins_ << "]" << std::endl;
  
  return frequency_[bin - 1];
}


// Put Data *******************************************************************


inline ImageComplexity3DHistogram& put(const ImageComplexity3DHistogram& h)
{
  return *this = h;
}


inline ImageComplexity3DHistogram& put(const unsigned n,
                      const unsigned bin)
{
  if (!(1 <= bin && bin <= bins_)) 
      std::cerr << "ImageComplexity3DHistogram::put :: Out of range" << std::endl;
  frequency_[bin - 1] = n;
  
  return *this;
}


ImageComplexity3DHistogram& in(const double real);
ImageComplexity3DHistogram& out(const double real);
ImageComplexity3DHistogram& reset();
ImageComplexity3DHistogram& filter(const unsigned from,
			   const unsigned to);


// Information ****************************************************************


inline unsigned getId()
{
  return id_;
}


inline unsigned getBins()
{
  return bins_;
}


inline double getMinimum()
{
  return minimum_;
}


inline double getMaximum()
{
  return maximum_;
}


inline double getBinSize()
{
  return binSize_;
}


inline double getMinimumBin(const unsigned bin)
{
  // Included in interval.
  if (!(1 <= bin && bin <= bins_)) 
      std::cerr << "ImageComplexity3DHistogram::getMinimumBin :: Out of range" << std::endl; 
  
  return minimum_ + (bin - 1) * binSize_;
}


inline double getMaximumBin(const unsigned bin)
{
  // Excluded in interval except the last.
  if (!(1 <= bin && bin <= bins_)) 
      std::cerr << "ImageComplexity3DHistogram::getMaximumBin :: Out of range" << std::endl; 
  return minimum_ + bin * binSize_;
}


inline double entropyNormal()
{
  // No equal to coefficient UH.
  double entropyH = entropy();
  if (entropyH == 0) // One o zero bins.
       return 1;
  else return entropyH / udg::MathTools::logTwo( (double)bins_);
  
}


inline double coefficientUH()
{
  // No equal to entropyNormal.
  double entropyH = entropy();
  if (entropyH == 0) // One o zero bins.
       return 1;
  else return 1 - entropyH /  udg::MathTools::logTwo( fullBins() );
}


unsigned minimum();
unsigned maximum();
unsigned total();
unsigned emptyBins();
unsigned fullBins();
double mean();
double entropy();


// Display ********************************************************************

void print()
{
    unsigned int i;
    for( i = 0; i < bins_; i++ )
        std::cout << "Frequencia bin[" << i << "] :: " << frequency_[i] << std::endl;
}
/*
ImageComplexity3DHistogram& display();
ImageComplexity3DHistogram& displayColumns(const std::string& title,
                          const double scale = 1);
ImageComplexity3DHistogram& displayRows(const double scale = 1);
*/

// Private ********************************************************************
//*****************************************************************************


private:


// Data ***********************************************************************
    

static unsigned idNumber__;

unsigned id_,
         bins_;
double minimum_,
	maximum_,
	binSize_;
unsigned* frequency_;
     
     
// Methods ********************************************************************

ImageComplexity3DHistogram& copy(const ImageComplexity3DHistogram& h);
unsigned where(const double real);
double value(const unsigned bin);
double sum();


};

}

#endif
    

