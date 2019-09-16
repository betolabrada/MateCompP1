#include "automata.h"
#include <iostream>
#include <queue>

//	--	implementation	--
Automata::Automata() {
	this->_states = vector<State>();
	this->_inputs = vector<State>();
	this->_delta = vector<vector<State>>();
	this->_initialState = State();
	this->_finalStates = vector<State>();
}

Automata::Automata(const vector<State>& states, const vector<State>& inputs,
	const vector<vector<State>>& delta, const State initialState,
	const vector<State> finalStates) : _states(states), _inputs(inputs), _delta(delta),
	_initialState(initialState), _finalStates(finalStates) {}

void Automata::convertToDFA() {
	// construir un AFD con todas las caracteristicas del objeto automata
	// states, inputs, delta, initialState, finalState
	// delta cambia cada vector dentro de 2d array es de 1 solo valor (pair)
	// algorithm: 
	// -1. introducir estado inicial en queue
	// -2. mientras no se haya vaciado la cola:
	//		3. sacar de la cola estado
	//		4. evaluar delta en todos los inputs
	//		5. si se encuentra nuevo estado
	//			6. agregarlo a vector de estados
	//			7. agregarlo a la cola
	//			8. si alguno de esos estados es un estado final, agregar a vector de estados finales

	// temporal variables
	queue<State> t_cola;
	vector<State> t_states;
	vector<State> t_inputs;
	vector<vector<State>> t_delta;
	State t_initialState;
	vector<State> t_finalStates;

	// implementation
	int i = 0,
		j,
		k; // iterate through delta
	int id = 0;
	t_states.push_back(this->_states[i]); // insert q0 a la lista de estados
	t_cola.push(t_states[i]); // insert q0 a la cola
	State outputState; // estado(s) resultante(s) de delta
	State currentState; // estado actualmente en proceso tras ser sacado de la cola
	int sts; // states in state
	while (!t_cola.empty())
	{
		currentState = t_cola.front();
		t_cola.pop();
		t_delta.push_back(vector<State>()); // haces nueva fila
		if (currentState.isEmptyState())
		{
			for (j = 0; j < this->_inputs.size(); j++)
			{
				t_delta[i].push_back(currentState);
			}
			i++;
			continue;
		}
		//unico estado
		// evaluar simbolos
		for (j = 0; j < this->_inputs.size(); j++)
		{
			outputState = this->_delta[currentState.getID()[0]][j];
			for (sts = 1; sts < currentState.getID().size(); sts++)
			{
				outputState = outputState + this->_delta[currentState.getID()[sts]][j];
			}
			t_delta[i].push_back(outputState);
			if (!this->isIn(outputState, t_states))
			{
				// es nuevo estado
				// agregarlo a la lista
				t_states.push_back(outputState);
				// agregarlo a la cola
				t_cola.push(outputState);
				// si el estado (o algunos de los estados) de outputstate es final, outputstate es final
				/*for (k = 0; k < outputState.getID().size(); k++)
				{
					this->isIn(outputState.getID()[k], this->_finalStates)
				}*/
				if (this->isIn(outputState, this->_finalStates))
					t_finalStates.push_back(outputState);
			}
			// insertar en delta
		}
		i++;
	}
	//string aux = "";
	//for (int id = 0; id < t_states.size(); id++)
	//{
	//	t_states[id].setFDAID(id);
	//	aux += "{" + to_string(id) + "}";
	//	t_states[id].setFDAlabel(aux);
	//	aux.clear();

	//}

	this->_states = t_states;
	this->_delta = t_delta;
	this->_finalStates = t_finalStates;
}

bool Automata::isIn(State checkState, vector<State> states) {
	bool is = false;
	if (checkState.getID().size() > 1)
	{
		vector<int> statesInCheckState = checkState.getID();
		for (int i = 0; i < statesInCheckState.size(); i++)
		{
			for (int j = 0; j < states.size(); j++)
			{
				vector<int> statesInState = states[j].getID();
				for (int k = 0; k < statesInState.size(); k++)
				{
					if (statesInCheckState[i] == statesInState[j])
						return true;
				}
			}
		}
	}
	else
	{
		vector<int> checkStates = checkState.getID();
		for (int i = 0; i < states.size(); i++)
		{
			if (checkStates.size() == states[i].getID().size())
			{
				is = true;
				for (int j = 0; j < checkStates.size(); j++)
					if (checkStates[j] != states[i].getID()[j])
						is = false;
				if (is) return true;
			}
		}

	}
	return false;
}

void Automata::shift(int n)
{
	// states
	for (int i = 0; i < this->_states.size(); i++)
	{
		this->_states[i].shift(n);
	}


	for (int i = 0; i < this->_delta.size(); i++)
	{
		for (int j = 0; j < this->_delta[i].size(); j++)
		{
			this->_delta[i][j].shift(n);
		}
	}

	this->_initialState.shift(n);

	for (int i = 0; i < this->_finalStates.size(); i++)
	{
		this->_finalStates[i].shift(n);

	}

	cout << "Automata is shifted!" << endl;

}

Automata Automata::operator + (const Automata & a2) const
{

	vector<State> t_states;
	vector<State> t_inputs = this->_inputs;
	vector<vector<State>> t_delta;
	State t_initialStates; // ahora son dos pues es la union de dos automatas;
	vector<State> t_finalStates;

	for (int i = 0; i < this->_states.size(); i++)
		t_states.push_back(this->_states[i]);
	for (int i = 0; i < a2._states.size(); i++)
		t_states.push_back(a2._states[i]);


	// delta
	// construir delta
	for (int i = 0; i < t_states.size(); i++)
	{
		vector<State> fila = vector<State>();
		for (int j = 0; j < t_inputs.size(); j++)
		{
			State col = State();
			fila.push_back(col);
		}
		t_delta.push_back(fila);
	}
	for (int i = 0; i < this->_states.size(); i++)
		for (int j = 0; j < t_inputs.size(); j++)
			t_delta[i][j] = this->_delta[i][j];
	for (int i = this->_states.size(); i < a2._states.size() + this->_states.size(); i++)
		for (int j = 0; j < t_inputs.size(); j++)
			t_delta[i][j] = a2._delta[i - this->_states.size()][j];

	// initialState
	vector<int> nums;
	nums.push_back(this->_initialState.getID().front());
	nums.push_back(a2.getInitialState().getID().front());
	string aux = to_string(nums.front()) + "," + to_string(nums.back());
	t_initialStates = State(aux, nums);


	// finalStates
	for (int i = 0; i < this->_finalStates.size(); i++)
		t_finalStates.push_back(this->_finalStates[i]);
	for (int i = 0; i < a2._finalStates.size(); i++)
		t_finalStates.push_back(a2._finalStates[i]);

	Automata M = Automata(t_states, t_inputs, t_delta, t_initialStates, t_finalStates);
	return M;
}
