#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "include/cache_packet.h"

namespace Kuiper {
    namespace Cache {
        const MemCmd::CommandInfo
            MemCmd::commandInfo[] = {
            /* InvalidCmd */
            { {}, InvalidCmd, "InvalidCmd" },
            /* ReadReq - Read issued by a non-caching agent such as a CPU or
             * device, with no restrictions on alignment. */
            { {IsRead, IsRequest, NeedsResponse}, ReadResp, "ReadReq" },
            /* ReadResp */
            { {IsRead, IsResponse, HasData}, InvalidCmd, "ReadResp" },
            /* ReadRespWithInvalidate */
            { {IsRead, IsResponse, HasData, IsInvalidate}, InvalidCmd, "ReadRespWithInvalidate" },
            /* WriteReq */
            { {IsWrite, NeedsWritable, IsRequest, NeedsResponse, HasData}, WriteResp, "WriteReq" },
            /* WriteResp */
            { {IsWrite, IsResponse}, InvalidCmd, "WriteResp" },
            /* WriteCompleteResp - The WriteCompleteResp command is needed
            * because in the GPU memory model we use a WriteResp to indicate
            * that a write has reached the cache controller so we can free
            * resources at the coalescer. Later, when the write succesfully
            * completes we send a WriteCompleteResp to the CU so its wait
            * counters can be updated. Wait counters in the CU is how memory
            * dependences are handled in the GPU ISA. */
            { {IsWrite, IsResponse}, InvalidCmd, "WriteCompleteResp" },
            /* WritebackDirty */
            { {IsWrite, IsRequest, IsEviction, HasData, FromCache}, InvalidCmd, "WritebackDirty" },
            /* WritebackClean - This allows the upstream cache to writeback a
            * line to the downstream cache without it being considered
            * dirty. */
            { {IsWrite, IsRequest, IsEviction, HasData, FromCache}, InvalidCmd, "WritebackClean" },
            /* WriteClean - This allows a cache to write a dirty block to a memory
            below without evicting its copy. */
            { {IsWrite, IsRequest, HasData, FromCache}, InvalidCmd, "WriteClean" },
            /* CleanEvict */
            { {IsRequest, IsEviction, FromCache}, InvalidCmd, "CleanEvict" },
            /* HardPFReq */
            { {IsRead, IsRequest, IsHWPrefetch, NeedsResponse, FromCache}, HardPFResp, "HardPFReq" },
            /* HardPFResp */
            { {IsRead, IsResponse, IsHWPrefetch, HasData}, InvalidCmd, "HardPFResp" },
            /* WriteLineReq */
            { {IsWrite, NeedsWritable, IsRequest, NeedsResponse, HasData}, WriteResp, "WriteLineReq" },
            /* UpgradeReq */
            { {IsInvalidate, NeedsWritable, IsUpgrade, IsRequest, NeedsResponse, FromCache}, UpgradeResp, "UpgradeReq" },
            /* SCUpgradeReq: response could be UpgradeResp or UpgradeFailResp */
            { {IsInvalidate, NeedsWritable, IsUpgrade, IsLlsc, IsRequest, NeedsResponse, FromCache}, UpgradeResp, "SCUpgradeReq" },
            /* UpgradeResp */
            { {IsUpgrade, IsResponse}, InvalidCmd, "UpgradeResp" },
            /* SCUpgradeFailReq: generates UpgradeFailResp but still gets the data */
            { {IsRead, NeedsWritable, IsInvalidate, IsLlsc, IsRequest, NeedsResponse, FromCache}, UpgradeFailResp, "SCUpgradeFailReq" },
            /* UpgradeFailResp - Behaves like a ReadExReq, but notifies an SC
            * that it has failed, acquires line as Dirty*/
            { {IsRead, IsResponse, HasData}, InvalidCmd, "UpgradeFailResp" },
            /* ReadExReq - Read issues by a cache, always cache-line aligned,
            * and the response is guaranteed to be writeable (exclusive or
            * even modified} */
            { {IsRead, NeedsWritable, IsInvalidate, IsRequest, NeedsResponse, FromCache}, ReadExResp, "ReadExReq" },
            /* ReadExResp - Response matching a read exclusive, as we check
            * the need for exclusive also on responses */
            { {IsRead, IsResponse, HasData}, InvalidCmd, "ReadExResp" },
            /* ReadCleanReq - Read issued by a cache, always cache-line
            * aligned, and the response is guaranteed to not contain dirty data
            * (exclusive or shared}.*/
            { {IsRead, IsRequest, NeedsResponse, FromCache}, ReadResp, "ReadCleanReq" },
            /* ReadSharedReq - Read issued by a cache, always cache-line
            * aligned, response is shared, possibly exclusive, owned or even
            * modified. */
            { {IsRead, IsRequest, NeedsResponse, FromCache}, ReadResp, "ReadSharedReq" },
            /* LoadLockedReq: note that we use plain ReadResp as response, so that
            *                we can also use ReadRespWithInvalidate when needed */
            { {IsRead, IsLlsc, IsRequest, NeedsResponse}, ReadResp, "LoadLockedReq" },
            /* StoreCondReq */
            { {IsWrite, NeedsWritable, IsLlsc, IsRequest, NeedsResponse, HasData}, StoreCondResp, "StoreCondReq" },
            /* StoreCondFailReq: generates failing StoreCondResp */
            { {IsWrite, NeedsWritable, IsLlsc, IsRequest, NeedsResponse, HasData}, StoreCondResp, "StoreCondFailReq" },
            /* StoreCondResp */
            { {IsWrite, IsLlsc, IsResponse}, InvalidCmd, "StoreCondResp" },
            /* LockedRMWReadReq */
            { {IsRead, IsLockedRMW, NeedsWritable, IsRequest, NeedsResponse}, LockedRMWReadResp, "LockedRMWReadReq" },
            /* LockedRMWReadResp */
            { {IsRead, IsLockedRMW, NeedsWritable, IsResponse, HasData}, InvalidCmd, "LockedRMWReadResp" },
            /* LockedRMWWriteReq */
            { {IsWrite, IsLockedRMW, NeedsWritable, IsRequest, NeedsResponse,
            HasData}, LockedRMWWriteResp, "LockedRMWWriteReq" },
            /* LockedRMWWriteResp */
            { {IsWrite, IsLockedRMW, NeedsWritable, IsResponse},
            InvalidCmd, "LockedRMWWriteResp" },
            /* SwapReq -- for Swap ldstub type operations */
            { {IsRead, IsWrite, NeedsWritable, IsRequest, HasData, NeedsResponse},
            SwapResp, "SwapReq" },
            /* SwapResp -- for Swap ldstub type operations */
            { {IsRead, IsWrite, IsResponse, HasData}, InvalidCmd, "SwapResp" },
            { {}, InvalidCmd, "Deprecated_MessageReq" },
            { {}, InvalidCmd, "Deprecated_MessageResp" },
            /* MemFenceReq -- for synchronization requests */
            {{IsRequest, NeedsResponse}, MemFenceResp, "MemFenceReq"},
            /* MemSyncReq */
            {{IsRequest, NeedsResponse}, MemSyncResp, "MemSyncReq"},
            /* MemSyncResp */
            {{IsResponse}, InvalidCmd, "MemSyncResp"},
            /* MemFenceResp -- for synchronization responses */
            {{IsResponse}, InvalidCmd, "MemFenceResp"},
            /* Cache Clean Request -- Update with the latest data all existing
            copies of the block down to the point indicated by the
            request */
            { {IsRequest, IsClean, NeedsResponse, FromCache}, CleanSharedResp, "CleanSharedReq" },
            /* Cache Clean Response - Indicates that all caches up to the
            specified point of reference have a up-to-date copy of the
            cache block or no copy at all */
            { {IsResponse, IsClean}, InvalidCmd, "CleanSharedResp" },
            /* Cache Clean and Invalidate Request -- Invalidate all existing
            copies down to the point indicated by the request */
            { {IsRequest, IsInvalidate, IsClean, NeedsResponse, FromCache}, CleanInvalidResp, "CleanInvalidReq" },
            /* Cache Clean and Invalidate Respose -- Indicates that no cache
            above the specified point holds the block and that the block
            was written to a memory below the specified point. */
            { {IsResponse, IsInvalidate, IsClean}, InvalidCmd, "CleanInvalidResp" },
            /* InvalidDestError  -- packet dest field invalid */
            { {IsResponse, IsError}, InvalidCmd, "InvalidDestError" },
            /* BadAddressError   -- memory address invalid */
            { {IsResponse, IsError}, InvalidCmd, "BadAddressError" },
            /* ReadError -- packet dest unable to fulfill read command */
            { {IsRead, IsResponse, IsError}, InvalidCmd, "ReadError" },
            /* WriteError -- packet dest unable to fulfill write command */
            { {IsWrite, IsResponse, IsError}, InvalidCmd, "WriteError" },
            /* FunctionalReadError */
            { {IsRead, IsResponse, IsError}, InvalidCmd, "FunctionalReadError" },
            /* FunctionalWriteError */
            { {IsWrite, IsResponse, IsError}, InvalidCmd, "FunctionalWriteError" },
            /* PrintReq */
            { {IsRequest, IsPrint}, InvalidCmd, "PrintReq" },
            /* Flush Request */
            { {IsRequest, IsFlush, NeedsWritable}, InvalidCmd, "FlushReq" },
            /* Invalidation Request */
            { {IsInvalidate, IsRequest, NeedsWritable, NeedsResponse, FromCache},
            InvalidateResp, "InvalidateReq" },
            /* Invalidation Response */
            { {IsInvalidate, IsResponse}, InvalidCmd, "InvalidateResp" },
            // hardware transactional memory
        { {IsRead, IsRequest, NeedsResponse}, HTMReqResp, "HTMReq" },
        { {IsRead, IsResponse}, InvalidCmd, "HTMReqResp" },
        { {IsRead, IsRequest}, InvalidCmd, "HTMAbort" },
        };

