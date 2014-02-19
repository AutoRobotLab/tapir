#include "HistoryEntry.hpp"

#include <iostream>

#include "BeliefNode.hpp"
#include "ChangeFlags.hpp"              // for ChangeFlags
#include "StateInfo.hpp"                // for StateInfo

namespace solver {
class HistorySequence;

HistoryEntry::HistoryEntry() :
    HistoryEntry(1.0, nullptr, 0) {
}

HistoryEntry::HistoryEntry(double discount, HistorySequence* owningSequence,
        long entryId) :
    stateInfo_(nullptr),
    action_(nullptr),
    observation_(),
    hasBeenBackedUp_(false),
    entryId_(entryId),
    discount_(discount),
    immediateReward_(0),
    totalDiscountedReward_(0),
    owningSequence_(owningSequence),
    owningBeliefNode_(nullptr),
    changeFlags_(ChangeFlags::UNCHANGED) {
}

HistoryEntry::HistoryEntry(StateInfo *stateInfo,
        double discount, HistorySequence* owningSequence,
        long entryId) :
                HistoryEntry(discount, owningSequence, entryId) {
    registerState(stateInfo);
}

HistoryEntry::~HistoryEntry() {
}

State const *HistoryEntry::getState() {
    return stateInfo_->getState();
}

void HistoryEntry::resetChangeFlags() {
    changeFlags_ = ChangeFlags::UNCHANGED;
}

void HistoryEntry::setChangeFlags(ChangeFlags flags) {
    changeFlags_ |= flags;
}

void HistoryEntry::registerNode(BeliefNode *node) {
    if (owningBeliefNode_ == node) {
        return;
    }
    if (owningBeliefNode_ != nullptr) {
        owningBeliefNode_->removeParticle(this);
        owningBeliefNode_ = nullptr;
    }
    if (node != nullptr) {
        owningBeliefNode_ = node;
        owningBeliefNode_->addParticle(this);
    }
}

void HistoryEntry::registerState(StateInfo *info) {
    if (stateInfo_ == info) {
        return;
    }
    if (stateInfo_ != nullptr) {
        stateInfo_->removeHistoryEntry(this);
        stateInfo_ = nullptr;
    }
    if (info != nullptr) {
        stateInfo_ = info;
        stateInfo_->addHistoryEntry(this);
    }
}
} /* namespace solver */
