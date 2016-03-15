/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "hangingprotocolimagesetrestrictionexpression.h"

#include "hangingprotocolimagesetrestriction.h"
#include "logging.h"

#include <QJSEngine>
#include <QRegularExpression>
#include <QSet>

namespace udg {

HangingProtocolImageSetRestrictionExpression::HangingProtocolImageSetRestrictionExpression()
    : m_expression("true")
{
}

HangingProtocolImageSetRestrictionExpression::HangingProtocolImageSetRestrictionExpression(const QString &expression,
                                                                                           const QMap<int, HangingProtocolImageSetRestriction> &restrictions)
    : m_expression(expression), m_restrictions(restrictions)
{
    sanitize();
    filterUsedRestrictions();
    prepareForEvaluation();
}

HangingProtocolImageSetRestrictionExpression::~HangingProtocolImageSetRestrictionExpression()
{
}

bool HangingProtocolImageSetRestrictionExpression::test(const Series *series) const
{
    QList<bool> results;

    foreach (const HangingProtocolImageSetRestriction &restriction, m_restrictions)
    {
        results.append(restriction.test(series));
    }

    return evaluate(results);
}

bool HangingProtocolImageSetRestrictionExpression::test(const Image *image) const
{
    QList<bool> results;

    foreach (const HangingProtocolImageSetRestriction &restriction, m_restrictions)
    {
        results.append(restriction.test(image));
    }

    return evaluate(results);
}

void HangingProtocolImageSetRestrictionExpression::sanitize()
{
    m_expression.replace(QRegularExpression("[^\\dandort()]"), "");

    if (m_expression.isEmpty())
    {
        m_expression = "true";
    }
}

void HangingProtocolImageSetRestrictionExpression::filterUsedRestrictions()
{
    QRegularExpression regularExpression("\\d+");
    QRegularExpressionMatchIterator it = regularExpression.globalMatch(m_expression);
    QSet<int> identifiers;

    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        identifiers << match.captured().toInt();
    }

    foreach (int i, m_restrictions.keys())
    {
        if (!identifiers.contains(i))
        {
            m_restrictions.remove(i);
        }
    }
}

void HangingProtocolImageSetRestrictionExpression::prepareForEvaluation()
{
    m_expression.replace("and", "&").replace("or", "|").replace("not", "!").replace(QRegularExpression("(\\d+)"), "%\\1");
}

bool HangingProtocolImageSetRestrictionExpression::evaluate(const QList<bool> &results) const
{
    QString expression = m_expression;

    foreach (bool result, results)
    {
        expression = expression.arg(result);
    }

    QJSEngine scriptEngine;
    QJSValue result = scriptEngine.evaluate(expression);

    if (!result.isError())
    {
        return result.toBool();
    }
    else
    {
        DEBUG_LOG(QString("Error while evaluating expression \"%1\": %2").arg(expression).arg(result.toString()));
        ERROR_LOG(QString("Error while evaluating expression \"%1\": %2").arg(expression).arg(result.toString()));
        return true;
    }
}

} // namespace udg
