// -- implementacion --
#include "state.h"
#include <algorithm>

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