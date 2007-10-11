
#ifndef LLENCA_H
#define LLENCA_H
#include <qobject.h>
#include <qwidget.h>
#include <iostream.h>
namespace udg {

class llenca : public QObject{
	Q_OBJECT
public:
	llenca(){};
	~llenca(){};
	void fesllenca(double aux){
                
		emit e(aux);
		
	}
signals:
	void e(double i);
};
}
#endif
