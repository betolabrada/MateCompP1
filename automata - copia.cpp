 /*
	- tablefillingalgorithm.cpp
	- Autor: Alberto Labrada Soto A01114148
	- Fecha: 04/09/2019
	- Version: 1.0
	- Proyecto 1: demostrar equivalencia entre dos AF
	- Matemáticas Computacionales AD19

*/

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <utility>
#include <queue>
#include <vector>
#include <string>
using namespace std;

class State {
	string _label;
	vector<int> _id;
	string _FDAlabel;
	int _FDAID;
public:
	State() : _label(""), _id(vector<int>()) {}; // estado vacío
	State(string label, vector<int> id) : _label(label), _id(id) {};
	State(const State& copyState) { this->_label = copyState._label; this->_id = copyState._id; }
	void setLabel(string label) { this->_label = label; };
	void setID(vector<int> id) { this->_id = id; };
	void setFDAlabel(string label) { _FDAlabel = label; };
	void setFDAID(int id) { _FDAID = id; };
	string getLabel() { return this->_label; };
	vector<int> getID() const { return this->_id; };
	string getFDAlabel() { return this->_FDAlabel; };
	int getFDAID() { return _FDAID; };
	

	bool isEmptyState() const { return this->_id.front() == -1; };
	State operator + (const State&) const;
	// operator += for later
	void shift(int);
};
// -- implementacion --

State State::operator + (const State& state2) const {
	if (this->isEmptyState()) return state2;
	if (state2.isEmptyState()) return *this;
	State result;
	vector<int> states;
	int largest = this->_id.size() > state2._id.size() ? this->_id.size() : state2._id.size();
	for (int i = 0; i < this->_id.size(); i++)
	{
		states.push_back(this->_id[i]);
	}
	bool isAlready;
	for (int i = 0; i < state2._id.size(); i++)
	{
		isAlready = false;
		for (int j = 0; j < states.size(); j++)
			if (state2._id[i] == states[j])
			{
				isAlready = true;
				break;
			}
		if (!isAlready)
			states.push_back(state2._id[i]);
	}
	sort(states.begin(), states.end());
	string aux = "{" + to_string(states[0]);
	for (int i = 1; i < states.size(); i++)
		aux += "," + to_string(states[i]);
	aux += "}";
	result.setLabel(aux);
	result.setID(states);
	return result;
}

void State::shift(int n)
{
	string saux;
	for (int i = 0; i < this->_id.size(); i++)
	{
		this->_id[i] += n;
	}
	saux = "" + to_string(this->_id[0]);
	for (int j = 1; j < this->_id.size(); j++)
		saux += "," + to_string(this->_id[j]);

	this->_label = saux;
}

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

