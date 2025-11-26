#pragma once
#include "Tape.hpp"
#include <cassert>
#include <iostream>

class TapeTests 
{
public:
    static void test_tape_basic()
    {
        std::cout << "Testing basic tape operations...\n";

        Tape<char> tape('_');
        char test[] = { 'A', 'B', 'C', 'D' };
        tape.initialize(test, 4, 1);  // Start at 'B'

        assert(tape.read() == 'B');

        tape.moveRight();
        assert(tape.read() == 'C');

        tape.moveLeft();
        assert(tape.read() == 'B');

        tape.write('X');
        assert(tape.read() == 'X');

        std::cout << "Basic tape tests passed!\n";
    }

    static void test_tape_edges()
    {
        std::cout << "Testing tape edge cases...\n";

        Tape<char> tape('_');
        char test[] = { 'A' };
        tape.initialize(test, 1, 0);

        assert(tape.read() == 'A');

        tape.moveRight();
        assert(tape.read() == '_');  // Should get blank

        tape.moveLeft();
        assert(tape.read() == 'A');  // Should return to A

        std::cout << "Tape edge tests passed!\n";
    }

    static void test_all() 
    {
        std::cout << "=== Running Tape Tests ===\n";
        test_tape_basic();
        test_tape_edges();
        std::cout << "=== All Tape tests passed! ===\n\n";
    }
};