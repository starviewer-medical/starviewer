/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSINGLETON_H
#define UDGSINGLETON_H

#include <QReadWriteLock>
#include <QThread>
#include <QWriteLocker>

namespace udg {

/**
    Classe que implementa el pattern singleton. La implementació no és thread-safe, per tant, no s'hauria d'utilitzar des de diferents
    threads. Qualsevol classe que es vulgui convertir a Singleton haurà de tenir un constructor sense paràmetres.
    La manera d'utilitzar-la seria la següent:
    \code
    typedef Singleton<TestingClass> TestingClassSingleton;
    ...
    TestingClass* instance = TestingClassSingleton::instance();
    instance->metode();
    ...
    \endcode
    Com es pot veure, és molt recomenable utilizar un typedef per tal de que no disminueixi la llegibilitat del codi. Aquesta manera
    d'utilitzar el singleton és perillosa si es vol que no hi pugui haver cap altra instància de la classe TestingClass. D'aquesta manera
    el que s'està fent és garantir que del tipus TestingClassSingleton tindrem una instància globalment accessible. Però res ens impedeix
    crear classes del tipus TestingClass a part.
    Una altra manera de declarar una classe com a singleton seria la següent:
    \code
    class OnlyOne : public Singleton<OnlyOne>
    {
        //..resta del codi
    };
    \endcode
    D'aquesta forma sí que estem assegurant que de la classe OnlyOne, en tota la vida del programa, només n'hi haurà una i serà la
    mateixa per tota l'execució.
    \todo Fer-la thread-safe.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <typename T>
class Singleton{

public:

    /// Ens serveix per accedir a l'única instància de la classe T
    static T* instance()
    {
        static T theSingleInstance;
        return &theSingleInstance;
    }

protected:
    Singleton(){}; // No s'implementa
    Singleton(const Singleton&){}; // No s'implementa
    Singleton &operator=(const Singleton&){}; // No s'implementa
};

/**
    Classe que implementa el patró Singleton però a nivell de punter. Es fa servir de la mateixa manera que Singleton però amb 
    la particularitat de que el singleton es fa a nivell de punter en comptes d'objecte.
    Aquest singleton només s'hauria de fer servir quan hi ha problemes que impedeixen fer servir l'anterior. Alguns d'aquests problemes
    serien problemes en l'ordre de destrucció d'objectes estàtics (com el cas de la DcmDatasetCache per culpa de dcmtk).
    El problema que té aquesta implementació és que el destructor no es cridarà mai.
    Aquesta implementació sí que és thread-safe.
*/
template<typename T>
class SingletonPointer{

public:
    static T* instance()
    {
        if (m_theSinglePointer == NULL)
        {
            QWriteLocker locker(&m_pointerLock);
            if (m_theSinglePointer == NULL)   //Fem double checking per evitar bloquejos innecessaris
            {
                m_theSinglePointer = new T();
            }
        }
        return m_theSinglePointer;
    }

protected:
    SingletonPointer(){}; // No s'implementa
    SingletonPointer(const SingletonPointer&){}; // No s'implementa
    SingletonPointer &operator=(const SingletonPointer&){}; // No s'implementa

private:
    static QReadWriteLock m_pointerLock;
    static T* m_theSinglePointer;
};

template<typename T> T* SingletonPointer<T>::m_theSinglePointer = NULL;
template<typename T> QReadWriteLock SingletonPointer<T>::m_pointerLock;
}

#endif
