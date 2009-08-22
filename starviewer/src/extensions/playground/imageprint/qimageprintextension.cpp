#include <iostream>
#include <QWidget>

#include "qimageprintextension.h"
#include "imageprintfactory.h"
#include "dicomprintfactory.h"
#include "qprintjobcreatorwidget.h"
#include "qdicomjobcreatorwidget.h"
#include "qprinterconfigurationwidget.h"
#include "qprintingconfigurationwidget.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

namespace udg {

QImagePrintExtension::QImagePrintExtension( QWidget *parent )
{	
    setupUi( this );
	createActions();	
	createConnections();

	// Només per provar.
	m_factory=new DicomPrintFactory();
	//m_tbToolBox->addItem(m_factory->getPrintingConfigurationWidget(),"PrintingConfigurationWidget");
	//m_tbToolBox->addItem(m_factory->getPrintJobCreatorWidget(),"JobCreatorWidget");
}

QImagePrintExtension::~QImagePrintExtension()
{
}

void QImagePrintExtension::createConnections()
{ 
    connect( m_qpbConfiguracio, SIGNAL( clicked() ), SLOT( configurationPrinter() ) );
}

void QImagePrintExtension::createActions()
{	
}

void QImagePrintExtension::configurationPrinter()
{
    QPrinterConfigurationWidget * printerConfigurationWidgetProof=m_factory->getPrinterConfigurationWidget();
    printerConfigurationWidgetProof->show();
}
}                                      