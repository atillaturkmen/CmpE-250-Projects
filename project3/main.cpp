#include <fstream>
#include <vector>
#include <list>
#include <queue>

using namespace std;

int main(int argc, char const *argv[]) {

	// read name of input and output files
	string infile_name = argv[1];
    string outfile_name = argv[2];

    // open input stream
    ifstream in;
    in.open(infile_name);

    // read number of vertexes in graph
    int nofVertexes;
    in >> nofVertexes;

    // create the graph that contains adjacency list of every vertex and reserve memory for every vertex
    vector<queue<int>> graph;
    graph.reserve(nofVertexes);

    vector<int> inOutBalance(nofVertexes, 0); // this will help check if graph is eulerian
    int totalNofEdges = 0;                    // total number of edges in graph

    // take info for every vertex
    for (int i = 0; i < nofVertexes; i++) {
        // Add adjacency list of this vertex to the graph
        // Adjecency graph is a queue thus finding first non used edge is O(1) as long as input is sorted
    	graph.push_back(queue<int> ());

        // Read ID and number of edges of this vertex
        // ID is redundant as long as input is sorted
        int vertexID, nofEdges;
        in >> vertexID >> nofEdges;

    	// keep track of total number of edges
    	totalNofEdges += nofEdges;

    	// increment by outdegree, decrease by indegree, graph is eulerian if last result is all zero.
    	inOutBalance[vertexID] += nofEdges;

    	// read all edges for this vertex
    	for (int j = 0; j < nofEdges; j++) {

    		int to;
    		in >> to;

    		graph[vertexID].push(to);

    		inOutBalance[to]--;
    	}
    }

    // read starting vertex
    int startingVertex;
    in >> startingVertex;

    // input taking is over
    in.close();

    // check if graph is eulerian
    bool isEulerian = true;
    for (const int& difference : inOutBalance) {
    	if (difference != 0) {
    		isEulerian = false;
    		break;
    	}
    }

    // Do not run the Hierholzer’s algorithm if graph is not eulerian.
    if (!isEulerian) {
    	ofstream out;
    	out.open(outfile_name);
    	out << "no path";
    	out.close();
    	return 0;
    }

    // Start of Hierholzer’s Algorithm

    list<int> eulerianCircuit;
    eulerianCircuit.push_front(startingVertex);

    int currentVertex = startingVertex;              // holds which vertex the tour is on
    auto connectingVertex = eulerianCircuit.begin(); // tour will merge with eulerian circuit after this vertex

    while (eulerianCircuit.size() <= totalNofEdges) {

    	vector<int> tour;

        // Get first non used edge
    	while (!graph[currentVertex].empty()) {
    		int firstNonUsedEdge = graph[currentVertex].front();
            graph[currentVertex].pop();

            currentVertex = firstNonUsedEdge;
            tour.push_back(currentVertex);
    	}

		// Merge
		eulerianCircuit.insert(++connectingVertex, tour.begin(), tour.end());

		// Find first vertex in the circuit with an unusedEdge
		for (auto it = eulerianCircuit.begin(), end = eulerianCircuit.end(); it != end; ++it) {
			int circuitVertex = *it;
            if (!graph[circuitVertex].empty()) {
                connectingVertex = it;
                currentVertex = circuitVertex;
                break;
            }
		}
    }

    // Print eularian circuit to output file
    ofstream out;
    out.open(outfile_name);

    for (const int& vertex : eulerianCircuit) {
        out << vertex << " ";
    }

    out.close();

	return 0;
}