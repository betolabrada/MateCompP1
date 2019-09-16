#include <vector>
#include "state.h"
using namespace std;

class Automata
{
	vector<State> _states;
	vector<State> _inputs;
	vector<vector<State>> _delta;
	State _initialState;
	vector<State> _finalStates;

	bool _isNFA = false;
public:
	Automata();
	Automata(const vector<State>& states, const vector<State>& inputs,
		const vector<vector<State>>& delta, const State initialState,
		const vector<State> finalStates);
	void setStates(const vector<State>& states) { _states = states; };
	void setInputs(const vector<State>& inputs) { _inputs = inputs; };
	void setDelta(const vector<vector<State>>& delta) { _delta = delta; };
	void setInitialState(const State& initialState) { _initialState = initialState; };
	void setFinalStates(const vector<State>& finalStates) { _finalStates = finalStates; };
	void toggleNFA() { _isNFA = !_isNFA; };
	vector<State> getStates() const { return _states; };
	vector<State> getInputs() const { return _inputs; };
	vector<vector<State>> getDelta() const { return _delta; };
	State getInitialState() const { return _initialState; };
	vector<State> getfinalStates() const { return _finalStates; };
	bool isNFA() { return _isNFA; };

	void convertToDFA();
	Automata operator + (const Automata&) const;
	bool isIn(State, vector<State>);
	void shift(int);

};