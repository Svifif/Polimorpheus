#pragma once
#include <string>

enum class Direction
{
    LEFT = -1,
    RIGHT = 1,
    STAY = 0
};
template<typename T>
class Transition 
{
private:
    std::string currentState;
    T readSymbol;
    std::string nextState;
    T writeSymbol;
    Direction moveDirection;

public:
    Transition(const std::string& currState, T readSym,const std::string& nState, T writeSym, Direction moveDir): currentState(currState), readSymbol(readSym),nextState(nState), writeSymbol(writeSym), moveDirection(moveDir)     {}
    // Геттеры
    const std::string& getCurrentState() const { return currentState; }
    T getReadSymbol() const { return readSymbol; }
    const std::string& getNextState() const { return nextState; }
    T getWriteSymbol() const { return writeSymbol; }
    Direction getMoveDirection() const { return moveDirection; }
    // Проверка соответствия правилу
    bool matches(const std::string& state, T symbol) const
    {
        return state == currentState && symbol == readSymbol;
    }
};