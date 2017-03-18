#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <list>
#include <cstdlib>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <ctime>
using namespace std;

#define MAX_NODES 30000

bool influenced[MAX_NODES];
double d[MAX_NODES];

void init_dijkstra()
{
    for (int i =  0;i<MAX_NODES;i++)
    {
        d[i] = numeric_limits<double>::max();
    }
}
//comparator to compare by distance
struct compare
{
    bool operator()(const pair<int, double> &l, const pair<int, double> &r)
    {
        return l.second > r.second;
    }
};

void dijkstra(const vector<list<pair<int, double> > > &adj_list, int source_node, double T)
{
    init_dijkstra();
    d[source_node] = 0;
    priority_queue <pair<int, double>, vector<pair<int, double> >, compare> pq;

    pq.push(make_pair(source_node, d[source_node]));

    while (!pq.empty())
    {
        int u = pq.top().first;
        double u_dist = pq.top().second;
        pq.pop();

        if (u_dist >= T)
            continue;

        for (auto i = adj_list[u].begin();i!=adj_list[u].end();i++)
        {
            int v = i->first;
            double weight = i->second;

            //check if d[v] == T if so then we cant visit this nodes neighbours

            //relaxation
            if (d[v] > d[u] + weight)
            {
                d[v] = d[u] + weight;

                if (d[v] < T)
                    pq.push(make_pair(v, d[v]));
            }
        }
    }

}

//returns the top influencer
int find_top_influencer(const vector<list<pair<int, double> > > &adj_list, double T)
{
    int max_spread  = 0;
    int top_influencer = 0;

    for (int source_node = 0;source_node<MAX_NODES;source_node++)
    {
        if (adj_list[source_node].empty())
            continue;

        //INITIALIZE SINGLE SOURCE
        
        dijkstra(adj_list, source_node, T);

        //now look at spread
        int spread = 0;
        for (int i = 0;i<MAX_NODES;i++)
        {
            if (d[i] <= T)
                spread++;
        }

        if (spread > max_spread)
        {
            max_spread = spread;
            top_influencer = source_node;
        }

        //if the spreads are equal (tie) randomly choose one as top influencer
        else if (spread == max_spread && rand()%2 == 0)
            top_influencer = source_node;
    }

    cout << "TOP-1 INFLUENCER: "<<top_influencer<<", SPREAD: "<<max_spread<<", ";
    return top_influencer;
}
void find_top2_influencer(const vector<list<pair<int, double> > > &adj_list, double T, int top_influencer)
{
    //run dijkstra on top 1 influencer and mark all influenced nodes influenced
    dijkstra(adj_list, top_influencer, T);
    //mark all nodes influened by the top 1 as influenced
    for (int i = 0;i<MAX_NODES;i++)
    {
        if (d[i] <= T)
            influenced[i] = true;
    }

    int top2_influencer = 0;
    int max_spread  = 0;
    for (int source_node = 0;source_node<MAX_NODES;source_node++)
    {
        if (source_node == top_influencer)
            continue;
        if (adj_list[source_node].empty())
            continue;
        
        dijkstra(adj_list, source_node, T);

        //now look at spread
        int spread = 0;
        for (int i = 0;i<MAX_NODES;i++)
        {
            if (d[i] <= T && !influenced[i])
                spread++;
        }

        if (spread > max_spread)
        {
            max_spread = spread;
            top2_influencer = source_node;
        }

        //if the spreads are equal (tie) randomly choose one as top influencer
        else if (spread == max_spread && rand()%2 == 0)
            top2_influencer = source_node;
    }


    cout << "TOP-2 INFLUENCER: "<<top2_influencer<<", MARGINAL SPREAD: "<<max_spread<<", ";

}
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "usage: influence <graph file> <deadline constraint (T)>\n";
        return -1;
    }

    srand(time(NULL));

    //adjacency list representation
    vector<list <pair<int, double> > > adj_list(MAX_NODES);

    ifstream in(argv[1]);

    stringstream ss(argv[2]);

    double T;

    ss >> T;

    //creating the graph, start timing for the top influencer here
    clock_t begin = clock();
    //get inputs
    int max_vert_no = 0;//holds max vertex, dont want to iterate through 0s
    while (!in.eof())
    {
        string buf;
        getline(in, buf);
        istringstream is(buf);
        int u, v;
        double weight;

        is >> u >> v >> weight;

        adj_list[u].push_back(make_pair(v, weight));

        if (u > max_vert_no)
            max_vert_no = u;
        if (v > max_vert_no)
            max_vert_no = v;

        assert(u >= 0);
        assert(v >= 0);
    }

    /*
       IC model calculations
     */

    //nothing influenced yet, initializes the influenced flag of each node
    for (int i = 0;i<MAX_NODES;i++)
    {
        influenced[i] = false;
    }

    int top_influencer = find_top_influencer(adj_list, T);
    //stop timing top influence here
    clock_t end = clock();
    double t = ((double)(end - begin))/CLOCKS_PER_SEC;

    cout << "TIME: " << t <<" sec"<<endl;

    //start timing for top 2 influencer here
    begin = clock();
    find_top2_influencer(adj_list, T, top_influencer);
    end = clock();
    t = ((double)(end - begin))/CLOCKS_PER_SEC;

    cout<<"TIME: "<<t<<" sec"<<endl;

    return 0;
}
