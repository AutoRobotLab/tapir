#include "RockSampleTextSerializer.hpp"

#include <iostream>                     // for operator<<, basic_ostream, basic_istream<>::__istream_type, basic_ostream<>::__ostream_type, cerr, endl
#include <string>                       // for operator>>, string
#include <vector>                       // for vector

#include "global.hpp"                     // for make_unique
#include "problems/shared/GridPosition.hpp"  // for GridPosition
#include "solver/Action.hpp"
#include "solver/State.hpp"
#include "solver/TextSerializer.hpp"    // for TextSerializer
#include "solver/Observation.hpp"

#include "RockSampleAction.hpp"         // for RockSampleAction
#include "RockSampleObservation.hpp"    // for RockSampleObservation
#include "RockSampleState.hpp"          // for RockSampleState

namespace solver {
class Solver;
} /* namespace solver */

namespace rocksample {
RockSampleTextSerializer::RockSampleTextSerializer() :
    solver::TextSerializer() {
}

RockSampleTextSerializer::RockSampleTextSerializer(solver::Solver *solver) :
    solver::TextSerializer(solver) {
}

void RockSampleTextSerializer::saveState(solver::State const *state,
        std::ostream &os) {
    if (state == nullptr) {
        os << "NULL";
        return;
    }
    RockSampleState const &rockSampleState =
        static_cast<RockSampleState const &>(*state);
    os << rockSampleState.position_.i << " " << rockSampleState.position_.j
       << " ";
    for (bool isGood : rockSampleState.getRockStates()) {
        if (isGood) {
            os << 'G';
        } else {
            os << 'B';
        }
    }
}

std::unique_ptr<solver::State> RockSampleTextSerializer::loadState(
        std::istream &is) {
    std::string text;
    is >> text;
    if (text == "NULL") {
        return nullptr;
    }
    long i, j;
    std::string rockString;
    std::vector<bool> rockStates;
    std::stringstream sstr(text);
    sstr >> i;
    is >> j >> rockString;
    for (char c : rockString) {
        if (c == 'G') {
            rockStates.push_back(true);
        } else if (c == 'B') {
            rockStates.push_back(false);
        } else {
            std::cerr << "Error; invalid rock state: " << c << std::endl;
        }
    }
    return std::make_unique<RockSampleState>(GridPosition(i, j), rockStates);
}

void RockSampleTextSerializer::saveObservation(solver::Observation const *obs,
        std::ostream &os) {
    if (obs == nullptr) {
        os << "NULL";
        return;
    }
    RockSampleObservation const &observation =
            static_cast<RockSampleObservation const &>(*obs);
    if (observation.isEmpty()) {
        os << "NONE";
    } else if (observation.isGood_) {
        os << "GOOD";
    } else {
        os << "BAD";
    }
}

std::unique_ptr<solver::Observation> RockSampleTextSerializer::loadObservation(
        std::istream &is) {
    std::string text;
    is >> text;
    if (text == "NULL") {
        return nullptr;
    } else if (text == "NONE") {
        return std::make_unique<RockSampleObservation>(true, true);
    } else if (text == "GOOD") {
        return std::make_unique<RockSampleObservation>(false, true);
    } else if (text == "BAD") {
        return std::make_unique<RockSampleObservation>(false, false);
    } else {
        std::cerr << "ERROR: Invalid observation!" << std::endl;
        return nullptr;
    }
}


void RockSampleTextSerializer::saveAction(solver::Action const *action,
        std::ostream &os) {
    if (action == nullptr) {
        os << "NULL";
        return;
    }
    RockSampleAction const &a =
            static_cast<RockSampleAction const &>(*action);
    RockSampleAction::Code code = a.getCode();
    if (code == RockSampleAction::Code::CHECK) {
        os << "CHECK-" << a.getRockNo();
        return;
    }
    switch (code) {
    case RockSampleAction::Code::NORTH:
        os << "NORTH";
        break;
    case RockSampleAction::Code::EAST:
        os << "EAST";
        break;
    case RockSampleAction::Code::SOUTH:
        os << "SOUTH";
        break;
    case RockSampleAction::Code::WEST:
        os << "WEST";
        break;
    case RockSampleAction::Code::SAMPLE:
        os << "SAMPLE";
        break;
    default:
        os << "ERROR-" << static_cast<long>(code);
        break;
    }
}

std::unique_ptr<solver::Action> RockSampleTextSerializer::loadAction(
        std::istream &is) {
    std::string text;
    is >> text;
    if (text == "NULL") {
        return nullptr;
    } else if (text == "NORTH") {
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::NORTH);
    } else if (text == "EAST") {
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::EAST);
    } else if (text == "SOUTH") {
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::SOUTH);
    } else if (text == "WEST") {
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::WEST);
    } else if (text == "SAMPLE") {
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::SAMPLE);
    } else if (text.find("CHECK") != std::string::npos) {
        std::string tmpStr;
        std::stringstream sstr(text);
        std::getline(sstr, tmpStr, '-');
        long rockNo;
        sstr >> rockNo;
        return std::make_unique<RockSampleAction>(RockSampleAction::Code::CHECK, rockNo);
    } else {
        std::string tmpStr;
        std::stringstream sstr(text);
        std::getline(sstr, tmpStr, '-');
        long code;
        sstr >> code;
        std::cerr << "ERROR: Invalid action!" << std::endl;
        return std::make_unique<RockSampleAction>(
                static_cast<RockSampleAction::Code>(code));
    }
}

} /* namespace rocksample */
