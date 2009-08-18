#ifndef UDGPRINTJOB_H
#define UDGPRINTJOB_H

/**
* Interfície pels PrintJobs.
*/

namespace udg{

class PrintJob
{
 public:
    
    virtual void setNumberCopies(const int &_numberCopies)=0;
    virtual int getNumberCopies() const =0;
};
}; 
#endif