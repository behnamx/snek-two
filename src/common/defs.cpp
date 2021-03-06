#pragma once

#include <map>

using namespace std;

typedef int snake_index;

enum class CellType {
    empty,
    wall,
    food
};

enum class Direction {
    None = 0,
    North = 1,
    South = 2,
    West = 3,
    East = 4
};

ostream& operator<<(std::ostream& o, Direction c)
{
    cout << static_cast<int>(c);
    return o;
}

const vector<Direction> DIRECTIONS = {Direction::North, Direction::South, Direction::West, Direction::East};
const int MAX_HEALTH = 100;
const int HEALTH_LOSS = 1;
const int FREE_MOVES = 2;

map<CellType, string> CELL_STR_MAP {
    {CellType::empty, "  "},
    {CellType::wall, "X "},
    {CellType::food, "f "}
};

map<Direction, string> DIR_STR_MAP {
    {Direction::North, "up"},
    {Direction::South, "down"},
    {Direction::West, "left"},
    {Direction::East, "right"}
};