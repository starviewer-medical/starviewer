/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGGENERICFACTORY_H
#define UDGGENERICFACTORY_H

#include <map>
#include <QObject>
#include <QList>

namespace udg {

/**
    Classe que serveix com a base per implementar el pattern Factory.
    L'ús d'aquesta classe és intern a la plataforma i no s'hauria d'utilitzar excepte si s'està desenvolupant per el core.
    La seva utilitat, juntament amb GenericFactoryRegister, és la de proveïr una implementació genèrica del pattern Factory.

    Aquesta classe ens és útil a l'hora de generar classes d'una mateixa jerarquia de classes. Per exemple, si tinguessim
    la classe Vehicle com a classe abstracta i les classes Camio, Cotxe, Motocicleta com a classes que hereden de Vehicle,
    podríem utilitzar la GenericFactory per instanciar objectes del tipus Camio, Cotxe o Motocicleta però que fossin retornats
    com a Vehicles.

    L'implementació actual pressuposa que tots els objectes que es voldran crear heredaran de QObject i, per tant, el seu constructor
    té un paràmetre que és el parent d'aquest.

    Exemple d'utilització:
    @code
     //Creem una Factory de Vehicles que seran identificats per una string. Vehicles i els seus fills són subclasses de QObject
     typedef GenericFactory<Vehicle, std::string> VehicleFactory;

     VehicleFactory vehicles;
     
     //.. Aquí hauríem de registrar les diferents classes amb el Factory.
     //   Veure GenericFactoryRegister per tenir una manera de fer-ho senzillament
     
     Vehicle* vehicle = vehicles->create("cotxe");
     Vehicle* vehicle2 = vehicles->create("motocicleta");

     std::cout<< "Total de rodes = " << vehicle->getNumeroRodes() + vehicle2->getNumeroRodes() << std::endl;

     //.. Això imprimiria "Total de rodes = 6" suposant que cotxe retornes 4 i motocicleta 2.
    @endcode
    Tot i que en l'exemple no es faci, caldria mirar si l'objecte retornat és NULL o no ho és.
    
    Aquesta classe s'utilitzarà, la majoria de les vegades, amb un singleton per facilitar-ne el registre i l'accés però no té perquè.
    
    @TODO Si s'utilitza aquesta classe conjuntament amb un singleton només es podrà tenir un objecte de cada tipus.
    @TODO En cas que fos necessari s'hauria de fer l'implementació més genèrica per permetre de 0 a n paràmetres en el constructor i no
          no obligar a que els objectes creat heretin de QObject.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
    @see GenericSingletonFactoryRegister
    @see ExtensionFactory
*/

template <class BaseClass, typename ClassIdentifier, class ParentType = QObject>
class GenericFactory
{
    typedef BaseClass* (*BaseClassCreateFunction)(ParentType*);
    typedef std::map<ClassIdentifier, BaseClassCreateFunction> FunctionRegistry;

public:
    ///Constructor de la classe
    GenericFactory(){}

    /**
     * Mètode que serveix per registrar una funció de creació d'una classe.
     * Aquest mètode és el que s'ha de fer servir per tal de poder registrar una determinada classe en el factory.
     * @param className Nom de la classe que es vol registrar.
     * @param function Funció del tipus BaseClassCreateFunction que ens retorna un objecte de la classe className.
     *                 Aquest mètode el dona, automàticament, la classe GenericFactoryRegister.
     */
    void registerCreateFunction(const ClassIdentifier &className, BaseClassCreateFunction function)
    {
        m_registry[className] = function;
    }

    /**
     * Mètode que ens crea l'objecte que vingui definit per l'identificador. El retorna del tipus BaseClass.
     * @param className Nom de la classe que volem que faci l'objecte
     * @param parent QObject pare de l'objecte que es crearà.
     * @return Retorna l'objecte convertit a la classe base BaseClass. En cas que no trobi l'objecte o error retornarà NULL.
     */
    BaseClass* create(const ClassIdentifier &className, ParentType* parent = 0) const
    {
        BaseClass* theObject = NULL;
        
        typename FunctionRegistry::const_iterator regEntry = m_registry.find(className);

        if (regEntry != m_registry.end()) 
        {
            try
            {
                theObject = regEntry->second(parent);
            }
            catch (std::bad_alloc)
            {
                theObject = NULL;
            }
        }
        return theObject;
    }

    QList<ClassIdentifier> getFactoryNamesList() const
    {
        QList<ClassIdentifier> list;
        
        typename FunctionRegistry::const_iterator item;
        for(item = m_registry.begin();  item != m_registry.end(); ++item)
        {
            list.append( item->first );
        }
        return list;
    }

private:
    FunctionRegistry m_registry;
};

}

#endif
