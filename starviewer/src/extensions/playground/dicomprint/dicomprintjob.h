#ifndef UDGDICOMPRINTJOB_H
#define UDGDICOMPRINTJOB_H

#include <QString>
#include <QList>

#include "printjob.h"
#include "dicomprintpage.h"

/**
* Implementació de la interfície PrintJob per impressores Dicom en DCMTK.
* Un PrintJob serà un objecte que contindrà la informació necessària per fer la impressió (Prioritat, Pàgines a imprimir...) 
*/

namespace udg
{   
class DicomPrintJob: public PrintJob
{
public:
    
    DicomPrintJob();

    ~DicomPrintJob();

    /// Afegim la Prioritat que tindrà la impressió (High, Medium, Low)
    void setPrintPriority( const QString& printPriority );

    /// Retorna la Prioritat que tindrà la impressió (High, Medium, Low)
    QString getPrintPriority() const;

    /// Afegim el Mitjà en el qual es realitzarà la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    void setMediumType( const QString& mediumType );

    /// Retorna el Mitjà en el qual es realitzarà la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    QString getMediumType() const;

    /// Afegim informació addicional a la impressió.
    void setLabel( const QString& label );

    /// Retorna la informació addicional de la impressió.
    QString getLabel() const;

    /// Afegim el destí de la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    void setFilmDestination( const QString& filmDestination );

    /// Retorna el destí de la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    QString getFilmDestination() const; 

    ///Especifica les PrintPage del PrintJob
    void setDicomPrintPages( QList<DicomPrintPage> dicomPrintPages );

    ///Retorna les PrintPage del PrintJob
    QList<DicomPrintPage> getDicomPrintPages();

private:

    QString m_printPriority;
    
    QString m_mediumType;
    
    QString m_label;
    
    QString m_filmDestination;

    QList<DicomPrintPage> m_listDicomPrintPage;
};
}; 
#endif