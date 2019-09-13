 /*
	- tablefillingalgorithm.cpp
	- Autor: Alberto Labrada Soto A01114148
	- Fecha: 04/09/2019
	- Version: 1.0
	- Proyecto 1: demostrar equivalencia entre dos AF
	- Matemáticas Computacionales AD19

	- Algoritmo:
		1. Dado el input(M1, M2) donde M1 y M2 son DFAs, NFAs y/o expresiones regulares:
		2. Convertir M1 y M2 a DFA. Si es necesario.
		3. Construir una matriz nxn donde n es la suma de los estados de M1 con los estados de M2.
			Solo se considerará el área de la diagonal inferior de la matriz
		4. Marcar cada celda de la tabla como DISTINGUIBLE si uno de los estados es aceptado y el otro no.
		5. Iterar hasta que ningún otro par de estados es marcado como distinguible
			(a) para cada par de estados {p,q} que no ha sido marcado distinguible aún:
				i. para cada simbolo en ai en el alfabeto si {p,q} no ha sido marcado distinguible:
					A. si el par de estados {d(p,ai), d(p,ai)} es distinguible entonces marcar {p,q} como distinguible
		6. Si los estados iniciales de los dos DFAs son marcados distinguibles entonces M1 y M2 NO son equivalentes
*/

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <utility>
#include <queue>
#include <vector>
#include <string>
using namespace std;

//struct Conjunto {
//	int length;
//	int* elementos;
//};

// struct state

class State {
	string _label;
	vector<int> _id;
public:
	State() : _label(""), _id(vector<int>()) {}; // estado vacío
	State(string label, vector<int> id) : _label(label), _id(id) {};
	State(const State& copyState) { this->_label = copyState._label; this->_id = copyState._id; }
	void setLabel(string label);
	void setID(vector<int> id);
	string getLabel();
	vector<int> getID();