        //AddrRange
        //    Packet::getAddrRange() const
        //{
        //    return RangeSize(getAddr(), getSize());
        //}

        //bool Packet::trySatisfyFunctional(Printable* obj, Addr addr, bool is_secure, int size,
        //        uint8_t* _data)
        //{
        //    const Addr func_start = getAddr();
        //    const Addr func_end = getAddr() + getSize() - 1;
        //    const Addr val_start = addr;
        //    const Addr val_end = val_start + size - 1;

        //    if (is_secure != _isSecure || func_start > val_end ||
        //        val_start > func_end) {
        //        // no intersection
        //        return false;
        //    }

        //    // check print first since it doesn't require data
        //    if (isPrint()) {
        //        assert(!_data);
        //        safe_cast<PrintReqState*>(senderState)->printObj(obj);
        //        return false;
        //    }

        //    // we allow the caller to pass NULL to signify the other packet
        //    // has no data
        //    if (!_data) {
        //        return false;
        //    }

        //    const Addr val_offset = func_start > val_start ?
        //        func_start - val_start : 0;
        //    const Addr func_offset = func_start < val_start ?
        //        val_start - func_start : 0;
        //    const Addr overlap_size = std::min(val_end, func_end) + 1 -
        //        std::max(val_start, func_start);

        //    if (isRead()) {
        //        std::memcpy(getPtr<uint8_t>() + func_offset,
        //            _data + val_offset,
        //            overlap_size);

