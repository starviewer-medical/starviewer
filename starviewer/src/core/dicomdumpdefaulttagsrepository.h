#ifndef UDGDICOMDUMPDEFAULTTAGSREPOSITORY_H
#define UDGDICOMDUMPDEFAULTTAGSREPOSITORY_H

#include "repository.h"
#include "dicomdumpdefaulttags.h"

namespace udg {

/**
   Classe que implementa un Repositori de DICOMDumpDefaultTags.
   Aquesta classe és una implementació de la classe Repository
*/

class DICOMDumpDefaultTagsRepository : public Repository<DICOMDumpDefaultTags>
{

public:

    /// Ens retorna l'única instància del repositori (Patró Singleton)
    static DICOMDumpDefaultTagsRepository* getRepository()
    {
        static DICOMDumpDefaultTagsRepository dicomRepository;
        return &dicomRepository;
    }

    ~DICOMDumpDefaultTagsRepository();

private:

    // Constructor private pq només es puguin definir objectes a través del patro Singleton
    DICOMDumpDefaultTagsRepository();
};

}

#endif
