/*
* COP3530 Fall 2021
* Project 3
*
* Date: 12/09/2021
* Group member: Zhen Wu, Nandini Tripathi, and Elizabeth Thorner
*
* Delivery simulation using the greedy approach by Dijkstra and Bellman-Ford algorithm.
* The program calculates a local optimal path the driver can take to finish his/her delivery trip.
* Estimation of how many gallons of gas will be used, CO2 emission, and estimated time will be also calculated.
*/

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
#include <stack>

using namespace std::chrono;
using namespace std;
typedef high_resolution_clock Clock;

vector<int> Dijkstra(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source, vector<int>& _prevNode);
vector<int> BellmanFord(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source, vector<int>& _prevNode);

int main()
{
    //graph varibles
    int numberNode = -1;
    int maxEdgePerNode = -1;                                     //minimum of 2 edges (2 edges will just be a straight path)
    int numDeliveryAddress = -1;
    int density = 1;
    int distanceApart = 1;
    int aveSpeed = 1;

    cout << "This program will simulate paths that a delivery driver could take to deliver all his/her packages " << endl;
    cout << "using the greedy approach. The user can choose between Dijkstra or Bellman-Ford algorithm or both. " << endl;
    cout << "The program will output the exact vertices the driver must follow to finish the trip. Estimation of how " << endl;
    cout << "many gallons of gas will be used, CO2 emission, and estimated time, and the total cost will be also calculated." << endl << endl;

    cout << "Choose delivery location density:" << endl;
    cout << "1 = countryside (addresses are no more than 1100 meters apart)" << endl;
    cout << "2 = town        (addresses are no more than 500 meters apart)" << endl;
    cout << "3 = city        (addresses are no more than 100 meters apart)" << endl;
    cout << "4 = Metropolis  (addresses are no more than 50 meters apart)" << endl;
    cin >> density;
    cout << endl;

    if (density == 1)
    {
        distanceApart = 1100;
        aveSpeed = 75;
    }
    else if (density == 2)
    {
        distanceApart = 500;
        aveSpeed = 45;
    }
    else if (density == 3)
    {
        distanceApart = 100;
        aveSpeed = 35;
    }
    else if (density == 4)
    {
        distanceApart = 50;
        aveSpeed = 30;
    }

    while (numberNode < 2)
    {
        cout << "Enter the number of vertices (minimum of 2): ";
        cin >> numberNode;
    }

    while (maxEdgePerNode < 2)
    {
        cout << "Enter the max number of edges per node (minimum of 2): ";
        cin >> maxEdgePerNode;
    }

    while (numDeliveryAddress < 1)
    {
        cout << "Enter the number of delivery addresses you want in your list (cant be >= the number of vertices): ";
        cin >> numDeliveryAddress;
    }
    cout << endl;

    //create random
    static random_device rd;
    static mt19937 rng(rd());
    static uniform_int_distribution<int> distanceRange(10, distanceApart);  //meter
    static uniform_int_distribution<int> neighborRange(2, 12);     // 10 possible nearby neighbors

    //adjacency list of the entire city's address and roads
    // <from, pair<to, weight>>
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

    //print the entire map
    int userInput = 2;

    cout << "Do You want to print entire graph? " << endl;
    cout << "1 = Yes " << endl;
    cout << "2 = No " << endl;
    cin >> userInput;

    if (userInput == 1)
    {
        for (auto i = graph.begin(); i != graph.end(); i++)
        {
            cout << "vertice: " << i->first << endl;
            cout << "Edges: ";

            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                cout << "(" << j->first << ", " << j->second << "); ";
            }

            cout << endl << endl;
        }
    }
    cout << endl;

    //generate a certain amount of random delivery address
    static uniform_int_distribution<int> deliveryRange(2, numberNode);     // random
    set<int> deliveryList;

    while (deliveryList.size() != numDeliveryAddress)
    {
        deliveryList.emplace(deliveryRange(rd));
    }

    cout << "Delivery addresses list: ";
    for (auto i = deliveryList.begin(); i != deliveryList.end(); i++)
    {
        cout << *i << ", ";
    }
    cout << endl << endl;

    cout << "Enter the algorithm you would like to use" << endl;
    cout << "1 = Dijkstra" << endl;
    cout << "2 = Bellman-Ford" << endl;
    cout << "3 = Both" << endl;
    cin >> userInput;
    cout << endl;

    double miles = 0;
    vector<int> path;                      //path taken tracker
    if (userInput == 1)                    // Dijkstra
    {
        auto t1 = Clock::now();

        int totalDistance = 0;             //keep track of total distance travelled
        int sourceNode = 1;                //go to the next closest node
        int preSourceNode = 1;

        while (!deliveryList.empty())
        {
            int min = 300000000;            //closest node distance
            vector<int> prevNode(numberNode + 1, -2);
            vector<int> closestDistance = Dijkstra(graph, numberNode, sourceNode, prevNode);

            if (sourceNode != 1)
            {
                deliveryList.erase(sourceNode);     //deleted the node that just delivered to
            }

            for (auto i = deliveryList.begin(); i != deliveryList.end(); i++)
            {
                if (closestDistance[*i] < min)
                {
                    min = closestDistance[*i];      //find the closest node
                    sourceNode = *i;
                }
            }

            while (prevNode[preSourceNode] != -1)   //push everything on to stack so we can reverse the order
            {
                path.push_back(preSourceNode);
                preSourceNode = prevNode[preSourceNode];
            }

            if (deliveryList.size() != 0)
            {
                totalDistance += min;
            }
            else                             //push the last desitaion node into path
            {
                path.push_back(sourceNode);
            }
        }

        miles = (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100;
        auto t2 = Clock::now();
        cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to use Dijkstra's algorithm" << endl;
        cout << "Total distance travelled: " << (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100 << " miles " << "(" << totalDistance << " meters)." << endl;
        cout << "Esmated time to complete trip (with average speed of " << aveSpeed << " miles per hour): " << (double)((int)((miles / aveSpeed) * 100)) / 100 << " hours." << endl;
        cout << endl;
    }
    else if (userInput == 2)            //Bellman-Ford
    {
        auto t1 = Clock::now();

        int totalDistance = 0;             //keep track of total distance travelled
        int sourceNode = 1;                //go to the next closest node
        int preSourceNode = 1;

        while (!deliveryList.empty())
        {
            int min = 300000000;            //closest node distance
            vector<int> prevNode(numberNode + 1, -2);
            vector<int> closestDistance = BellmanFord(graph, numberNode, sourceNode, prevNode);

            if (sourceNode != 1)
            {
                deliveryList.erase(sourceNode);     //deleted the node that just delivered to
            }

            for (auto i = deliveryList.begin(); i != deliveryList.end(); i++)
            {
                if (closestDistance[*i] < min)
                {
                    min = closestDistance[*i];      //find the closest node
                    sourceNode = *i;
                }
            }

            while (prevNode[preSourceNode] != -1)   //push everything on to stack so we can reverse the order
            {
                path.push_back(preSourceNode);
                preSourceNode = prevNode[preSourceNode];
            }

            if (deliveryList.size() != 0)
            {
                totalDistance += min;
            }
            else                             //push the last desitaion node into path
            {
                path.push_back(sourceNode);
            }
        }

        miles = (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100;
        auto t2 = Clock::now();
        cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to use Dijkstra's algorithm" << endl;
        cout << "Total distance travelled: " << (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100 << " miles " << "(" << totalDistance << " meters)." << endl;
        cout << "Esmated time to complete trip (with average speed of " << aveSpeed << " miles per hour): " << (double)((int)((miles / aveSpeed) * 100)) / 100 << " hours." << endl;
        cout << endl;
    }
    else if (userInput == 3)
    {
        set<int>tempList = deliveryList;

        auto t1 = Clock::now();

        int totalDistance = 0;                    //keep track of total distance travelled
        int sourceNode = 1;                       //go to the next closest node
        int preSourceNode = 1;

        while (!tempList.empty())
        {
            int min = 300000000;                   //closest node distance
            vector<int> prevNode(numberNode + 1, -2);
            vector<int> closestDistance = Dijkstra(graph, numberNode, sourceNode, prevNode);

            if (sourceNode != 1)
            {
                tempList.erase(sourceNode);         //deleted the node that just delivered to
            }

            for (auto i = tempList.begin(); i != tempList.end(); i++)
            {
                if (closestDistance[*i] < min)
                {
                    min = closestDistance[*i];      //find the closest node
                    sourceNode = *i;
                }
            }

            while (prevNode[preSourceNode] != -1)   //push everything on to stack so we can reverse the order
            {
                path.push_back(preSourceNode);
                preSourceNode = prevNode[preSourceNode];
            }

            if (tempList.size() != 0)
            {
                totalDistance += min;
            }
            else                                    //push the last desitaion node into path
            {
                path.push_back(sourceNode);
            }
        }

        miles = (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100;
        auto t2 = Clock::now();
        cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to use Dijkstra's algorithm" << endl;
        cout << "Total distance travelled: " << (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100 << " miles " << "(" << totalDistance << " meters)." << endl;
        cout << "Esmated time to complete trip (with average speed of " << aveSpeed << " miles per hour): " << (double)((int)((miles / aveSpeed) * 100)) / 100 << " hours." << endl;

        cout << endl;

        t1 = Clock::now();            //Bellman-Ford

        totalDistance = 0;             //keep track of total distance travelled
        sourceNode = 1;                //go to the next closest node
        preSourceNode = 1;

        while (!deliveryList.empty())
        {
            int min = 300000000;            //closest node distance
            stack<int> temp;
            vector<int> prevNode(numberNode + 1, -2);
            vector<int> closestDistance = BellmanFord(graph, numberNode, sourceNode, prevNode);

            if (sourceNode != 1)
            {
                deliveryList.erase(sourceNode);     //deleted the node that just delivered to
            }

            for (auto i = deliveryList.begin(); i != deliveryList.end(); i++)
            {
                if (closestDistance[*i] < min)
                {
                    min = closestDistance[*i];      //find the closest node
                    sourceNode = *i;
                }
            }

            while (prevNode[preSourceNode] != -1)   //push everything on to stack so we can reverse the order
            {
                path.push_back(preSourceNode);
                preSourceNode = prevNode[preSourceNode];
            }

            if (deliveryList.size() != 0)
            {
                totalDistance += min;
            }
            else                             //push the last desitaion node into path
            {
                path.push_back(sourceNode);
            }
        }

        miles = (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100;
        t2 = Clock::now();
        cout << "It took " << duration_cast<seconds>(t2 - t1).count() << " seconds to use Bellman-Ford's algorithm" << endl;
        cout << "Total distance travelled: " << (double)((int)(((double)totalDistance / 1609.34) * 100)) / 100 << " miles " << "(" << totalDistance << " meters)." << endl;
        cout << "Esmated time to complete trip (with average speed of " << aveSpeed << " miles per hour): " << (double)((int)((miles / aveSpeed) * 100)) / 100 << " hours." << endl;
        cout << endl;
    }

    //print the path
    cout << "Do You want to print the path the driver took?" << endl;
    cout << "1 = yes" << endl;
    cout << "2 = no" << endl;
    cin >> userInput;
    cout << endl;

    if (userInput == 1)
    {
        cout << "Note: #1 is the warehouse" << endl;
        for (int i = 0; i < path.size(); i++)
        {
            cout << path[i] << "->";
        }
    }
    cout << "end" << endl << endl;;

    int numOfDrivers = 1;
    double pay = 0;
    double mpg = 0;
    double costPerGallon = 0;

    cout << "Trip cost calculation" << endl;
    cout << "---------------------" << endl;
    cout << "Enter number of workers: ";
    cin >> numOfDrivers;
    cout << endl;

    cout << "Enter the hourly wage: ";
    cin >> pay;
    cout << endl;

    cout << "Enter the vehicle mpg: ";
    cin >> mpg;
    cout << endl;

    cout << "Enter the price per gallon of gas: ";
    cin >> costPerGallon;
    cout << endl;

    cout << "Result:" << endl;
    cout << "Wage: $" << (numOfDrivers*pay)*(double)((int)((miles/aveSpeed)*100))/100 << endl;
    cout << "Gas: $" << (double)((int)(((miles/mpg)*costPerGallon)*100))/100 << endl;
    cout << "Trip total cost: $" << (double)((int)(((miles/mpg)*costPerGallon)*100))/100 + (numOfDrivers*pay)*(double)((int)((miles/aveSpeed)*100))/100 << endl;

    //source: https://www.epa.gov/energy/greenhouse-gases-equivalencies-calculator-calculations-and-references
    cout << "CO2 emission for this trip is: " << 8.887*(miles/mpg) << " x10^-3 metric tons." << endl;

    cout << "Program Finished" << endl;
    return 0;
}

//return the distance from source to each node
vector<int> Dijkstra(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source, vector<int>& _prevNode)
{
    vector<int> visited(_numNodes + 1, 0);                     // keep track which nodes is visited
    vector<int> distance(_numNodes + 1, 300000000);            // track the distance from the source node
    _prevNode[_source] = -1;

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
                _prevNode[j->first] = minIndex;
            }
        }

        //finished visiting this node
        visited[minIndex] = 1;          //set to true
    }

    return distance;
}

//return the distance from source to each node
vector<int> BellmanFord(map<int, set<pair<int, int>>>& _graph, int _numNodes, int _source, vector<int>& _prevNode)
{
    vector<int> visited(_numNodes + 1, 0);                     // keep track which nodes is visited
    vector<int> distance(_numNodes + 1, 300000000);            // track the distance from the source node
    _prevNode[_source] = -1;

    distance[_source] = 0;

    while (true)                                               //BellmanFord runs at most v-1 times or when after an iteration that cant improve distance (break)
    {
        bool hasChanged = false;

        for (auto j = _graph.begin(); j != _graph.end(); j++)  //visit each node every iteration
        {
            if (distance[j->first] != 300000000)               //if the node havent been to then we skip
            {
                for (auto k = j->second.begin(); k != j->second.end(); k++)
                {
                    if (distance[j->first] + k->second < distance[k->first])   //if distance is less update
                    {
                        distance[k->first] = distance[j->first] + k->second;
                        _prevNode[k->first] = j->first;
                        hasChanged = true;
                    }
                }
            }

        }

        if (!hasChanged)
        {
            break;
        }
    }

    return distance;
}
