#ifndef HANGINGPROTOCOLIMAGESETTESTHELPER_H
#define HANGINGPROTOCOLIMAGESETTESTHELPER_H

namespace udg{
    class HangingProtocolImageSet;
}

namespace testing
{

/// Classe que retorna Imatges per utilitzar per testing
class HangingProtocolImageSetTestHelper
{
public:
    static udg::HangingProtocolImageSet* createHangingProtocolImageSet();
};

}

#endif // HANGINGPROTOCOLTESTHELPER_H