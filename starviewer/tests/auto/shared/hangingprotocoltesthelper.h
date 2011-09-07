#ifndef HANGINGPROTOCOLTESTHELPER_H
#define HANGINGPROTOCOLTESTHELPER_H

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
};

}

#endif // HANGINGPROTOCOLTESTHELPER_H