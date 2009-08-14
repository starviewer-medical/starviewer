
#ifndef UDGQIMAGEPRINTEXTENSION_H
#define UDGQIMAGEPRINTEXTENSION_H

#include "ui_qimageprintextensionbase.h"


namespace udg {

class ImagePrintFactory;
class QPrintJobCreatorWidget;

class QImagePrintExtension : public QWidget , private::Ui::QImagePrintExtensionBase {
Q_OBJECT

public:
    QImagePrintExtension( QWidget *parent = 0 );
    ~QImagePrintExtension();

private:
	void createConnections();
	void createActions();

private: 			
	ImagePrintFactory		* m_factory;
	
};
} // end namespace udg.

#endif
