
#ifndef UDGUTILS_H
#define UDGUTILS_H

#include <QString>

namespace udg {

/**
    Classe que conté utilitats vàries utilitzades per la classe inputoutput
  */
class Utils {

public:
    static QString generateUID(const QString &prefix = QString());
};

};  // end namespace udg

#endif