bool Automata::isIn(State checkState , vector<State> states) {
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

Automata Automata::operator + (const Automata& a2) const
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

///----------------------------------------------------------------------------------------------------------------------

void printTable(vector<vector<bool>> p, int n)
{
	cout << endl;
	int diag = 1;

	for (int i = 1; i < n; i++)
	{
		for (int j = 0; j < diag; j++)
		{
			if (p[i][j])
				cout << setw(5) << 1;
			else
				cout << setw(5) << 0;
		}
		diag++;
		cout << endl;
	}
}

void printAtCell(vector<vector<bool>> p, int n, int iS, int jS)
{
	int diag = 1;

	for (int i = 1; i < n; i++)
	{
		for (int j = 0; j < diag; j++)
		{
			
			if (p[i][j])
			{
				if (i == iS && j == jS) cout << setw(5) << "->1";
				else cout << setw(5) << '1';
			}
			else
			{
				if (i == iS && j == jS) cout << setw(5) << "->0";
				else cout << setw(5) << '0';
			}
		}
		diag++;
		cout << endl;
	}
}

Automata fillAutomata()
{
	char r;
	cout << "Escriba 'N' si su automata finito es no determinista, 'D' si es determinista (N/D): ";
	cin >> r;
	cout << endl;
	vector<State> i_states;
	vector<State> i_inputs;
	vector<vector<State>> i_delta;
	State i_initialState;
	vector<State> i_finalStates;

	bool isNFA = r == 'N';

	int numStates, numInputs;
	cout << "Ingresa numero de estados y numero de simbolos del alfabeto: ";
	cin >> numStates >> numInputs;
	cout << endl;

	State st;
	vector<int> nums;
	nums.resize(1);
	for (int i = 0; i < numStates; i++)
	{
		nums[0] = i;
		st = State(to_string(i), nums);
		i_states.push_back(st);
	}

	// ---- revisar ----
	cout << "Escriba los simbolos: ";
	for (int i = 0; i < numInputs; i++)
	{
		string input;
		cin >> input;
		nums[0] = i;
		st = State(input, nums);
		i_inputs.push_back(st);
	}

	// construir delta
	for (int i = 0; i < i_states.size(); i++)
	{
		vector<State> fila = vector<State>();
		for (int j = 0; j < i_inputs.size(); j++)
		{
			State col = State();
			fila.push_back(col);
		}
		i_delta.push_back(fila);
	}

	// llenar delta
	int num; // los numeros dados por el usuario
	vector<int> states; // aqui se acomodaran los estados;
	int idNew = i_states.size();
	vector<int> newStates;
	int statesInDelta; // se preguntara cuantos estados hay en cada casilla
	vector<int> estados; // facilitador de num de estados
	for (int i = 0; i < numStates; i++)
	{
		for (int j = 0; j < numInputs; j++)
		{
			if (r == 'N')
			{
				cout << "Cuantos estados resultantes tiene delta en el estado" + i_states[i].getLabel() + ", con input " +
					i_inputs[j].getLabel() + ": ";
				cin >> statesInDelta;
			}
			else statesInDelta = 1;
			cout << endl;
			cout << "Escriba estado(s) en delta(" + i_states[i].getLabel() + ", " +
				i_inputs[j].getLabel() + "): ";
			states.resize(statesInDelta);
			for (int k = 0; k < statesInDelta; k++)
			{
				cin >> num;
				states[k] = num;
			}
			sort(states.begin(), states.end());
			string aux = "{" + to_string(states[0]);
			for (int k = 1; k < statesInDelta; k++)
				aux += "," + to_string(states[k]);
			aux += "}";
			i_delta[i][j] = State(aux, states);
		}
	}
	
	cout << "Cual es el estado inicial? ";
	cin >> num;
	cout << endl;

	st.setID(vector<int>(1,num));
	i_initialState = st;

	cout << "Cuantos estados finales hay? ";
	cin >> num;
	cout << "Cuales ?: ";
	int k;
	i_finalStates.resize(num);
	for (int i = 0; i < num; i++)
	{
		cin >> k;
		st.setLabel(to_string(k));
		st.setID(vector<int>(1, k));
		i_finalStates[i] = st;
	}

	Automata aut = Automata(i_states, i_inputs, i_delta, i_initialState, i_finalStates);
	if (isNFA) aut.toggleNFA();
	return aut;
}
/*
	-Algoritmo:
	1. Dado el input(M1, M2) donde M1 y M2 son DFAs, NFAs y / o expresiones regulares :
	2. Convertir M1 y M2 a DFA.Si es necesario.
	3. Construir una matriz nxn donde n es la suma de los estados de M1 con los estados de M2.
	Solo se considerará el área de la diagonal inferior de la matriz
	4. Marcar cada celda de la tabla como DISTINGUIBLE si uno de los estados es aceptado y el otro no.
	5. Iterar hasta que ningún otro par de estados es marcado como distinguible
	(a) para cada par de estados {
		p, q
	} que no ha sido marcado distinguible aún :
	i.para cada simbolo en ai en el alfabeto si{ p,q } no ha sido marcado distinguible :
	A.si el par de estados{ d(p,ai), d(p,ai) } es distinguible entonces marcar{ p,q } como distinguible
	6. Si los estados iniciales de los dos DFAs son marcados distinguibles entonces M1 y M2 NO son equivalentes
*/

bool isInitialState(State initialState, int n)
{
	for (int state : initialState.getID())
	{
		if (n == state) return true;
	}
	return false;
}

bool isDistinguishable(Automata afdM, int estado_i, int estado_j, vector<vector<bool>> matrix)
{
	for (int k = 0; k < afdM.getInputs().size(); k++)
	{
		State nuevo_i = afdM.getDelta()[estado_i][k];
		State nuevo_j = afdM.getDelta()[estado_j][k];

		int nuevo_iID = nuevo_i.getID().front();
		int nuevo_jID = nuevo_j.getID().front();
		
		if (nuevo_iID > nuevo_jID)
		{
			if (matrix[nuevo_iID][nuevo_jID])
				return true;
		}
		else if (nuevo_iID < nuevo_jID)
		{
			if (matrix[nuevo_jID][nuevo_iID])
				return true;
		}
	}
	return false;

}


void tableFillingAlgorithm(Automata afd1, Automata afd2)
{
	Automata afdM = afd1 + afd2;

	
	// construir matriz nxn, solo la diagonal inferior
	int n = afdM.getStates().size();
	vector<vector<bool>> matrix;
	for (int i = 0; i < n; i++)
		matrix.push_back(vector<bool>(i, false));

	int estado_i,
		estado_j,
		diag;
	bool cambios,
		hayCambiosAviso,
		equivalente;

	
	// initialTable
	diag = 1;
	for (estado_i = 1; estado_i < afdM.getStates().size(); estado_i++)
	{
		for (estado_j = 0; estado_j < diag; estado_j++)
		{
			if (afdM.isIn(afdM.getStates()[estado_i], afdM.getfinalStates()) !=
				afdM.isIn(afdM.getStates()[estado_j], afdM.getfinalStates()))
			{
				matrix[estado_i][estado_j] = !matrix[estado_i][estado_j];
				if (isInitialState(afdM.getInitialState(), estado_i) && isInitialState(afdM.getInitialState(),
					estado_j))
				{
					equivalente = false;
					cout << "NO SON EQUIVALENTES " << endl;
					//printAtCell(matrix, n, estado_i, estado_j);
					cout << endl;
					return; // ya podemos terminar
				}
			}
		}
		diag++;
	}
	printTable(matrix, n);
	cout << endl;

	// algorithm
	cambios = true; // bool mientras siga habiendo cambios
	hayCambiosAviso = false; // bool que avisa si hubo algun cambio
	equivalente = true; // son equivalentes hasta demostrar lo contrario
	int cont = 0;
	while (cambios)
	{
		cont++;
		
		diag = 1;
		for (estado_i = 1; estado_i < afdM.getStates().size(); estado_i++)
		{
			for (estado_j = 0; estado_j < diag; estado_j++)
			{
				if (!matrix[estado_i][estado_j])
				{
					bool distinct = isDistinguishable(afdM, estado_i, estado_j, matrix);
					
					if (distinct)
					{
						matrix[estado_i][estado_j] = !matrix[estado_i][estado_j];
						if (isInitialState(afdM.getInitialState(), estado_i) && isInitialState(afdM.getInitialState(),
							estado_j))
						{
							equivalente = false;
							cout << "NO SON EQUIVALENTES " << endl;
							printAtCell(matrix, n, estado_i, estado_j);
							cout << endl;
							return; // ya podemos terminar
						}
						hayCambiosAviso = true;
					}	
				}
			}
			diag++;
		}

		if (!hayCambiosAviso) cambios = false;
		else {
			hayCambiosAviso = false;
		}
	}
	cout << "SON EQUIVALENTES" << endl;
	printAtCell(matrix, n, afdM.getInitialState().getID().back(), afdM.getInitialState().getID().front());
	cout << endl;
}



int main() 
{
	
	Automata aut1, aut2;

		aut1 = fillAutomata();
		if (aut1.isNFA()) aut1.convertToDFA();
		aut2 = fillAutomata();
		if (aut2.isNFA()) aut2.convertToDFA();
		aut2.shift(aut1.getStates().size());
		tableFillingAlgorithm(aut1, aut2);
		cout << "--------------------------------------------" << endl;
	/*while (1) 
	{
	}*/

	return 0;
}
