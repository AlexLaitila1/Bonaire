#include "gameboard.hh"
#include <iostream>
#include <random>

GameBoard::GameBoard()
{
    init();
}

std::string GameBoard::fill_randomly(const std::string& str_seed)
{
    unsigned int seed = stoi_with_check(str_seed);
    // Checking the validity of the given seed
    for(unsigned int bad_seed : BAD_SEEDS)
    {
        if(seed == bad_seed)
        {
            return "";
        }
    }
    // Constructing a string with random symbols
    std::default_random_engine eng(seed);
    std::uniform_int_distribution<unsigned int> distr(0, DISTR_UPPER_BOUND);
    std::string input = "";

    for(unsigned int i = 0; i < SIZE * SIZE; ++i)
    {
        switch(distr(eng))
        {
        case 0: input += '0'; break;
        case 1: input += '1'; break;
        default: input += ' '; break;
        }
    }

    // Calling fill_from_input with the constructed string (enclosed with quote
    // marks), whereupon the return value of fill_from_input will be true
    return '"' + input + '"';
}

bool GameBoard::fill_from_input(const std::string &input)
{
    // Checking the size (assuming that input is enclosed with quote marks)
    if(input.size() != SIZE * SIZE + 2)
    {
        return false;
    }

    // Removing quote marks
    std::string actual_input = input.substr(1, SIZE * SIZE);

    // Checking the content and moving each element on the gameboard
    for(unsigned int i = 0; i < SIZE * SIZE; ++i)
    {
        Element_type current_element = EMPTY;
        switch(actual_input.at(i))
        {
        case '0': current_element = ZERO; break;
        case '1': current_element = ONE; break;
        case ' ': current_element = EMPTY; break;
        default: return false;
        }

        board_.at(i / SIZE).at(i % SIZE ) = current_element;
    }
    if(ok_adjacent_symbols() and ok_amount_of_symbols())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool GameBoard::ok_adjacent_symbols() const
{
    return ok_adjacent_syms_in_rows(ZERO) and
           ok_adjacent_syms_in_rows(ONE) and
           ok_adjacent_syms_in_columns(ZERO) and
           ok_adjacent_syms_in_columns(ONE);
}

bool GameBoard::ok_amount_of_symbols() const
{
    unsigned int zeros_in_row = 0;
    unsigned int ones_in_row = 0;
    unsigned int zeros_in_column = 0;
    unsigned int ones_in_column = 0;

    for(unsigned int i = 0; i < board_.size(); ++i)
    {
        for(unsigned int j = 0; j < board_.at(i).size(); ++j)
        {
            if(board_.at(i).at(j) == ZERO)
            {
                ++zeros_in_row;
            }
            else if(board_.at(i).at(j) == ONE)
            {
                ++ones_in_row;
            }
            if(board_.at(j).at(i) == ZERO)
            {
                ++zeros_in_column;
            }
            else if(board_.at(j).at(i) == ONE)
            {
                ++ones_in_column;
            }
        }
        if(zeros_in_row > NUMBER_OF_SYMBOLS or
            ones_in_row > NUMBER_OF_SYMBOLS or
            zeros_in_column > NUMBER_OF_SYMBOLS or
            ones_in_column > NUMBER_OF_SYMBOLS)
        {
            return false;
        }
        zeros_in_row = ones_in_row = zeros_in_column = ones_in_column = 0;
    }
    return true;
}

bool GameBoard::add_symbol(unsigned int x, unsigned int y, char symbol_char)
{
    if(board_.at(y).at(x) != EMPTY)
    {
        //std::cout << "Not empty: ";
        return false;
    }

    Element_type elem = EMPTY;
    switch(symbol_char)
    {
    case '0': elem = ZERO; break;
    case '1': elem = ONE; break;
    default: return false; // This should never happen, checked in main.cpp
    }

    // Adding the symbol and checking if everything is still fine
    board_.at(y).at(x) = elem;
    if(ok_adjacent_symbols() and
        ok_amount_of_symbols())
    {
        return true;
    }
    // If all was not fine after adding, making the index empty again
    else
    {
        board_.at(y).at(x) = EMPTY;
        return false;
    }
}

bool GameBoard::is_game_over() const
{
    for(unsigned int i = 0; i < board_.size(); ++i)
    {
        for(unsigned int j = 0; j < board_.at(i).size(); ++j)
        {
            if(board_.at(i).at(j) == EMPTY)
            {
                return false;
            }
        }
    }
    return true;
}

void GameBoard::init()
{
    std::vector<Element_type> row(SIZE, EMPTY);
    for(unsigned int i = 0; i < SIZE; ++i)
    {
        board_.push_back(row);
    }
}

void GameBoard::print_line(unsigned int length, char fill_character) const
{
    for(unsigned int i = 0; i < length; ++i)
    {
        std::cout << fill_character;
    }
    std::cout << std::endl;
}

bool GameBoard::ok_adjacent_syms_in_rows(Element_type elem) const
{
    for(unsigned int i = 0; i < board_.size(); ++i)
    {
        for(unsigned int j = 0; j < board_.at(i).size() - 2; ++j)
        {
            if(board_.at(i).at(j) == elem and
                board_.at(i).at(j + 1) == elem and
                board_.at(i).at(j + 2) == elem)
            {
                return false;
            }
        }
    }
    return true;
}

bool GameBoard::ok_adjacent_syms_in_columns(Element_type elem) const
{
    for(unsigned int i = 0; i < board_.size() - 2; ++i)
    {
        for(unsigned int j = 0; j < board_.at(i).size(); ++j)
        {
            if(board_.at(i).at(j) == elem and
                board_.at(i + 1).at(j) == elem and
                board_.at(i + 2).at(j) == elem)
            {
                return false;
            }
        }
    }
    return true;
}

unsigned int GameBoard::stoi_with_check(const std::string& str)
{
    bool is_numeric = true;

    // Go thrue all the caracters in the string and check if they are integers.
    for(unsigned int i = 0; i < str.length(); ++i)
    {
        if(not isdigit(str.at(i)))
        {
            // if even one character is not integer method returns 0.
            is_numeric = false;
            break;
        }
    }

    if(is_numeric)
    {
        // Bad seeds are only handled between 1-50.
        int int_str = stoi(str);
        if(int_str < 50 and int_str > 0){
            return int_str;
        }
        else{
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
