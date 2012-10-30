#ifndef HANGINGPROTOCOLTESTHELPER_H
#define HANGINGPROTOCOLTESTHELPER_H

#include <QString>

namespace udg{
    class HangingProtocol;
}

namespace testing
{

/// Classe que retorna Imatges per utilitzar per testing
class HangingProtocolTestHelper
{
public:
    static udg::HangingProtocol* createEmptyHangingProtocol();
    static udg::HangingProtocol* createHangingProtocolWithoutImageSetAndDisplaySet();
    static udg::HangingProtocol* createHangingProtocolWithOneImageSetAndOneDisplaySet();
    static udg::HangingProtocol* createHangingProtocolWithAttributes(QString name, int priority, bool strictness, bool allDifferent, bool previous, int identifier, int imageSets, int displaySets);
};

}

#endif // HANGINGPROTOCOLTESTHELPER_H