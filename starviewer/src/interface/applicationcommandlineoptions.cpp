#include "applicationcommandlineoptions.h"

#include <QString>
#include <QObject>

namespace udg {

ApplicationCommandLineOptions::ApplicationCommandLineOptions(const QString &applicationName)
{
    m_applicationName = applicationName;
}

QString ApplicationCommandLineOptions::getApplicationName() const
{
    return m_applicationName;
}

bool ApplicationCommandLineOptions::addOption(const CommandLineOption &option)
{
    if (!m_commandLineOptions.contains(option.getName()))
    {
        m_commandLineOptions.insert(option.getName(), option);
        return true;
    }
    else
    {
        return false;
    }
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

// Les comandes de línia està compostes per un opció que es precedida d'un '-' i llavors pot contenir a continuació un argument,
// indicant el valor de la opció
bool ApplicationCommandLineOptions::parse()
{
    QStringList argumentList = m_argumentList;
    QString parameter;
    bool lastParameterWasAnOption = false, nextParameterHasToBeAnArgumentOption = false;
    CommandLineOption lastOption;

    m_parserErrorMessage = "";
    // Treiem el primer string que és el nom de l'aplicació
    argumentList.removeFirst();

    // Mentre hi hagi arguments per processar o no s'hagi produït un error parsegem els arguments
    while (!argumentList.isEmpty() && m_parserErrorMessage.isEmpty())
    {
        parameter = argumentList.takeFirst();

        if (isAnOption(parameter))
        {
            if (!nextParameterHasToBeAnArgumentOption)
            {
                // Treiem el "-" del paràmetre
                parameter = parameter.right(parameter.length() -1);
                // Comprovem si és una opció vàlida
                if (m_commandLineOptions.contains(parameter))
                {
                    // Si és una opció que ens han especificat com a vàlida l'inserim com a parsejada, de moment com argument de l'opció hi posem ""
                    m_parsedOptions.insert(parameter, "");

                    lastParameterWasAnOption = true;
                    lastOption = m_commandLineOptions.value(parameter);
                    nextParameterHasToBeAnArgumentOption = lastOption.requiresArgument();
                }
                else
                {
                    m_parserErrorMessage += QObject::tr("Unknown option ") + CommandLineOption::OptionSelectorPrefix + parameter + "\n";
                }
            }
            else
            {
                // Si l'últim paràmetre parsejat era una opció que se li havia de passar obligatòriament un argument ex "-accessionnumber 12345"
                // i no se li ha especificat cap argument ex: "-accessionnumber -studyUID" guardem l'error i parem.
                m_parserErrorMessage += lastOption.getName() + QObject::tr(" option requires an argument") + "\n";
            }
        }
        else
        {
            // És un argument
            if (lastParameterWasAnOption)
            {
                // Si tenim un argument i l'últim paràmetre era un opció, vol dir aquest paràmetre és un argument
                m_parsedOptions[lastOption.getName()] = parameter;
            }
            else
            {
                m_parserErrorMessage += QObject::tr("Unexpected value ") + parameter + "\n";
            }

            lastParameterWasAnOption = false;
            nextParameterHasToBeAnArgumentOption = false;
        }
    }

    if (nextParameterHasToBeAnArgumentOption)
    {
        m_parserErrorMessage += lastOption.getName() + QObject::tr(" option requires an argument") + "\n";
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
    else
    {
        return NULL;
    }
}

int ApplicationCommandLineOptions::getNumberOfParsedOptions()
{
    return m_parsedOptions.count();
}

QString ApplicationCommandLineOptions::getParserErrorMessage()
{
    return m_parserErrorMessage;
}

QString ApplicationCommandLineOptions::getSynopsis() const
{
    QString synopsis;

    synopsis = QObject::tr("Synopsis:");
    synopsis += "\n";
    synopsis += m_applicationName;

    QString applicationOptions;
    QString detailedOptions;
    foreach (const CommandLineOption &option, m_commandLineOptions.values())
    {
        applicationOptions += " [" + option.toString(false) + "]";
        detailedOptions += option.toString(true) + "\n\n";
    }

    synopsis += applicationOptions + "\n";
    synopsis += "\n" + QObject::tr("Options:");
    synopsis += "\n" + detailedOptions;

    return synopsis;
}

QList<CommandLineOption> ApplicationCommandLineOptions::getCommandLineOptionsList() const
{
    return m_commandLineOptions.values();
}

bool ApplicationCommandLineOptions::isAnOption(QString optionName)
{
    // Les opcions sempre comencen "-"
    return optionName.startsWith(CommandLineOption::OptionSelectorPrefix);
}

bool ApplicationCommandLineOptions::isAnArgument(QString argument)
{
    return !argument.startsWith(CommandLineOption::OptionSelectorPrefix);
}

}
