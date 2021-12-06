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

#include "Dijkstra.h"

using namespace std::chrono;
using namespace std;
typedef high_resolution_clock Clock;

vector<int> Dijkstra(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source);

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
            if (edgeCount[i] < maxEdgePerNode - 1 && edgeCount[*j] < maxEdgePerNode - 2)    //save 1 edge for self and 2 edge for nearby Neighbor
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
    //print the entire map
    /*for (auto i = graph.begin(); i != graph.end(); i++)
    {
        cout << "index: " << i->first << endl;
        cout << "edges: ";

        for (auto j = i->second.begin(); j != i->second.end(); j++)
        {
            cout << "(" << j->first << ", " << j->second<< "); ";
        }

        cout << endl << endl;
    }*/

    //generate random amount of delivery address
    /*static uniform_int_distribution<int> deliveryRange(2, numberNode);     // random
    int numDeliveryAddress = 2;
    set<int> deliveryList;

    while (deliveryList.size() != numDeliveryAddress)
    {
        deliveryList.emplace(deliveryRange(rd));
    }

    for (auto i = deliveryList.begin(); i != deliveryList.end(); i++)
    {
        cout << *i << ", ";
    }
    cout << endl;*/

    Dijkstra(graph, numberNode, 1);

    cout << "Dijkstra finished";
    return 0;
}

//return the distance from source to each node
vector<int> Dijkstra(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source)
{
    vector<int> visited (_numNodes + 1, 0);                     // keep track which nodes is visited
    vector<int> distance(_numNodes + 1, 300000000);              // track the distance from the source node 

    int minIndex = _source;
    distance[_source] = 0;

    for (int i = 0; i < _numNodes; i++)
    {
        int min = 300000000;

        //find the smallest distance thats not been to
        for (int k = 0; k < distance.size(); k++)
        {
            if (distance[k] < min && visited[k] == 0)
            {
                min = distance[k];
                minIndex = k;
            }
        }

        //do calculate to its childrens
        for (auto j = _graph[minIndex].begin(); j != _graph[minIndex].end(); j++)
        {
            //if the new distance is less then update
            if (distance[minIndex] + j->second < distance[j->first])
            {
                distance[j->first] = distance[minIndex] + j->second;
            }
        }

        //finished visiting this node
        visited[minIndex] = 1; //set to true 
    }

    //test print
    /*for (int i = 0; i < distance.size(); i++)
    {
        cout << "Distance to " << i << ": " << distance[i] << endl;
    }*/

    return distance;
}
