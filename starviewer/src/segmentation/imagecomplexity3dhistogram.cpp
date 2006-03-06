/*
 *  ICHistogram.cpp
 *  Image Complexity
 *
 *  Created by Jaume Rigau on Sat Nov 15 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */


//#include "ICHistogram.h"
#include "imagecomplexity3dhistogram.h"



// Implementation *************************************************************
// ****************************************************************************


namespace udg{
unsigned ImageComplexity3DHistogram::idNumber__;


// Class Methods **************************************************************
// ****************************************************************************


ImageComplexity3DHistogram::ImageComplexity3DHistogram(const unsigned bins , const double min , const double max )
{
  // Default is a binary question (i.e. Head and Tail).
  if (bins < 2) 
      std::cerr << "ImageComplexity3DHistogram::ImageComplexity3DHistogram :: bins under 2! :(" << std::endl;
  if (min >= max) 
      std::cerr << "ImageComplexity3DHistogram::ImageComplexity3DHistogram :: Incorrect interval min max" << std::endl;

  id_ = ++idNumber__;
  bins_ = bins;
  minimum_ = min;
  maximum_ = max;
  binSize_ = (max - min) / bins;
  frequency_ = new unsigned[bins];

  reset();
}


ImageComplexity3DHistogram::ImageComplexity3DHistogram(const ImageComplexity3DHistogram &h)
{
  id_ = ++idNumber__;
  frequency_ = NULL; // Necessary for control delete copy!
  copy(h);
}


ImageComplexity3DHistogram::ImageComplexity3DHistogram(const ImageComplexity3DHistogram &h,
                     const unsigned from,
				 const unsigned to)
{
  id_ = ++idNumber__;
  frequency_ = NULL; // Necessary for control delete copy!
  copy(h);
  filter(from, to);
}


ImageComplexity3DHistogram::~ImageComplexity3DHistogram()
{
  delete [] frequency_; 
  // messageUser(NEW, "Destroyed #" + intToStr(id_), "ImageComplexity3DHistogram::~ImageComplexity3DHistogram"); Test OK.
}


// Instance Methods ***********************************************************
// ****************************************************************************


// Operators ******************************************************************


ImageComplexity3DHistogram& ImageComplexity3DHistogram::operator= (const ImageComplexity3DHistogram& h)
{
  return copy(h);
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::operator+= (const ImageComplexity3DHistogram& h)
{
  // For flexibility, no preconditions of intervals and bin numbers.
  // The operation is between the minimum intersection of bins.
  unsigned n = (bins_ < h.bins_) ? bins_ : h.bins_;
  for (unsigned bin = 0; 
       bin < n; 
       bin++)
	       frequency_[bin] += h.frequency_[bin];
	  
  return *this;
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::operator-= (const ImageComplexity3DHistogram& h)
{
  // For flexibility, no preconditions of intervals and bin numbers.
  // The operation is between the minimum intersection of bins.
  // Is the natural substraction.
  unsigned n = (bins_ < h.bins_) ? bins_ : h.bins_;
  unsigned f;
  for (unsigned bin = 0; 
       bin < n; 
       bin++) {
	       f = h.frequency_[bin];
		  if (f >= frequency_[bin]) 
		        frequency_[bin] = 0;
		   else frequency_[bin] -= f; }
	  
  return *this;
}


/* Binary operators are not ofered because there are non-commutative and 
   the erroneous interpretation well be easy. Example:
ImageComplexity3DHistogram operator+ (const ImageComplexity3DHistogram& h1,
				 const ImageComplexity3DHistogram& h2)
{
  ImageComplexity3DHistogram h = h1;
  return h += h2;
} */


// Put Data *******************************************************************


ImageComplexity3DHistogram& ImageComplexity3DHistogram::in(const double real)
{   
  frequency_[where(real)]++;
  
  return *this;
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::out(const double real)
{   
  unsigned bin = where(real);  
  if (frequency_[bin]) 
       frequency_[bin]--;
  else //messageUser(WARNING, "No items to out", "ImageComplexity3DHistogram::out"); 
      std::cerr << "WARNING: ImageComplexity3DHistogram::out:No items to out" << std::endl; 
  return *this;
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::reset()
{   
  for (unsigned bin = 0; 
       bin < bins_; 
       bin++)
            frequency_[bin] = 0;
       
  return *this;
}
 

ImageComplexity3DHistogram& ImageComplexity3DHistogram::filter(const unsigned from,
					    const unsigned to)
{   
  if (!(1 <= from && from <= bins_)) //messageUser(PRECONDITION, "From out range", "ImageComplexity3DHistogram::filter");
      std::cerr << "ImageComplexity3DHistogram::filter :: 'from' out of range " << std::endl;
  if (!(1 <= to && to <= bins_)) //messageUser(PRECONDITION, "To out range", "ImageComplexity3DHistogram::filter");
      std::cerr << "ImageComplexity3DHistogram::filter :: 'to' out of range " << std::endl;
  if (!(from <= to)) //messageUser(PRECONDITION, "Interval null", "ImageComplexity3DHistogram::filter");
      std::cerr << "ImageComplexity3DHistogram::filter :: Interval is null " << std::endl;

  for (unsigned bin = 0; 
       bin < from - 1; 
       frequency_[bin++] = 0);

  for (unsigned bin = to; 
       bin < bins_; 
       frequency_[bin++] = 0);
       
  return *this;
}



// Information ****************************************************************


unsigned ImageComplexity3DHistogram::minimum()
{   
  unsigned min = frequency_[0];
  for (unsigned bin = 1; 
       bin < bins_; 
       bin++)
            if (frequency_[bin] < min) min = frequency_[bin];
       
  return min;
}


unsigned ImageComplexity3DHistogram::maximum()
{   
  unsigned max = frequency_[0];
  for (unsigned bin = 1; 
       bin < bins_; 
       bin++)
            if (frequency_[bin] > max) max = frequency_[bin];
       
  return max;
}


unsigned ImageComplexity3DHistogram::total()
{   
  unsigned n = frequency_[0];
  for (unsigned bin = 1; 
       bin < bins_; 
       n += frequency_[bin++]);
  
  return n;
}


unsigned ImageComplexity3DHistogram::emptyBins()
{
  unsigned n = 0;
  for (unsigned bin = 0; 
       bin < bins_; 
       bin++)
            if (frequency_[bin] == 0) n++;
       
  return n;  
}


unsigned ImageComplexity3DHistogram::fullBins()
{
  return bins_ - emptyBins();  
}


double ImageComplexity3DHistogram::mean()
{   
  double tot = total();

  if( tot == 0 )
      return 0;
  else
      return sum() / tot;
}


double ImageComplexity3DHistogram::entropy()
{   
  // Use (-1/N)Sum(N_i Log(N_i)) + Log(N).
  // Precis zero better.
  if (fullBins() < 2) return 0;  

  double entropy = 0.0;
// això és copiat d'exemple itk
    double n = 0.0;
    for (unsigned bin = 0; bin < bins_; bin++) 
    {
        n += frequency_[bin];
    }
    for (unsigned bin = 0; bin < bins_; bin++) 
    {    
        const double probability = frequency_[bin] / n;
          
        if( probability > 1e-16 )
        {
            entropy += - probability * udg::MathTools::logTwo( probability ); //( log( probability ) / log( 2.0 ) );
        }
        
    }
    return entropy;
/*
  unsigned n = 0;
  double sum = 0;
  for (unsigned bin = 0; bin < bins_; bin++) 
  {
    unsigned f = frequency_[bin];
    //sum += f * udg::logTwo(f); 
    sum += f * ( log(f) / log(2.0) ); 
    n += f; 
  }
  
  //return (- sum / n) + udg::logTwo(n);
  return (- sum / n) + ( log(n)/log(2.0) );
*/
}



// Private ********************************************************************
//*****************************************************************************


// Methods ********************************************************************


ImageComplexity3DHistogram& ImageComplexity3DHistogram::copy(const ImageComplexity3DHistogram& h)
{
  bins_ = h.bins_;
  minimum_ = h.minimum_;
  maximum_ = h.maximum_;
  binSize_ = h.binSize_;
  
  if (frequency_ ) delete [] frequency_; // Control constructor. 
  frequency_ = new unsigned[bins_];
  for (unsigned bin = 0; 
       bin < bins_; 
       bin++)
	       frequency_[bin] = h.frequency_[bin];
	  
  return *this;
}


unsigned ImageComplexity3DHistogram::where(const double real)
{
  if (!(minimum_ <= real && real <= maximum_)) //messageUser(PRECONDITION, "Data out of interval", "ImageComplexity3DHistogram::where");
      std::cerr << "ImageComplexity3DHistogram::where :: Data [" << real <<"] out of interval [" << minimum_ << "-" << maximum_ << "]" << std::endl;
   
  unsigned bin = (real == maximum_) ?
                 bins_ :
                 static_cast<unsigned>(floor((real - minimum_) / binSize_)) + 1;
		 
  return bin - 1; // C index.
}


double ImageComplexity3DHistogram::value(const unsigned bin)
{
  if (bin >= bins_) //messageUser(PRECONDITION, "Bin out range", "ImageComplexity3DHistogram::value");
      std::cerr << "ImageComplexity3DHistogram::value :: 'bin' out of range " << std::endl;
  return minimum_ + binSize_ * (bin + .5);
}


double ImageComplexity3DHistogram::sum()
{   
  double sum = 0;
  for (unsigned bin = 0; 
       bin < bins_; 
       bin++)
            sum += frequency_[bin] * value(bin);
       
  return sum;
}

}


// Display ********************************************************************

/*
ImageComplexity3DHistogram& ImageComplexity3DHistogram::display()
{

  FILE* out = getOut();
  
  fprintf(out, "[ImageComplexity3DHistogram DATA #%u:\n", id_);
  fprintf(out, "Bins:              %u\n", bins_);
  fprintf(out, "Minimum:           %.6f\n", minimum_);
  fprintf(out, "Maximum:           %.6f\n", maximum_);
  fprintf(out, "Bin size:          %.6f\n", binSize_);
  fprintf(out, "Minimum frequency: %u\n", minimum());
  fprintf(out, "Maximum frequency: %u\n", maximum());
  fprintf(out, "Total frequency:   %u\n", total());
  fprintf(out, "Mean:              %.6f\n", mean());
  fprintf(out, "Entropy:           %.6f\n", entropy());
  fprintf(out, "Entropy Normal:    %.6f\n]\n", entropyNormal());

  return *this;
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::displayColumns(const string& title,
                                     const double scale)
{

  if (scale <= 0) messageUser(PRECONDITION, "Incorrect scale", "ImageComplexity3DHistogram::displayColumns");
  
  FILE* out = getOut();
  fprintf(out, "[ImageComplexity3DHistogram PLOT: #%u\n", id_);
  for (unsigned height = roundReal(maximum() * scale); 
       height > 0; 
       height--) {
            fprintf(out, "|");
            for (unsigned bin = 0; 
	         bin < bins_; 
		 bin++) 
                      fprintf(out, "%c", (static_cast<unsigned>(roundReal(frequency_[bin] * scale)) >= height) ? '*' : ' ');
       fprintf(out, "|\n"); }

  string line(bins_, '-');
  fprintf(out, " %s\n", line.c_str());      
  fprintf(out, " %s\n", title.c_str());
  display();
  fprintf(out, "]\n");
   
  return *this;
}


ImageComplexity3DHistogram& ImageComplexity3DHistogram::displayRows(const double scale = 1)
{

  if (scale <= 0) messageUser(PRECONDITION, "Incorrect scale", "ImageComplexity3DHistogram::displayRows");
  
  FILE* out = getOut();  
  fprintf(out, "[ImageComplexity3DHistogram PLOT: #%u\n", id_);  
  unsigned n = total();
  unsigned maxHeight = roundReal(maximum() * scale);
  for (unsigned bin = 0; 
      bin < bins_; 
      bin++) {
           unsigned f = frequency_[bin];
           fprintf(out, "%7.3f ", value(bin));   
           unsigned max = roundReal(f * scale);
	   string line(max, '*'),
	          space(maxHeight - max, ' ');
	   line += space;
	   fprintf(out, "%s", line.c_str());
           fprintf(out, " %6u (%6.2f%%)\n", f, divReal(f * 100, n)); }
  display();
  fprintf(out, "]\n");
 
  return *this;
}
*/

