#ifndef HANGINGPROTOCOLDISPLAYSETTESTHELPER_H
#define HANGINGPROTOCOLDISPLAYSETTESTHELPER_H

namespace udg{
    class HangingProtocolDisplaySet;
}

namespace testing
{

/// Classe que retorna Imatges per utilitzar per testing
class HangingProtocolDisplaySetTestHelper
{
public:
    static udg::HangingProtocolDisplaySet* createHangingProtocolDisplaySet();
};

}

#endif // HANGINGPROTOCOLTESTHELPER_H