	State operator + (const State&) const;
	bool isEmptyState()
	{
		return this->_id.front() == -1;
	}
};
// -- implementacion --
State State::operator + (const State& state2) const {
	State result;
	vector<int> states;
	int largest = this->_id.size() > state2._id.size() ? this->_id.size() : state2._id.size;
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
	string aux = "{" + states[0];
	for (int i = 1; i < states.size(); i++)
		aux += "," + states[i];
	aux += "}";
	result.setLabel(aux);
	result.setID(states);
	return result;
			

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
	
	Automata convertToDFA();
	Automata operator + (const Automata&) const;
	bool isIn(State, vector<State>);
	bool findFinalStates(vector<State>);
	bool pertenece(State, vector<State>);
	void addNewState(State, vector<State>);

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

Automata Automata::convertToDFA() {
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
	t_states.push_back(this->_states[i]); // insert q0 a la lista de estados
	t_cola.push(t_states[i]); // insert q0 a la cola
	State outputState; // estado(s) resultante(s) de delta
	State currentState; // estado actualmente en proceso tras ser sacado de la cola
	t_delta.resize(this->_delta.size());
	int sts; // states in state
	while (!t_cola.empty())
	{
		currentState = t_cola.front();
		t_cola.pop();
		t_delta.push_back(vector<State>()); // haces nueva fila
		
		//unico estado
		// evaluar simbolos
		for (j = 0; j < this->_inputs.size(); j++)
		{
			for (sts = 0; sts < currentState.getID().size(); sts++)
			{
				if (outputState.isEmptyState())
					outputState = this->_delta[currentState.getID()[sts]][j];
				else if (!this->_delta[currentState.getID()[sts]][j].isEmptyState())
					outputState = outputState + this->_delta[currentState.getID()[sts]][j];
			}
			if (!this->isIn(outputState, t_states))
			{
				// es nuevo estado
				// agregarlo a la lista
				t_states.push_back(outputState);
				// agregarlo a la cola
				t_cola.push(outputState);
				// si el estado (o algunos de los estados) de outputstate es final, outputstate es final
				if (this->isIn(outputState, this->_finalStates))
					t_finalStates.push_back(outputState);
			}
			// insertar en delta
			t_delta[i][j] = outputState;
		}
		i++;
	}
}

bool Automata::isIn(State checkState , vector<State> states) {
	for (State state : states)
	{
		if (checkState)
	}
}

bool Automata::pertenece(State checkState, vector<State> states) {
	for (State state : states) {
		if (checkState.getLabel().compare(state.getLabel()) == 0) {
			return false;
		}
	}
	return true;
}

void Automata::addNewState(State state, vector<State> states) {
	if (!this->pertenece(state, states)) {
		states.push_back(state);
	}
}

///----------------------------------------------------------------------------------------------------------------------

bool findStateWithID(vector<int> ids, int id)
{
	for (int i = 0; i < ids.size(); i++)
		if (ids[i] == id)
			return true;
	return false;
}

struct AutomataFinito {
	Conjunto estados;
	Conjunto simbolos;
	int** delta;
	int edoInicial;
	Conjunto edosFinales;
};

struct AFN {
	Conjunto estados;
	Conjunto simbolos;
	Conjunto** delta;
	int edoInicial;
	Conjunto edosFinales;
};

//Conjunto delta(Conjunto estados, int simbolo)
//{
//	for (int i = 0; i < estados.length; i++)
//	{
//		if (estados.elementos[i] == -1)
//	}
//}

//void fromNFAtoDFA(AFN af)
//{
//	vector<Conjunto> finalStates;
//	vector<Conjunto> states;
//	queue<Conjunto> cola;
//	Conjunto outputState;
//	// estado inicial no es conjunto, hay que convertirlo
//	//convert(int, conjuntode1);
//	Conjunto cEstadoInicial;
//	cEstadoInicial.length = 1;
//	cEstadoInicial.elementos = new int [1];
//	cEstadoInicial.elementos[0] = af.edoInicial;
//	cola.push(cEstadoInicial);
//	int c = 0;
//	while (!cola.empty())
//	{
//		Conjunto checkNewState;
//		Conjunto currentState = cola.front(); // {0} cEstadoInicial la primer vez
//		if (currentState.elementos[0] >= 0) // si no es el estado vacío
//		{
//			for (int estado = 0; estado < currentState.length; estado++)
//			{
//				for (int simbolo = 0; simbolo < af.simbolos.length; simbolo++)
//				{
//					outputState.length = af.delta[estado][simbolo].length;
//					outputState.elementos = new int [outputState.length];
//					for (int k = 0; k < outputState.length; k++)
//					{
//						outputState.elementos[k] = af.delta[estado][simbolo].elementos[k];
//					}
//					if (isNewState(outputState))
//					{
//						cola.push(outputState);
//						states.push_back(outputState);
//						if (checkForFinalStates(outputState))
//							finalStates.push_back(outputState);
//					}
//				}
//			}
//
//		}
//		
//		cola.pop();
//	}
//
//}

void printTable(bool** p, int n)
{
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

void printInequivalent(bool** p, int n, int iS, int jS)
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
				cout << setw(5) << '0';
		}
		diag++;
		cout << endl;
	}
}

//void findState(int* states, int n, int state)
//{
//	for (int i = 0; i < )
//}


//AutomataFinito unir(AutomataFinito afd1, AutomataFinito afd2)
//{
//	AutomataFinito afd;
//	// estados
//	afd.estados.length = afd1.estados.length + afd2.estados.length;
//	afd.estados.elementos = new int[afd.estados.length];
//	for (int i = 0; i < afd1.estados.length; i++)
//		afd.estados.elementos[i] = afd1.estados.elementos[i];
//	int desp = afd1.estados.length; // desplazamiento
//	for (int i = 0; i < afd2.estados.length; i++)
//		/*afd.estados.elementos[i + desp] = afd2.estados.elementos[i] + desp;*/
//	// delta
//	
//
//}

