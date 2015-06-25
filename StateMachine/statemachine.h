#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QList>
#include "../Logic/logic_rule.h"
#include "machinestate.h"
#include "role.h"
#include "move.h"

class StateMachine
{
public:
    StateMachine();

    virtual void initialize(QList<LRelation> relations, QList<LRule> rules) = 0;
    virtual void initialize(QString filename) = 0;


    virtual int getGoal(const MachineState& state, Role role) = 0;
    virtual bool isTerminal(const MachineState& state) = 0;
    virtual QVector<Role> getRoles() = 0;
    virtual MachineState getInitialState() = 0;
    virtual QList<Move> getLegalMoves(const MachineState& state, Role role) = 0;
    virtual MachineState getNextState(const MachineState& state, QList<Move> moves) = 0;


    virtual Role getRoleFromString(QString s) = 0;
    virtual Move getMoveFromString(QString s) = 0;
    virtual QList<Move> getMovesFromString(QString s) = 0;

    /**
        public List<List<Move>> getLegalJointMoves(MachineState state) throws MoveDefinitionException
        {
            List<List<Move>> legals = new ArrayList<List<Move>>();
            for (Role role : getRoles()) {
                legals.add(getLegalMoves(state, role));
            }

            List<List<Move>> crossProduct = new ArrayList<List<Move>>();
            crossProductLegalMoves(legals, crossProduct, new LinkedList<Move>());

            return crossProduct;
        }


        public List<List<Move>> getLegalJointMoves(MachineState state, Role role, Move move) throws MoveDefinitionException
        {
            List<List<Move>> legals = new ArrayList<List<Move>>();
            for (Role r : getRoles()) {
                if (r.equals(role)) {
                    List<Move> m = new ArrayList<Move>();
                    m.add(move);
                    legals.add(m);
                } else {
                    legals.add(getLegalMoves(state, r));
                }
            }

            List<List<Move>> crossProduct = new ArrayList<List<Move>>();
            crossProductLegalMoves(legals, crossProduct, new LinkedList<Move>());

            return crossProduct;
        }

        protected void crossProductLegalMoves(List<List<Move>> legals, List<List<Move>> crossProduct, LinkedList<Move> partial)
        {
            if (partial.size() == legals.size()) {
                crossProduct.add(new ArrayList<Move>(partial));
            } else {
                for (Move move : legals.get(partial.size())) {
                    partial.addLast(move);
                    crossProductLegalMoves(legals, crossProduct, partial);
                    partial.removeLast();
                }
            }
        }

        private Map<Role,Integer> roleIndices = null;

        public Map<Role, Integer> getRoleIndices()
        {
            if (roleIndices == null) {
                ImmutableMap.Builder<Role, Integer> roleIndicesBuilder = ImmutableMap.builder();
                List<Role> roles = getRoles();
                for (int i = 0; i < roles.size(); i++) {
                    roleIndicesBuilder.put(roles.get(i), i);
                }
                roleIndices = roleIndicesBuilder.build();
            }

            return roleIndices;
        }


        public List<Integer> getGoals(MachineState state) throws GoalDefinitionException {
            List<Integer> theGoals = new ArrayList<Integer>();
            for (Role r : getRoles()) {
                theGoals.add(getGoal(state, r));
            }
            return theGoals;
        }


        public List<Move> getRandomJointMove(MachineState state) throws MoveDefinitionException
        {
            List<Move> random = new ArrayList<Move>();
            for (Role role : getRoles()) {
                random.add(getRandomMove(state, role));
            }

            return random;
        }


        public List<Move> getRandomJointMove(MachineState state, Role role, Move move) throws MoveDefinitionException
        {
            List<Move> random = new ArrayList<Move>();
            for (Role r : getRoles()) {
                if (r.equals(role)) {
                    random.add(move);
                } else {
                    random.add(getRandomMove(state, r));
                }
            }

            return random;
        }


        public Move getRandomMove(MachineState state, Role role) throws MoveDefinitionException
        {
            List<Move> legals = getLegalMoves(state, role);
            return legals.get(new Random().nextInt(legals.size()));
        }


        public MachineState getRandomNextState(MachineState state) throws MoveDefinitionException, TransitionDefinitionException
        {
            List<Move> random = getRandomJointMove(state);
            return getNextState(state, random);
        }


        public MachineState getRandomNextState(MachineState state, Role role, Move move) throws MoveDefinitionException, TransitionDefinitionException
        {
            List<Move> random = getRandomJointMove(state, role, move);
            return getNextState(state, random);
        }


        public MachineState performDepthCharge(MachineState state, final int[] theDepth) throws TransitionDefinitionException, MoveDefinitionException {
            int nDepth = 0;
            while(!isTerminal(state)) {
                nDepth++;
                state = getNextStateDestructively(state, getRandomJointMove(state));
            }
            if(theDepth != null)
                theDepth[0] = nDepth;
            return state;
        }


    **/

};

#endif // STATEMACHINE_H
