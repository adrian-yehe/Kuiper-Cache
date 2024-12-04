#include <cassert>
#include "intmath.h"
#include "include/base.h"
#include "cache/include/cache_base.h"
#include "params/BasePrefetcher.h"

namespace Kuiper {
    namespace Cache {
        namespace prefetch {

            Base::PrefetchInfo::PrefetchInfo(PacketPtr pkt, Addr addr, bool miss)
                : address(addr), pc(pkt->req->hasPC() ? pkt->req->getPC() : 0),
                  requestorId(pkt->req->requestorId()), validPC(pkt->req->hasPC()),
                  secure(pkt->isSecure()), size(pkt->req->getSize()), write(pkt->isWrite()),
                  paddress(pkt->req->getPaddr()), cacheMiss(miss) {
                unsigned int req_size = pkt->req->getSize();
                if (!write && miss)
                {
                    data = nullptr;
                }
                else
                {
                    data = new uint8_t[req_size];
                    Addr offset = pkt->req->getPaddr() - pkt->getAddr();
                    std::memcpy(data, &(pkt->getConstPtr<uint8_t>()[offset]), req_size);
                }
            }

            Base::PrefetchInfo::PrefetchInfo(PrefetchInfo const &pfi, Addr addr)
                : address(addr), pc(pfi.pc), requestorId(pfi.requestorId),
                  validPC(pfi.validPC), secure(pfi.secure), size(pfi.size),
                  write(pfi.write), paddress(pfi.paddress), cacheMiss(pfi.cacheMiss),
                  data(nullptr)
            {
            }

            void
            Base::PrefetchListener::notify(const CacheAccessProbeArg &arg) {
                if (isFill) {
                    parent.notifyFill(arg);
                }
                else {
                    parent.probeNotify(arg, miss);
                }
            }

            void
            Base::PrefetchEvictListener::notify(const EvictionInfo &info)
            {
                if (info.newData.empty())
                    parent.notifyEvict(info);
            }

            Base::Base(const BasePrefetcherParams &p): 
                  probeManager(nullptr),
                  blkSize(p.block_size), lBlkSize(floorLog2(blkSize)),
                  onMiss(p.on_miss), onRead(p.on_read),
                  onWrite(p.on_write), onData(p.on_data), onInst(p.on_inst),
                //   requestorId(p.sys->getRequestorId(this)),
                  requestorId(0),
                  pageBytes(p.page_bytes),
                  prefetchOnAccess(p.prefetch_on_access),
                  prefetchOnPfHit(p.prefetch_on_pf_hit),
                  useVirtualAddresses(p.use_virtual_addresses),
                //   prefetchStats(this), 
                issuedPrefetches(0),
                  usefulPrefetches(0)
            {
            }

            void
            Base::setParentInfo(ProbeManager *pm, unsigned blk_size)
            {
                assert(!system && !probeManager);
                probeManager = pm;
                // If the cache has a different block size from the system's, save it
                blkSize = blk_size;
                lBlkSize = floorLog2(blkSize);
            }

            // Base::StatGroup::StatGroup(statistics::Group *parent)
            //     : statistics::Group(parent),
            //       ADD_STAT(demandMshrMisses, statistics::units::Count::get(),
            //                "demands not covered by prefetchs"),
            //       ADD_STAT(pfIssued, statistics::units::Count::get(),
            //                "number of hwpf issued"),
            //       ADD_STAT(pfUnused, statistics::units::Count::get(),
            //                "number of HardPF blocks evicted w/o reference"),
            //       ADD_STAT(pfUseful, statistics::units::Count::get(),
            //                "number of useful prefetch"),
            //       ADD_STAT(pfUsefulButMiss, statistics::units::Count::get(),
            //                "number of hit on prefetch but cache block is not in an usable "
            //                "state"),
            //       ADD_STAT(accuracy, statistics::units::Count::get(),
            //                "accuracy of the prefetcher"),
            //       ADD_STAT(coverage, statistics::units::Count::get(),
            //                "coverage brought by this prefetcher"),
            //       ADD_STAT(pfHitInCache, statistics::units::Count::get(),
            //                "number of prefetches hitting in cache"),
            //       ADD_STAT(pfHitInMSHR, statistics::units::Count::get(),
            //                "number of prefetches hitting in a MSHR"),
            //       ADD_STAT(pfHitInWB, statistics::units::Count::get(),
            //                "number of prefetches hit in the Write Buffer"),
            //       ADD_STAT(pfLate, statistics::units::Count::get(),
            //                "number of late prefetches (hitting in cache, MSHR or WB)")
            // {
            //     using namespace statistics;

