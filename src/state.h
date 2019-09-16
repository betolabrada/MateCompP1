#pragma once
#include <string>
#include <vector>
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