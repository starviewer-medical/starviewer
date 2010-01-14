/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "applicationcommandlineoptions.h"

#include <QString>
#include <QObject>

namespace udg {

const QString ApplicationCommandLineOptions::optionSelectorCharacter("-");

bool ApplicationCommandLineOptions::addOption(QString optionName, QString description) 
{
    if (!m_commandLineOptions.contains(optionName))
    {
        m_commandLineOptions.insert(optionName, description);
        return true;
    }
    else return false;
}

bool ApplicationCommandLineOptions::parseArgumentList(QStringList argumentList)
{
    m_argumentList = argumentList;
    return parse();
}

QStringList ApplicationCommandLineOptions::getArgumentList()
{
    return m_argumentList;
}

/**Les comandes de línia està compostes per un opció que es precedida d'un '-' i llavors pot contenir a continuació un argument,
  * indicant el valor de la opció*/
bool ApplicationCommandLineOptions::parse()
{
    QStringList argumentList = m_argumentList;
    QString lastOption, parameter;
    bool lastParameterWasAnOption = false;

    m_parserErrorMessage = "";
    argumentList.removeFirst();//Treiem el primer string que és el nom de l'aplicació;

    while (!argumentList.isEmpty())
    {
        parameter = argumentList.takeFirst();

        if (isAnOption(parameter))
        {
            parameter = parameter.right(parameter.length() -1); //treiem el "-" del paràmetre
            if (m_commandLineOptions.contains(parameter)) //Comprovem si és una opció vàlida
            {
                //Si és una opció que ens han especificat com a vàlida l'inserim com a parsejada, de moment com argument de l'opció hi posem ""     
                m_parsedOptions.insert(parameter, "");
                
                lastParameterWasAnOption = true;
                lastOption = parameter;
            }
            else m_parserErrorMessage += parameter + QObject::tr(" is an invalid option.\n");
        }
        else 
        {
            //és un argument
            if (lastParameterWasAnOption)
            {
                //Si tenim un argument i l'últim paràmetre era un opció, vol dir aquest paràmetre és un argument
                m_parsedOptions[lastOption] = parameter;
            }
			else m_parserErrorMessage += parameter + QObject::tr(" ignored value.\n");

            lastOption = "";
            lastParameterWasAnOption = false;
        }
    }

    return m_parserErrorMessage.isEmpty();
}

bool ApplicationCommandLineOptions::isSet(QString optionName)
{
    return m_parsedOptions.contains(optionName);
}

QString ApplicationCommandLineOptions::getOptionArgument(QString optionName)
{
    if (isSet(optionName))
    {
        return m_parsedOptions[optionName];
    }
    else return NULL;
}

int ApplicationCommandLineOptions::getNumberOfParsedOptions()
{
    return m_parsedOptions.count();
}

QString ApplicationCommandLineOptions::getParserErrorMessage()
{
    return m_parserErrorMessage;
}

bool ApplicationCommandLineOptions::isAnOption(QString optionName)
{
    return optionName.startsWith(optionSelectorCharacter); //Les opcions sempre comencen "-"
}

bool ApplicationCommandLineOptions::isAnArgument(QString argument)
{
    return !argument.startsWith(optionSelectorCharacter);
}

}
