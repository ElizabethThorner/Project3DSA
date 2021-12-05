#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <utility>
#include <chrono>
#include <cstdlib>
#include <random>
#include <string>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using namespace std::chrono;
using namespace std;
typedef high_resolution_clock Clock;

int main()
{
    //graph varibles
    int numberNode = -1;
    int maxEdgePerNode = -1;                                     //minimum of 2 edges (2 edges will just be a straight path)

    while (numberNode < 2)
    {
        cout << "Enter the number of node: ";
        cin >> numberNode;
    }

    while (maxEdgePerNode < 2)
    {
        cout << "Enter the max number of edges per node (minimum of 2): ";
        cin >> maxEdgePerNode;
    }

    //create random
    static random_device rd;
    static mt19937 rng(rd());
    static uniform_int_distribution<int> distanceRange(10, 3000);  //meter
    static uniform_int_distribution<int> neighborRange(2, 12);     // 10 possible nearby neighbors

    //adjacency list of the entire city's address and roads
    //           <from, pair<to, weight>>
    map<int, set<pair<int, int>>> graph;

    vector<int> edgeCount(numberNode + 2, 0);                               //number of edges for each address tracker
    int random = 0;

    //add all the address and edges to the graph
    for (int i = 1; i <= numberNode; i++)
    {
        //from itself to 2 other possble neighbor within (itself + 2) to (itself + 12) (ex: address 3 can have numEdgePerNode edges to address 5 - 15)
        set<int> nearbyNeighbor;
        while ((nearbyNeighbor.size() != maxEdgePerNode - 2) && nearbyNeighbor.size() != numberNode - i - 1 && i != numberNode) //2 edges is reserved for straight path
        {
            random = i + neighborRange(rd);

            if (random <= numberNode)                            // neighbor edge cant be larger then number of nodes that exist
            {
                nearbyNeighbor.emplace(random);
            }
        }

        //add edges from current address to nearby neighbor
        for (auto j = nearbyNeighbor.begin(); j != nearbyNeighbor.end(); j++)
        {
            if (edgeCount[i] < maxEdgePerNode - 1 && edgeCount[*j] < maxEdgePerNode - 1)                 //if there is room, add the new edge
            {
                random = distanceRange(rd);
                graph[i].emplace(make_pair(*j, random));
                graph[*j].emplace(make_pair(i, random));
                edgeCount[i] += 1;
                edgeCount[*j] += 1;
            }
        }

        random = distanceRange(rd);
        graph[i].emplace(make_pair(i + 1, random));             //edge for the next neighbor (from, to) 
        graph[i + 1].emplace(make_pair(i, random));             //                           (to, from)
        edgeCount[i] += 1;                                      //increment edge tracker
        edgeCount[i + 1] += 1;
    }

    graph.erase(numberNode + 1);                                //delete the one excess ndoe from the graph
    graph[numberNode].erase(make_pair(numberNode + 1, random)); //delete the unreachable ndoe from the last address

    //cout << "map size: " << graph.size() << endl;

    for (auto i = graph.begin(); i != graph.end(); i++)
    {
        cout << "index: " << i->first << endl;
        cout << "edges: ";

        for (auto j = i->second.begin(); j != i->second.end(); j++)
        {
            cout << "(" << j->first << ", " << j->second << "); ";
        }

        cout << endl << endl;
    }

    return 0;
}