        //        // initialise the tracking of valid bytes if we have not
        //        // used it already
        //        if (bytesValid.empty())
        //            bytesValid.resize(getSize(), false);

        //        // track if we are done filling the functional access
        //        bool all_bytes_valid = true;

        //        int i = 0;

        //        // check up to func_offset
        //        for (; all_bytes_valid && i < func_offset; ++i)
        //            all_bytes_valid &= bytesValid[i];

        //        // update the valid bytes
        //        for (i = func_offset; i < func_offset + overlap_size; ++i)
        //            bytesValid[i] = true;

        //        // check the bit after the update we just made
        //        for (; all_bytes_valid && i < getSize(); ++i)
        //            all_bytes_valid &= bytesValid[i];

        //        return all_bytes_valid;
        //    }
        //    else if (isWrite()) {
        //        std::memcpy(_data + val_offset,
        //            getConstPtr<uint8_t>() + func_offset,
        //            overlap_size);
        //    }
        //    else {
        //        panic("Don't know how to handle command %s\n", cmdString());
        //    }

        //    // keep going with request by default
        //    return false;
        //}

        void Packet::copyResponderFlags(const PacketPtr pkt) {
            assert(isRequest());
            // If we have already found a responder, no other cache should
            // commit to responding
            assert(!pkt->cacheResponding() || !cacheResponding());
            flags.set(pkt->flags & RESPONDER_FLAGS);
        }

