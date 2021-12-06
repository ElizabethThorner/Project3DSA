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
#include <list>

using namespace std::chrono;
using namespace std;
typedef high_resolution_clock Clock;

int main()
{
    //graph variables
    int numberNode = -1;
    int maxEdgePerNode = -1;                                     //minimum of 2 edges (2 edges will just be a straight path)

    while (numberNode < 2)
    {
        cout << "Enter the number of nodes: " << endl;
        cin >> numberNode;
    }

    while (maxEdgePerNode < 2)
    {
        cout << "Enter the max number of edges per node (minimum of 2): " << endl;
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
        //from itself to 2 other possible neighbor within (itself + 2) to (itself + 12) (ex: address 3 can have numEdgePerNode edges to address 5 - 15)
        set<int> nearbyNeighbor;
        while ((nearbyNeighbor.size() != maxEdgePerNode - 2) && nearbyNeighbor.size() != numberNode - i - 1 && i != numberNode) //2 edges is reserved for straight path
        {
            random = i + neighborRange(rd);

            if (random <= numberNode)                            // neighbor edge can't be larger than number of nodes that exist
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

    graph.erase(numberNode + 1);                                //delete the one excess node from the graph
    graph[numberNode].erase(make_pair(numberNode + 1, random)); //delete the unreachable node from the last address

    //cout << "map size: " << graph.size() << endl;
    //print the entire map
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

    const int infinity = 2147483647;

    cout << "Generating an efficient path through Dijkstra's algorithm..." << endl;

    //time of start
    auto t1 = Clock::now();

    set<int> addressesAdded;
    list<int> dijkstraPath;
    auto iter = graph.begin();
    while (iter != graph.end()) {
        set<int> processed;
        vector<int> dist(graph.size() + 1, infinity);
        dijkstraPath.push_back(iter->first);
        dist[iter->first] = 0;
        addressesAdded.insert(iter->first);

        while (processed.size() != graph.size()) {
            int v = 0;
            for (auto iter2 = graph.begin(); iter2 != graph.end(); iter2++) {
                if (processed.count(iter2->first) == 0) {
                    v = iter2->first;
                    break;
                }
            }
            for (auto iter2 = graph.begin(); iter2 != graph.end(); iter2++) {
                if ((processed.count(iter2->first) == 0) && (dist[iter2->first] < dist[v])) {
                    v = iter2->first;
                }
            }
            processed.insert(v);
            for (auto j = graph[v].begin(); j != graph[v].end(); j++) {
                if (processed.count(j->first) == 0) {
                    if (dist[j->first] > dist[v] + j->second) {
                        dist[j->first] = dist[v] + j->second;
                    }
                }
            }
        }
        int min = infinity;
        int minIndex = 0;
        for (int i = 0; i < dist.size(); i++) {
            if (addressesAdded.count(i) == 0) {
                if (dist[i] < min) {
                    min = dist[i];
                    minIndex = i;
                }
            }
        }
        if (min == infinity) {
            iter = graph.end();
        }
        else {
            iter = graph.find(minIndex);
        }
    }

    //time of end
    auto t2 = Clock::now();
    cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to run Dijkstra's algorithm once";

    cout << "The path produced by Dijkstra's algorithm:" << endl;
    for (auto i = dijkstraPath.begin(); i != dijkstraPath.end(); i++) {
        cout << *i << endl;
    }

    cout << "Generating an efficient path through the Bellman-Ford algorithm..." << endl;

    //time start
    t1 = Clock::now();

    addressesAdded.clear();
    list<int> bellmanPath;
    iter = graph.begin();
    while (iter != graph.end()) {
        vector<int> dist(graph.size() + 1, infinity);
        bellmanPath.push_back(iter->first);
        dist[iter->first] = 0;
        addressesAdded.insert(iter->first);

        for (int i = 0; i < graph.size() - 1; i++) {
            for (int v = 1; v < graph.size() + 1; v++) {
                for (auto j = graph[v].begin(); j != graph[v].end(); j++) {
                    if (dist[j->first] > dist[v] + j->second) {
                        dist[j->first] = dist[v] + j->second;
                    }
                }
            }
        }
        int min = infinity;
        int minIndex = 0;
        for (int i = 0; i < dist.size(); i++) {
            if (addressesAdded.count(i) == 0) {
                if (dist[i] < min) {
                    min = dist[i];
                    minIndex = i;
                }
            }
        }
        if (min == infinity) {
            iter = graph.end();
        }
        else {
            iter = graph.find(minIndex);
        }

    }

    //time of end
    t2 = Clock::now();
    cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to run  Bellman-Ford algorithm once";

    cout << "The path produced by the Bellman-Ford algorithm:" << endl;
    for (auto i = bellmanPath.begin(); i != bellmanPath.end(); i++) {
        cout << *i << endl;
    }

    return 0;
}
