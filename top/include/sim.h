#ifndef _SIM_OBJECT_H__
#define _SIM_OBJECT_H__
#include <cstdint>
#include "params/CacheNoncoherent.h"
#include "probe.h"

namespace Kuiper {
    namespace Cache {
        class SimObject {
            public:
                auto GetL0ParamsPtr(void) { return &simL0Params; };
                auto GetL1ParamsPtr(void) { return &simL1Params; };

            private:
                void InitSimObject();
                void InitL0BaseCache();
                void InitL0SetAssoc();
                void InitL0Tags();
                void InitL0Replacement();
                void InitL0Tagged();

            private:
                NoncoherentCacheParams  simL0Params;
                NoncoherentCacheParams  simL1Params;

            private:
                /** Manager coordinates hooking up probe points with listeners. */
                ProbeManager *probeManager;

            public:
                /**
                 * Get the probe manager for this object.
                 *
                 * Probes generate traces. A trace is a file that
                 * keeps a log of events. For example, we can have a probe
                 * listener for an address and the trace will be a file that
                 * has time stamps for all the reads and writes to that address.
                 *
                 * @ingroup api_simobject
                 */
                ProbeManager *getProbeManager() { return probeManager; };

            public:
                SimObject() { InitSimObject(); };
        };
    }
}
#endif /* _SIM_OBJECT_H__ */