int edosIniciales[2]; // almacena el estado inicial del afd1 y el del afd2
bool isInitialState(int state)
{
	for (int i = 0; i < 2; i++)
		if (state == edosIniciales[i])
			return true;
	return false;
}

bool isFinalState(int state, AutomataFinito afd)
{
	for (int i = 0; i < afd.edosFinales.length; i++)
		if (state == afd.edosFinales.elementos[i]) return true;
	return false;
}

AFN fillNFA()
{
	Conjunto i_estados;
	Conjunto i_simbolos;
	Conjunto** i_delta;
	int i_edoInicial;
	Conjunto i_edosFinales;

	cout << "Ingresa el número de estados y el numero de simbolos: ";
	cin >> i_estados.length >> i_simbolos.length;
	cout << endl;

	i_estados.elementos = new int[i_estados.length];
	for (int i = 0; i < i_estados.length; i++) i_estados.elementos[i] = i;

	i_simbolos.elementos = new int[i_simbolos.length];
	for (int i = 0; i < i_simbolos.length; i++) i_simbolos.elementos[i] = i;

	char r;
	i_delta = new Conjunto* [i_estados.length];
	for (int i = 0; i < i_estados.length; i++)
	{
		
		i_delta[i] = new Conjunto[i_simbolos.length];
		for (int j = 0; j < i_simbolos.length; j++)
		{
			cout << "Estado " << i << " input " << j << endl;
			cout << "Escriba el numero de estados resultantes de |d(" << i << "," << j << ")|= ";
			cin >> r;
			int n = r - '0';
			if (n > 0){
				i_delta[i][j].length = n;
				i_delta[i][j].elementos = new int[n];
				cout << "Escriba separado por espacios los estados output de " <<
					"|d(" << i << ", " << j << ") |= ";
				for (int k = 0; k < r - '0'; k++)
				{
					cin >> i_delta[i][j].elementos[k];
				}
			}
			else
			{
				// conjunto vacío
				i_delta[i][j].length = 1;
				i_delta[i][j].elementos = new int[1];
				i_delta[i][j].elementos[0] = -1;
			}

		}	
	}

	cout << "Cual es el estado inicial? ";
	cin >> i_edoInicial;
	cout << endl;

	cout << "Cuantos estados finales hay? ";
	cin >> i_edosFinales.length;
	i_edosFinales.elementos = new int[i_edosFinales.length];
	cout << "Cuales son los " << i_edosFinales.length << " estados ? ";
	for (int i = 0; i < i_edosFinales.length; i++)
		cin >> i_edosFinales.elementos[i];
	return { i_estados, i_simbolos, i_delta, i_edoInicial, i_edosFinales };
}

