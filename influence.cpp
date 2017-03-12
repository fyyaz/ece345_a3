#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <list>
#include <cstdlib>
#include <limits>
#include <cassert>

using namespace std;

//from sample graph file info
#define MAX_VERTEX 1000
#define MAX_EDGE 20000

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "usage: influence <graph file> <deadline constraint (T)>\n";
        return -1;
    }
    double **adj_matrix = new double *[MAX_VERTEX];
    double **dist = new double *[MAX_VERTEX];
    for (int i = 0;i < MAX_VERTEX;i++)
    {
	dist[i] = new double[MAX_VERTEX];
        adj_matrix[i] = new double[MAX_VERTEX];
        for (int j = 0;j < MAX_VERTEX;j++)
        {
            if (i == j) //to get from node to itself is 0 cost
                adj_matrix[i][j] = 0;
            else //initializes all distances to infinity
                adj_matrix[i][j] = numeric_limits<double>::max();
        }
    }
    
    ifstream in(argv[1]);
    int T = atoi(argv[2]);

    //get inputs
    int max_vert_no = 0;//holds max vertex, dont want to iterate through 0s
    while (!in.eof())
    {
        int u, v;
        double weight;

        in >> u >> v >> weight;

        adj_matrix[u][v] = weight;

        if (u > max_vert_no)
            max_vert_no = u;
        if (v > max_vert_no)
            max_vert_no = v;

        assert(u >= 0 && u <= MAX_VERTEX);
        assert(v >= 0 && v <= MAX_VERTEX);
    }

    /*
       IC model calculations
       Top 1 influencer: 
                run floyd warshall O(|V|^3)
                find max spread node O(|V|)
       Top 2 influencer:
                same idea
       Thinking to use floyd warshall since we need to calculate all pairs 
       shortest paths for this while 
                djikstra will take |V|*O(|E| + |V|log|V|)
       since graph can become very dense |V||E| > |V|^3 
     */

    //floyd warshall top1 
    for (int i = 0;i < max_vert_no;i++)
    {
        dist[i][i] = 0;//not needed really
        for (int j = 0;j < max_vert_no;j++)
        {
            dist[i][j] = adj_matrix[i][j];
        }
    }
    for (int k = 0;k < max_vert_no;k++)
    {
        for (int i = 0;i < max_vert_no;i++)
        {
            for (int j = 0;j < max_vert_no;j++)
            {
                if (dist[i][j] > dist[i][k] + dist[k][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    //find top 1 
    int top_node = 0;
    int max_spread = 0;

    for (int i = 0;i < max_vert_no;i++)
    {
        int spread = 0;
        for (int j = 0;j < max_vert_no;j++)
        {
            //little sigma(i) = size{v belongs to V:dist[u][v]<=T}
            if (dist[i][j] <= T)
                spread++;
        }

        if (spread > max_spread)
        {
            max_spread = spread;
            top_node = i;
        }
    }

    cout << "top 1: " << top_node << endl;

    //delete the adjacency matrix
    for (int i = 0;i < MAX_VERTEX;i++)
        delete [] adj_matrix[i];

    delete [] adj_matrix;
    return 0;
}
