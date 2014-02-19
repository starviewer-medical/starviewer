#ifndef UDGUSERMESSAGE_H
#define UDGUSERMESSAGE_H

#include <QString>

namespace udg {

/**
    Class to centralize common messages shown to the user by different classes
 */
class UserMessage {
public:
    UserMessage();
    ~UserMessage();

    static QString getProblemPersistsAdvice();

    static QString getCloseWindowsAndTryAgainAdvice();
};

} // End namespace udg

#endif
