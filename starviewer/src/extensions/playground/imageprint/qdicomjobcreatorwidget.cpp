#include <iostream>
#include "qdicomjobcreatorwidget.h"
#include "dicomprintjob.h"

namespace udg {

QDicomJobCreatorWidget::QDicomJobCreatorWidget()
{
    setupUi( this );
	createActions();	
	createConnections();
    m_printJob=new DicomPrintJob();
}

QDicomJobCreatorWidget::~QDicomJobCreatorWidget()
{

}

void QDicomJobCreatorWidget::createConnections()
{ 
	
}

void QDicomJobCreatorWidget::createActions()
{
	
}

PrintJob* QDicomJobCreatorWidget::getPrintJob()
{
    return m_printJob;
}
}                                      