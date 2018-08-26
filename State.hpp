#ifndef CPPREMOTEAPI_STATE_H
#define CPPREMOTEAPI_STATE_H

#include <iostream>
#include <vector>

#define START_STATE {9, 9, 0}
#define END_STATE {{0, 9, 0}, {0, 9, 90}, {0, 9, 180}, {0, 9, 270}}

class State {
public:
	//Contructor
	State(int x, int y, int theta, float cost) : x(x), y(y), theta(theta), cost(cost) {}
	State(int x, int y, int theta) : x(x), y(y), theta(theta), cost(0) {}

	//Public methods
	std::vector<State> expand();

	//Static methods
	static State getStartState() {return START_STATE;}
	static std::vector<State> getEndState() {return END_STATE;}

	//Operator overload
	friend std::ostream& operator<<(std::ostream &o, const State &s);

	bool operator==(const State &rhs) const;

	//Getters
	int getX() const { return x; }
	int getY() const { return y; }
	int getTheta() const { return theta; }
	float getCost() const { return cost; }

	//Setters
	void setX(int x) { State::x = x; }
	void setY(int y) { State::y = y; }
	void setTheta(int theta) { State::theta = theta; }
	void setCost(float cost) { State::cost = cost; }

private:
	int x, y, theta;
	float cost;

	bool isRightObstacle();
	bool isLeftObstacle();
	bool isObstacle(int x, int y);
};

//Other Overloads
bool operator==(const State &s, const std::vector<State> &rhs);
std::ostream& operator<<(std::ostream& out, const std::vector<State>& vector);
#endif //CPPREMOTEAPI_STATE_H
