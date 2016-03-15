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

#ifndef UDG_HANGINGPROTOCOLIMAGESETRESTRICTIONEXPRESSION_H
#define UDG_HANGINGPROTOCOLIMAGESETRESTRICTIONEXPRESSION_H

#include "hangingprotocolimagesetrestriction.h"

#include <QMap>
#include <QString>

namespace udg {

class Image;
class Series;

/**
 * @brief The HangingProtocolImageSetRestrictionExpression class represents a boolean expression involving several restrictions of type
 * HangingProtocolImageSetRestriction. The expression is evaluated by evaluating all the restrictions and combining their results according to the expression.
 */
class HangingProtocolImageSetRestrictionExpression
{
public:
    /// Creates an expression that always evaluates to true.
    HangingProtocolImageSetRestrictionExpression();
    HangingProtocolImageSetRestrictionExpression(const QString &expression, const QMap<int, HangingProtocolImageSetRestriction> &restrictions);
    ~HangingProtocolImageSetRestrictionExpression();

    /// Evaluates the expression for the given series and returns the result.
    bool test(const Series *series) const;
    /// Evaluates the expression for the given image and returns the result.
    bool test(const Image *image) const;

private:
    /// Removes unwanted characters from the expression.
    void sanitize();
    /// Removes unused restrictions from the list.
    void filterUsedRestrictions();
    /// Transforms the expression to prepare it for evaluation.
    void prepareForEvaluation();
    /// Evaluates the expression with the given results for each restriction.
    bool evaluate(const QList<bool> &results) const;

private:
    /// Boolean expression that is evaluated.
    QString m_expression;
    /// Restrictions used in the expression.
    QMap<int, HangingProtocolImageSetRestriction> m_restrictions;

};

} // namespace udg

#endif // UDG_HANGINGPROTOCOLIMAGESETRESTRICTIONEXPRESSION_H
