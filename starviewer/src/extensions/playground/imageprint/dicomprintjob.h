#ifndef UDGDICOMPRINTJOB_H
#define UDGDICOMPRINTJOB_H

#include <QString>
#include "printjob.h"

/**
* Implementació per la Interfície PrintJob per impressores Dicom en DCMTK.
*/

namespace udg
{   

class DicomPrintJob: public PrintJob
{
public:
	 DicomPrintJob();
	~DicomPrintJob();
    
    void setNumberCopies(const int &_numberCopies);
    void setPriority(const QString &_priority);
    void setMediumType(const int &_mediumType);
    void setLabel(const QString &_label);
    void setFilmDestination(const QString &_filmDestination);

    int getNumberCopies() const;
    QString getPriority() const;
    int getMediumType() const;
    QString getLabel() const;
    QString getFilmDestination() const;

private:

    int         m_numberCopies;
    QString     m_priority;
    int         m_mediumType;
    QString     m_label;
    QString     m_filmDestination;
};
}; 
#endif