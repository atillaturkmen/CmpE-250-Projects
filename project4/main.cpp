#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>

using namespace std;

/*
I did not write detect_cycle and spfa_early_terminate functions. They are taken from this website:
https://konaeakira.github.io/posts/using-the-shortest-path-faster-algorithm-to-find-negative-cycles.html
*/

const int MAX_N = 300;
long long dis[MAX_N];
int pre[MAX_N];
bool in_queue[MAX_N];

bool visited[MAX_N], on_stack[MAX_N];

vector<int> cycle;

void trace(int v, int n) {
	std::fill(on_stack, on_stack + n, false);
	stack<int> s;
	while (!on_stack[v]) {
		s.push(v);
		on_stack[v] = true;
		v = pre[v];
	}
	cycle.push_back(v);
	while (s.top() != v) {
		cycle.push_back(s.top());
		s.pop();
	}
	cycle.push_back(v);
}

bool detect_cycle(int n)
{
    std::vector<int> vec;
    std::fill(on_stack, on_stack + n, false);
    std::fill(visited, visited + n, false);
    for (int i = 0; i < n; ++i)
        if (!visited[i])
        {
            for (int j = i; j != -1; j = pre[j])
                if (!visited[j])
                {
                    visited[j] = true;
                    vec.push_back(j);
                    on_stack[j] = true;
                }
                else
                {
                    if (on_stack[j]) {
                    	trace(i, n);
                        return true;
                    }
                    break;
                }
            for (int j : vec)
                on_stack[j] = false;
            vec.clear();
        }
    return false;
}

// returns true if there is a negative cycle in the graph
bool spfa_early_terminate(vector<unordered_map<int,int>>& graph)
{
	int n = graph.size();
	std::fill(dis, dis + n, 0);
	std::fill(pre, pre + n, -1);
	std::fill(in_queue, in_queue + n, true);
	std::queue<int> queue;
	for (int i = 0; i < n; ++i)
		queue.push(i);
    int iter = 0;
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue[u] = false;
		for (auto &it : graph[u]) {
			int v = it.first;
			int w = it.second;
			if (dis[u] + w < dis[v])
			{
				pre[v] = u;
				dis[v] = dis[u] + w;
				if (++iter == n)
                {
                    iter = 0;
                    if (detect_cycle(n))
                        return true;
                }
				if (!in_queue[v])
				{
					queue.push(v);
					in_queue[v] = true;
				}
			}
		}
	}
    if (detect_cycle(n))
        return true;
	return false;
}

int main(int argc, char* argv[]) {

	// read name of input and output files
	string infile_name = argv[1];
    string outfile_name = argv[2];

    // open input stream
    ifstream in;
    in.open(infile_name);

    // read number of test cases
    int nofTests;
    in >> nofTests;

    // open output stream
    ofstream out;
    out.open(outfile_name);

    // read and solve every test case
    for (int i = 0; i < nofTests; i++) {
    	// read number of cables
	    int nofCables;
	    in >> nofCables;

	    // create the graph
	    vector<unordered_map<int, int>> graph(nofCables * 2); // <pointing node, weight>

	    // read from input and fill the graph
	    for (int i = 0; i < nofCables; i++) {
	    	for (int j = 0; j < nofCables; j++) {
	    		int weight;
	    		in >> weight;
	    		// ID's of left and right nodes
	    		int left = i;
	    		int right = j + nofCables;
	    		// pair every node with its opposite node initially
	    		// after pairing the edge changes direction (and weight) like a residual graph
	    		if (i == j) {
	    			graph[right][left] = weight;
	    		}
	    		else {
	    			graph[left][right] = (-1) * weight;
	    		}
	    	}
	    }

	    // while there are negative cycles, reverse the edges in the cycle
	    while (spfa_early_terminate(graph)) {
	    	for (int i = 0; i < cycle.size() - 1; i++) {
	    		// reverse
	    		int from = cycle[i];
	    		int to = cycle[i + 1];
	    		int weight = graph[from][to];

	    		graph[from].erase(to);
	    		graph[to][from] = weight * (-1);
	    	}
	    	cycle.clear();
	    }

	    // sum positive weighted edges
	    long long sum = 0;
	    for (auto &adj : graph) {
	    	for (auto &edge : adj) {
	    		if (edge.second > 0) {
	    			sum += edge.second;
	    			break;
	    		}
	    	}
	    }

	    // print the result to the output file
	    out << sum << endl;
    }

    in.close();
    out.close();

    return 0;
}
