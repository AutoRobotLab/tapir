#ifndef TAGMODEL_HPP_
#define TAGMODEL_HPP_

#include <memory>                       // for unique_ptr
#include <ostream>                      // for ostream
#include <string>                       // for string
#include <utility>                      // for pair
#include <vector>                       // for vector

#include <boost/program_options.hpp>    // for variables_map

#include "global.hpp"                     // for RandomGenerator
#include "problems/shared/GridPosition.hpp"  // for GridPosition
#include "problems/shared/ModelWithProgramOptions.hpp"  // for ModelWithProgramOptions

#include "solver/changes/ChangeFlags.hpp"        // for ChangeFlags
#include "solver/abstract-problem/Model.hpp"             // for Model::StepResult, Model
#include "solver/abstract-problem/ModelChange.hpp"             // for ModelChange
#include "solver/abstract-problem/TransitionParameters.hpp"
#include "solver/abstract-problem/Action.hpp"            // for Action
#include "solver/abstract-problem/Observation.hpp"       // for Observation
#include "solver/abstract-problem/State.hpp"

#include "solver/mappings/enumerated_actions.hpp"
#include "solver/mappings/discrete_observations.hpp"

#include "TagAction.hpp"

namespace po = boost::program_options;

namespace solver {
class StatePool;
} /* namespace solver */

namespace tag {
class TagObervation;
class TagState;

/** Represents a change in the Tag model. */
struct TagChange : solver::ModelChange {
    std::string changeType = "";
    double i0 = 0;
    double i1 = 0;
    double j0 = 0;
    double j1 = 0;
};

class TagModel: virtual public ModelWithProgramOptions,
        virtual public solver::ModelWithEnumeratedActions,
        virtual public solver::ModelWithDiscreteObservations {
    friend class TagObservation;

  public:
    TagModel(RandomGenerator *randGen, po::variables_map vm);
    ~TagModel() = default;
    TagModel(TagModel const &) = delete;
    TagModel(TagModel &&) = delete;
    TagModel &operator=(TagModel const &) = delete;
    TagModel &operator=(TagModel &&) = delete;

    /** The cells are either empty or walls; empty cells are numbered
     * starting at 0
     */
    enum TagCellType : int {
        EMPTY = 0,
        WALL = -1
    };

    std::string getName() override {
        return "Tag";
    }

    /***** Start implementation of Model's virtual methods *****/
    // Simple getters
    long getNumberOfStateVariables() override {
        return nStVars_;
    }
    double getMinVal() override {
        return minVal_;
    }
    double getMaxVal() override {
        return maxVal_;
    }
    double getDefaultVal() override {
        return 0;
    }

    // Other virtual methods
    std::unique_ptr<solver::State> sampleAnInitState() override;
    /** Generates an untagged state uniformly at random. */
    std::unique_ptr<solver::State> sampleStateUniform() override;

    bool isTerminal(solver::State const &state) override;
    double getHeuristicValue(solver::State const &state) override;

    /* --------------- Black box dynamics ----------------- */
    virtual std::unique_ptr<solver::State> generateNextState(
            solver::State const &state,
            solver::Action const &action,
            solver::TransitionParameters const */*tp*/) override;

    virtual std::unique_ptr<solver::Observation> generateObservation(
            solver::State const */*state*/,
            solver::Action const &action,
            solver::TransitionParameters const */*tp*/,
            solver::State const &nextState) override;

    virtual double generateReward(
                solver::State const &state,
                solver::Action const &action,
                solver::TransitionParameters const */*tp*/,
                solver::State const */*nextState*/) override;

    virtual Model::StepResult generateStep(solver::State const &state,
            solver::Action const &action) override;


    std::vector<std::unique_ptr<solver::State>> generateParticles(
            solver::BeliefNode *previousBelief,
            solver::Action const &action,
            solver::Observation const &obs,
            std::vector<solver::State const *>
                const &previousParticles) override;
    std::vector<std::unique_ptr<solver::State>> generateParticles(
            solver::BeliefNode *previousBelief,
            solver::Action const &action,
            solver::Observation const &obs) override;

    virtual void applyChange(solver::ModelChange const &change, solver::StatePool *pool) override;

    /** Displays a single cell of the map. */
    void dispCell(TagCellType cellType, std::ostream &os);
    void drawEnv(std::ostream &os) override;
    void drawSimulationState(solver::BeliefNode *belief,
            solver::State const &state,
            std::ostream &os) override;

    virtual std::vector<std::unique_ptr<solver::DiscretizedPoint>> getAllActionsInOrder();

  private:
    /** Initialises the required data structures and variables */
    void initialize();

    /** Generates a random empty grid cell. */
    GridPosition randomEmptyCell();

    /**
     * Generates a next state for the given state and action;
     * returns true if the action was legal, and false if it was illegal.
     */
    std::pair<std::unique_ptr<TagState>, bool> makeNextState(
            solver::State const &state, solver::Action const &action);
    /** Generates an observation given a next state (i.e. after the action)
     * and an action.
     */
    std::unique_ptr<solver::Observation> makeObservation(
            solver::Action const &action, TagState const &nextState);
    /** Moves the opponent. */
    GridPosition getMovedOpponentPos(GridPosition const &robotPos,
            GridPosition const &opponentPos);
    /** Generates the distribution for the opponent's actions. */
    std::vector<ActionType> makeOpponentActions(GridPosition const &robotPos,
            GridPosition const &opponentPos);

    /** Gets the expected coordinates after taking the given action;
     *  this may result in invalid coordinates.
     */
    GridPosition getMovedPos(GridPosition const &position, ActionType action);
    /** Returns true iff the given GridPosition form a valid position. */
    bool isValid(GridPosition const &pos);

    /** The penalty for each movement. */
    double moveCost_;
    /** The reward for taggint the opponent. */
    double tagReward_;
    /** The penalty for failing a tag attempt. */
    double failedTagPenalty_;
    /** The probability that the opponent will stay still. */
    double opponentStayProbability_;

    /** The number of rows in the map. */
    long nRows_;
    /** The number of columns in the map. */
    long nCols_;

    /** The environment map in text form. */
    std::vector<std::string> mapText_;
    /** The environment map in vector form. */
    std::vector<std::vector<TagCellType>> envMap_;

    // General problem parameters
    long nActions_, nStVars_;
    double minVal_, maxVal_;
};
} /* namespace tag */

#endif /* TAGMODEL_HPP_ */
