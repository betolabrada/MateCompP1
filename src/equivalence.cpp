 /*
	- equivalence.cpp
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
#include "automata.h"

using namespace std;


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

	cout << "Equivalencia entre AFDs" << endl;
	
	vector<State> i_states;
	vector<State> i_inputs;
	vector<vector<State>> i_delta;
	State i_initialState;
	vector<State> i_finalStates;

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
			statesInDelta = 1;
			cout << endl;
			cout << "Escriba estado en delta(" + i_states[i].getLabel() + ", " +
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
	while (1) 
	{

		aut1 = fillAutomata();
		aut2 = fillAutomata();
		aut2.shift(aut1.getStates().size());
		tableFillingAlgorithm(aut1, aut2);
		cout << "--------------------------------------------" << endl;

	}

	return 0;
}
