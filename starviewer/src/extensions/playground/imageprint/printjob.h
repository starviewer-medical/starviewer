#ifndef UDGPRINTJOB_H
#define UDGPRINTJOB_H

/**
* Interfície pels PrintJobs.
*/

namespace udg{

class PrintJob
{
public:
    
    ///Especifica/retorna el número de pàgines que s'han d'implementar del JOB
    void setNumberOfCopies(int numberOfCopies);
    int getNumberOfCopies();

private:

    int m_numberOfCopies;
};
}; 
#endif