        void Packet::pushSenderState(Packet::SenderState* sender_state) {
            assert(sender_state != NULL);
            sender_state->predecessor = senderState;
            senderState = sender_state;
        }

        Packet::SenderState* Packet::popSenderState() {
            assert(senderState != NULL);
            SenderState* sender_state = senderState;
            senderState = sender_state->predecessor;
            sender_state->predecessor = NULL;
            return sender_state;
        }

        std::string
            Packet::print( const int verbosity,
                const std::string& prefix) const
        {
            std::string info;
            info.resize(100);

            sprintf(&info[0], "%s%s [%lx:%lx]", prefix.c_str(), cmdString().c_str(),
                getAddr(), getAddr() + getSize() - 1);
            return info;
        }

        std::string
            Packet::print() const {
            return print(0);
        }

        bool Packet::matchBlockAddr(const Addr addr, const bool is_secure,
                const int blk_size) const {
            return (getBlockAddr(blk_size) == addr) && (isSecure() == is_secure);
        }

        bool Packet::matchBlockAddr(const PacketPtr pkt, const int blk_size) const {
            return matchBlockAddr(pkt->getBlockAddr(blk_size), pkt->isSecure(),
                blk_size);
        }

        bool Packet::matchAddr(const Addr addr, const bool is_secure) const {
            return (getAddr() == addr) && (isSecure() == is_secure);
        }

        bool   Packet::matchAddr(const PacketPtr pkt) const {
            return matchAddr(pkt->getAddr(), pkt->isSecure());
        }

 /*       Packet::PrintReqState::PrintReqState(std::ostream& _os, int _verbosity)
            : curPrefixPtr(new std::string("")), os(_os), verbosity(_verbosity)
        {
            labelStack.push_back(LabelStackEntry("", curPrefixPtr));
        }

        Packet::PrintReqState::~PrintReqState()
        {
            labelStack.pop_back();
            assert(labelStack.empty());
            delete curPrefixPtr;
        }

        Packet::PrintReqState::
            LabelStackEntry::LabelStackEntry(const std::string& _label,
                std::string* _prefix)
            : label(_label), prefix(_prefix), labelPrinted(false)
        {
        }

        void
            Packet::PrintReqState::pushLabel(const std::string& lbl,
                const std::string& prefix)
        {
            labelStack.push_back(LabelStackEntry(lbl, curPrefixPtr));
            curPrefixPtr = new std::string(*curPrefixPtr);
            *curPrefixPtr += prefix;
        }

        void
            Packet::PrintReqState::popLabel()
        {
            delete curPrefixPtr;
            curPrefixPtr = labelStack.back().prefix;
            labelStack.pop_back();
            assert(!labelStack.empty());
        }

        void
            Packet::PrintReqState::printLabels()
        {
            if (!labelStack.back().labelPrinted) {
                LabelStack::iterator i = labelStack.begin();
                LabelStack::iterator end = labelStack.end();
                while (i != end) {
                    if (!i->labelPrinted) {
                        ccprintf(os, "%s%s\n", *(i->prefix), i->label);
                        i->labelPrinted = true;
                    }
                    i++;
                }
            }
        }

        void
            Packet::PrintReqState::printObj(Printable* obj)
        {
            printLabels();
            obj->print(os, verbosity, curPrefix());
        }*/
    } /* namespace Cache */ 
} /* namespace Kuiper */ 