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
    void setPrintPriority(const QString &_printPriority);
    void setMediumType(const QString &_mediumType);
    void setLabel(const QString &_label);
    void setFilmDestination(const QString &_filmDestination);

    int getNumberCopies() const;
    QString getPrintPriority() const;
    QString getMediumType() const;
    QString getLabel() const;
    QString getFilmDestination() const;

private:

    int         m_numberCopies;
    QString     m_printPriority;
    QString     m_mediumType;
    QString     m_label;
    QString     m_filmDestination;
};
}; 
#endif