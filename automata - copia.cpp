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

struct Conjunto {
	int length;
	int* elementos;
};

// struct state

struct State {
	string label;
	int id;
};


class Automata
{
	vector<struct State> _states;
	vector<struct State> _inputs;
	vector<vector<vector<struct State>>> _delta;
	struct State _initialState;
	vector<struct State> _finalStates;
public:
	Automata(const vector<struct State>& states, const vector<struct State>& inputs,
		const vector<vector<vector<struct State>>>& delta, const struct State initialState, 
		const vector<struct State> finalStates);
	void setStates(const vector<struct State>& states) { _states = states; };
	void setInputs(const vector<struct State>& inputs) { _inputs = inputs; };
	void setDelta(const vector<vector<vector<struct State>>>& delta) { _delta = delta; };
	void setInitialState(const struct State& initialState) { _initialState = initialState; };
	void setFinalStates(const vector<struct State>& finalStates) { _finalStates = finalStates; };
	vector<struct State> getStates() const { return _states; };
	vector<struct State> getInputs() const { return _inputs; };
	vector<vector<vector<struct State>>> getDelta() const { return _delta; };
	struct State getInitialState() const { return _initialState; };
	vector<struct State> getfinalStates() const { return _finalStates; };
	
	Automata convertToDFA();
	Automata operator + (const Automata&) const;
	bool isNewState(struct State, vector<struct State>);
	bool findFinalStates(vector<struct State>);
	bool pertenece(struct State, vector<struct State>);
	void addNewState(struct State, vector<struct State>);

};

//	--	implementation	--

Automata::Automata(const vector<struct State>& states, const vector<struct State>& inputs,
	const vector<vector<vector<struct State>>>& delta, const struct State initialState,
	const vector<struct State> finalStates) : _states(states), _inputs(inputs), _delta(delta),
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
	queue<struct State> t_cola;
	vector<struct State> t_states;
	vector<struct State> t_inputs;
	vector<vector<vector<struct State>>> t_delta;
	struct State t_initialState;
	vector<struct State> t_finalStates;

	// implementation
	int it = 0; // iterate through delta
	int idState = 0; // for new states;
	t_states.push_back(this->_states[idState]);
	t_cola.push(t_states[idState]);
	idState++;
	t_delta.push_back(vector<vector<struct State>>());
	while (!t_cola.empty())
	{
		struct State currentState = t_cola.front();
		t_cola.pop();
		// evaluar simbolos
		for (int s = 0; s < this->_inputs.size(); s++)
		{
			vector<struct State> outputStates = this->_delta[currentState.id][s];
			// construct name
			// asign new state string se convertira en un solo estado
			string aux = "{" + to_string(outputStates[0].id);
			for (int j = 1; j < outputStates.size(); j++) 
				aux += "," + to_string(outputStates[s].id);
			aux += "}";
			
			struct State outputSingleState = { aux, -1 }; // si es estado nuevo, se le asigna id
			t_delta[it][s].push_back(outputSingleState);
			if (this->isNewState(outputSingleState, t_states))
			{
				outputSingleState.id = idState++;
				t_states.push_back(outputSingleState);
				t_cola.push(outputSingleState);
				if (this->findFinalStates(outputStates)) {
					this->addNewState(outputSingleState, t_finalStates);
				}
			}
		}
		t_delta.push_back(vector<vector<struct State>>());
		it++;
	}

}

bool Automata::isNewState(struct State checkState , vector<struct State> states) {
	this->pertenece(checkState, states);
}

bool Automata::findFinalStates(vector<struct State> checkStates) {
	for (struct State state : checkStates)
		if (this->pertenece(state, this->_finalStates)) {
			return true;
		}
	return false;
}

bool Automata::pertenece(struct State checkState, vector<struct State> states) {
	for (struct State state : states) {
		if (checkState.label.compare(state.label) == 0) {
			return false;
		}
	}
	return true;
}

void Automata::addNewState(struct State state, vector<struct State> states) {
	if (!this->pertenece(state, states)) {
		states.push_back(state);
	}
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

Conjunto delta(Conjunto estados, int simbolo)
{
	for (int i = 0; i < estados.length; i++)
	{
		if (estados.elementos[i] == -1)
	}
}

void fromNFAtoDFA(AFN af)
{
	vector<Conjunto> finalStates;
	vector<Conjunto> states;
	queue<Conjunto> cola;
	Conjunto outputState;
	// estado inicial no es conjunto, hay que convertirlo
	//convert(int, conjuntode1);
	Conjunto cEstadoInicial;
	cEstadoInicial.length = 1;
	cEstadoInicial.elementos = new int [1];
	cEstadoInicial.elementos[0] = af.edoInicial;
	cola.push(cEstadoInicial);
	int c = 0;
	while (!cola.empty())
	{
		Conjunto checkNewState;
		Conjunto currentState = cola.front(); // {0} cEstadoInicial la primer vez
		if (currentState.elementos[0] >= 0) // si no es el estado vacío
		{
			for (int estado = 0; estado < currentState.length; estado++)
			{
				for (int simbolo = 0; simbolo < af.simbolos.length; simbolo++)
				{
					outputState.length = af.delta[estado][simbolo].length;
					outputState.elementos = new int [outputState.length];
					for (int k = 0; k < outputState.length; k++)
					{
						outputState.elementos[k] = af.delta[estado][simbolo].elementos[k];
					}
					if (isNewState(outputState))
					{
						cola.push(outputState);
						states.push_back(outputState);
						if (checkForFinalStates(outputState))
							finalStates.push_back(outputState);
					}
				}
			}

		}
		
		cola.pop();
	}

}

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

Automata fillFDA()
{
	Conjunto i_estados;
	Conjunto i_simbolos;
	int** i_delta;
	int i_edoInicial;
	Conjunto i_edosFinales;

	cout << "Ingresa el número de estados y el numero de simbolos: ";
	cin >> i_estados.length >> i_simbolos.length;
	cout << endl;

	i_estados.elementos = new int[i_estados.length];
	for (int i = 0; i < i_estados.length; i++) i_estados.elementos[i] = i;

	i_simbolos.elementos = new int[i_simbolos.length];
	for (int i = 0; i < i_simbolos.length; i++) i_simbolos.elementos[i] = i;

	i_delta = new int* [i_estados.length];
	for (int i = 0; i < i_estados.length; i++)
	{
		cout << "Escriba separado con espacios los " << i_simbolos.length << " input del estado numero "
			<< i << ": ";
		i_delta[i] = new int[i_simbolos.length];
		for (int j = 0; j < i_simbolos.length; j++)
			cin >> i_delta[i][j];
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
	char r;
	cout << "El automata es finito determinista?(y/n) ";
	cin >> r;
	if (r == 'y')
		fillFDA();
	else
	{
		fillNFA();
		//fromNFAtoDFA();
	}
	cout << endl;
	return 0;
}
