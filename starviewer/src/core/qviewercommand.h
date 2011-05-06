#ifndef UDGQVIEWERCOMMAND_H
#define UDGQVIEWERCOMMAND_H

#include <QObject>

namespace udg {

/**
  Classe que segueix el patró Command i és la classe base de tots els Command dels QViewer.
  Serveix per encapsular comandes que s'han de fer sobre un viewer.

  Per implementar un command simplement cal heretar d'aquesta classe i implementar el mètode execute().
  Per evitar-ne un mal ús caldrà que les classes que heretin d'aquesta implementin un constructor amb tots
  els paràmetres necessaris per tal de cridar el mètode execute()
  */

class QViewerCommand : public QObject {
    Q_OBJECT
public:
    virtual ~QViewerCommand();

public slots:
    virtual void execute() = 0;

protected:
    /// Fem que el constructor no es pugui cridar si no és heretant per obligar a tenir constructors
    /// que obliguin a passar els paràmetres necessàries pel execute().
    QViewerCommand(QObject *parent = 0);
};

} // End namespace udg

#endif // UDGQVIEWERCOMMAND_H
