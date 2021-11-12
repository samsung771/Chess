#pragma once
#include <vector>
#include <algorithm>

//node data type
struct node
{
    bool obstacle = false;
    bool tested = false;
    bool diagonals = true;
    int x;
    int y;
    double gWeight = 1000000;
    double lWeight = 1000000;
    std::vector<node*> neighbours;
    node* parent = nullptr;
};

//A* pathfinding function 
//modified from OneLoneCoder's tutorial (https://www.youtube.com/watch?v=icZj67PTFhc)
std::vector<std::vector<int>> Astar(std::vector<std::vector<node>> grid, int Sx, int Sy, int Ex, int Ey);