Automata fillAutomata()
{
	vector<State> i_states;
	vector<State> i_inputs;
	vector<vector<State>> i_delta;
	State i_initialState;
	vector<State> i_finalStates;

	bool isNFA = false;

	int numStates, numInputs;
	cout << "Ingresa numero de estados y numero de simbolos del alfabeto: ";
	cin >> numStates >> numInputs;
	cout << endl;

	for (int i = 0; i < numStates; i++)
	{
		State st(to_string(i), vector<int>());
		st.getID().push_back(i);
		i_states.push_back(st);
	}

	// ---- revisar ----
	cout << "Escriba los simbolos: ";
	for (int i = 0; i < numInputs; i++)
	{
		string input;
		cin >> input;
		State st;
		st.setLabel(input);
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
	bool notified = false;
	int idNew = i_states.size();
	vector<int> newStates;
	int statesInDelta; // se preguntara cuantos estados hay en cada casilla
	vector<int> estados; // facilitador de num de estados
	for (int i = 0; i < numStates; i++)
	{
		for (int j = 0; j < numInputs; j++)
		{
			cout << "Cuantos estados resultantes tiene delta en el estado" + i_states[i].getLabel() + ", con input " +
				i_inputs[j].getLabel() + ": ";
			cin >> statesInDelta;
			if (statesInDelta > 1 && !notified) isNFA = true;
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
			string aux = "{" + states[0];
			for (int k = 1; k < statesInDelta; k++)
				aux += "," + states[k];
			aux += "}";
			i_delta[i][j] = State(aux, states);
		}
	}
	
	cout << "Cual es el estado inicial? ";
	cin >> num;
	cout << endl;

	State st = State(to_string(num), vector<int>());
	st.getID().push_back(num);

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

void tableFillingAlgorithm(AutomataFinito afd1, AutomataFinito afd2)
{
	//if (afd1.simbolos.length != afd2.simbolos.length) return; // no tienen el mismo alfabeto
	AutomataFinito afdM = afd1;
	edosIniciales[0] = 0;
	edosIniciales[1] = 4;
	// join two AFD


	// construir matriz nxn, solo la diagonal inferior
	bool** matrix = new bool* [afdM.estados.length];
	int estado_i,
		estado_j,
		diag;
	bool cambios,
		noted,
		equivalente;

	//preparacion
	diag = 1;
	for (estado_i = 1; estado_i < afdM.estados.length; estado_i++)
	{
		matrix[estado_i] = new bool[diag];
		for (estado_j = 0; estado_j < diag; estado_j++)
			matrix[estado_i][estado_j] = 0;
		diag++;
	}
	printTable(matrix, afdM.estados.length);
	cout << endl;
	// initialTable
	diag = 1;
	for (estado_i = 1; estado_i < afdM.estados.length; estado_i++)
	{
		for (estado_j = 0; estado_j < diag; estado_j++)
		{
			if (isFinalState(estado_i, afdM) != isFinalState(estado_j, afdM))
				matrix[estado_i][estado_j] = !matrix[estado_i][estado_j];
		}
		diag++;
	}
	printTable(matrix, afdM.estados.length);
	cout << endl;

	// algorithm
	cambios = true;
	noted = false;
	equivalente = true; // son equivalentes hasta demostrar lo contrario
	while (cambios)
	{
		diag = 1;
		for (estado_i = 1; estado_i < afdM.estados.length; estado_i++)
		{
			for (estado_j = 0; estado_j < diag; estado_j++)
			{

				if (!matrix[estado_i][estado_j])
				{
					bool isDistinguishable = false;
					for (int k = 0; k < afdM.simbolos.length; k++)
					{
						int nuevo_i = afdM.delta[estado_i][k];
						int nuevo_j = afdM.delta[estado_j][k];

						if (isFinalState(nuevo_i, afdM) || isFinalState(nuevo_j, afdM)) 
							isDistinguishable = true;
						else if (nuevo_i > nuevo_j) 
						{
							if (matrix[nuevo_i][nuevo_j]) 
								isDistinguishable = true;
						}
						else if (nuevo_i < nuevo_j)
						{
							if (matrix[nuevo_j][nuevo_i]) 
								isDistinguishable = true;
						}

						if (isDistinguishable)
						{
							matrix[estado_i][estado_j] = !matrix[estado_i][estado_j];
							if (!noted) noted = true;
							if (isInitialState(estado_i) && isInitialState(estado_j))
							{
								equivalente = false;
								cout << "NO SON EQUIVALENTES: " << endl;
								printInequivalent(matrix, afdM.estados.length, estado_i, estado_j);
								return;
							}
							break;
						}
					}
				}
			}
			diag++;
		}
		if (!noted || !equivalente) cambios = false;
		printTable(matrix, afdM.estados.length);
		cout << endl;
	}
}

int main() 
{
	Automata aut1 = fillAutomata();
	if (aut1.isNFA()) aut1.convertToDFA();
	Automata aut2 = fillAutomata();
	if (aut2.isNFA()) aut2.convertToDFA();

	return 0;
}
