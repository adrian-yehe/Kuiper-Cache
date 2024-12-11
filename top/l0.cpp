#include "include/l0.h"

namespace Kuiper {
    namespace Cache {
        L0::L0(const std::string &_name, const Params &_params) : NoncoherentCache(_params),
                                                                       Initiator("socket"),
                                                                       mLoad0("load0Port", this),
                                                                       mLoad1("load1Port", this),
                                                                       mStore("storePort", this),
                                                                       // mMemSPort("l0"),
                                                                       mLoad0Fifo("load0_fifo", 16),
                                                                       mLoad1Fifo("load1_fifo", 16),
                                                                       mStoreFifo("store_fifo", 10)
        {
            mLoad0.mReqPort(mLoad0Fifo);
            mLoad1.mReqPort(mLoad1Fifo);
            mStore.mReqPort(mStoreFifo);
        };

        void L0::BindLoad0(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mLoad0Fifo);
            mLoad0.mResPort(_res_signal);
        };
        void L0::BindLoad1(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mLoad1Fifo);
            mLoad1.mResPort(_res_signal);
        };
        void L0::BindStore(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mStoreFifo);
            mStore.mResPort(_res_signal);
        };
    } /* namespace Cache */
} /* namesapce Kuiper */
