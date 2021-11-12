#include "Pathfinding.h"

//operator overload for nodes
bool operator== (node a, node* b)
{
    if (a.x == b->x && a.y == b->y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::vector<int>> Astar(std::vector<std::vector<node>> grid, int Sx, int Sy, int Ex, int Ey)
{
    //returns the distance using pythagoras
    auto heuristic = [&](node* a, node* b)
    {
        return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
    };

    //giving neighbour pointers to each node
    for (int y = 0; y < grid.size(); y++)
    {
        for (int x = 0; x < grid[0].size(); x++)
        {
            if (y > 0)
            {
                grid[y][x].neighbours.push_back(&grid[y - 1][x]);
                if (grid[y][x].diagonals) {
                    if (x < grid[0].size() - 1) grid[y][x].neighbours.push_back(&grid[y - 1][x + 1]);
                    if (x > 0) grid[y][x].neighbours.push_back(&grid[y - 1][x - 1]);
                }
            }

            if (x > 0) grid[y][x].neighbours.push_back(&grid[y][x - 1]);

            if (x < grid[0].size() - 1) grid[y][x].neighbours.push_back(&grid[y][x + 1]);

            if (y < grid.size() - 1)
            {
                grid[y][x].neighbours.push_back(&grid[y + 1][x]);
                if (grid[y][x].diagonals) {
                    if (x < grid[0].size() - 1) grid[y][x].neighbours.push_back(&grid[y + 1][x + 1]);
                    if (x > 0) grid[y][x].neighbours.push_back(&grid[y + 1][x - 1]);
                }
            }
        }
    }

    //creates pointers start and end nodes
    node* nodeEnd = &grid[Ey][Ex];

    node* nodeStart = &grid[Sy][Sx];
    nodeStart->lWeight = 0.0f;
    nodeStart->gWeight = heuristic(nodeStart, nodeEnd);

    node* nodeCurrent = nodeStart;

    //creates queue array of pointers
    std::vector<node*> Queue;
    Queue.push_back(nodeCurrent);

    int nodesChecked = 1;

    //searching until it finds the end or the queue is empty
    while (!Queue.empty() && !(*nodeCurrent == nodeEnd))
    {
        //sorts the queue using a compare lambda function that returns what gWeight is higher
        std::sort(Queue.begin(), Queue.end(), [](node* a, node* b) {return a->gWeight < b->gWeight; });

        //removes tested nodes from queue
        while (!Queue.empty() && Queue.front()->tested) Queue.erase(Queue.begin());

        //breaks loop if queue is finished
        if (Queue.empty()) break;

        //changes current node
        nodeCurrent = Queue.front();
        nodeCurrent->tested = true;
        nodesChecked++;

        //goes through each neighbour of the current node
        for (node* nodeNeighbour : nodeCurrent->neighbours)
        {
            //add node to queue if not already tested or an obstacle
            if (!nodeNeighbour->tested && !nodeNeighbour->obstacle) Queue.push_back(nodeNeighbour);

            //changes the local weight of the node if its smaller
            double goalTest = nodeCurrent->lWeight + heuristic(nodeCurrent, nodeNeighbour);

            if (goalTest < nodeNeighbour->lWeight)
            {
                //updates parent node if its changed
                nodeNeighbour->parent = nodeCurrent;
                nodeNeighbour->lWeight = goalTest;

                //updatees global weight
                nodeNeighbour->gWeight = nodeNeighbour->lWeight + heuristic(nodeNeighbour, nodeEnd);
            }
        }
    }

    node* backtracker = &grid[Ey][Ex];

    //creates vector of coords for the trail
    std::vector<std::vector<int>> trail;

    //while the backtracker is not at the start
    while (!(*backtracker == nodeStart))
    {
        //add the backtracker to the list
        trail.push_back({ backtracker->x, backtracker->y });

        //changes backtracker pointer to the parent of the current node
        backtracker = backtracker->parent;
    }

    return trail;
}