            //     pfUnused.flags(nozero);

            //     accuracy.flags(total);
            //     accuracy = pfUseful / pfIssued;

            //     coverage.flags(total);
            //     coverage = pfUseful / (pfUseful + demandMshrMisses);

            //     pfLate = pfHitInCache + pfHitInMSHR + pfHitInWB;
            // }

            bool
            Base::observeAccess(const PacketPtr &pkt, bool miss, bool prefetched) const
            {
                bool fetch = pkt->req->isInstFetch();
                bool read = pkt->isRead();
                bool inv = pkt->isInvalidate();

                if (!miss)
                {
                    if (prefetchOnPfHit)
                        return prefetched;
                    if (!prefetchOnAccess)
                        return false;
                }
                if (pkt->req->isUncacheable())
                    return false;
                if (fetch && !onInst)
                    return false;
                if (!fetch && !onData)
                    return false;
                if (!fetch && read && !onRead)
                    return false;
                if (!fetch && !read && !onWrite)
                    return false;
                if (!fetch && !read && inv)
                    return false;
                if (pkt->cmd == MemCmd::CleanEvict)
                    return false;

                if (onMiss)
                {
                    return miss;
                }

                return true;
            }

            bool
            Base::samePage(Addr a, Addr b) const
            {
                return roundDown(a, pageBytes) == roundDown(b, pageBytes);
            }

            Addr
            Base::blockAddress(Addr a) const
            {
                return a & ~((Addr)blkSize - 1);
            }

            Addr
            Base::blockIndex(Addr a) const
            {
                return a >> lBlkSize;
            }

            Addr
            Base::pageAddress(Addr a) const
            {
                return roundDown(a, pageBytes);
            }

            Addr
            Base::pageOffset(Addr a) const
            {
                return a & (pageBytes - 1);
            }

            Addr
            Base::pageIthBlockAddress(Addr page, uint32_t blockIndex) const
            {
                return page + (blockIndex << lBlkSize);
            }

            void
            Base::probeNotify(const CacheAccessProbeArg &acc, bool miss)
            {
                const PacketPtr pkt = acc.pkt;
                const CacheAccessor &cache = acc.cache;

                // Don't notify prefetcher on SWPrefetch, cache maintenance
                // operations or for writes that we are coaslescing.
                if (pkt->cmd.isSWPrefetch())
                    return;
                if (pkt->req->isCacheMaintenance())
                    return;
                if (pkt->isWrite() && cache.coalesce())
                    return;
                if (!pkt->req->hasPaddr())
                {
                    panic("Request must have a physical address");
                }

                bool has_been_prefetched =
                    acc.cache.hasBeenPrefetched(pkt->getAddr(), pkt->isSecure(),
                                                requestorId);
                if (has_been_prefetched)
                {
                    usefulPrefetches += 1;
                    // prefetchStats.pfUseful++;
                    // if (miss)
                    //     // This case happens when a demand hits on a prefetched line
                    //     // that's not in the requested coherency state.
                    //     prefetchStats.pfUsefulButMiss++;
                }

                // Verify this access type is observed by prefetcher
                if (observeAccess(pkt, miss, has_been_prefetched))
                {
                    if (useVirtualAddresses && pkt->req->hasVaddr())
                    {
                        PrefetchInfo pfi(pkt, pkt->req->getVaddr(), miss);
                        notify(acc, pfi);
                    }
                    else if (!useVirtualAddresses)
                    {
                        PrefetchInfo pfi(pkt, pkt->req->getPaddr(), miss);
                        notify(acc, pfi);
                    }
                }
            }

            void
            Base::regProbeListeners() {
                /**
                 * If no probes were added by the configuration scripts, connect to the
                 * parent cache using the probe "Miss". Also connect to "Hit", if the
                 * cache is configured to prefetch on accesses.
                 */
                if (listeners.empty() && probeManager != nullptr) {
                    listeners.push_back(new PrefetchListener(*this, probeManager,
                                                             "Miss", false, true));
                    listeners.push_back(new PrefetchListener(*this, probeManager,
                                                             "Fill", true, false));
                    listeners.push_back(new PrefetchListener(*this, probeManager,
                                                             "Hit", false, false));
                    listeners.push_back(new PrefetchEvictListener(*this, probeManager,
                                                                  "Data Update"));
                }
            }

            // void
            // Base::addEventProbe(SimObject *obj, const char *name) {
            //     // ProbeManager *pm(obj->getProbeManager());
            //     // listeners.push_back(new PrefetchListener(*this, pm, name));
            // }
        } // namespace prefetch
    } // namespace Cache
} // namespace Kuiper
