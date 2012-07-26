#ifndef UDGCUSTOMWINDOWLEVELSREPOSITORY_H
#define UDGCUSTOMWINDOWLEVELSREPOSITORY_H

#include "repository.h"
#include "windowlevel.h"

namespace udg {

/**
    Classe que implementa un Repositori de CustomWindowLevels.
    Aquesta classe és una implementació de la classe Repository
  */
class CustomWindowLevelsRepository : public Repository<CustomWindowLevel> {

public:
    /// Ens retorna l'única instància del repositori (Patró Singleton)
    static CustomWindowLevelsRepository* getRepository()
    {
        static CustomWindowLevelsRepository customWindowLevelRepository;
        return &customWindowLevelRepository;
    }

    ~CustomWindowLevelsRepository();

private:
    // Constructor private pq només es puguin definir objectes a través del patro Singleton
    CustomWindowLevelsRepository();
};

}

#endif
