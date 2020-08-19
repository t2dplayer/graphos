#include <iostream>
#include <vector>
#include <locale>
#include <memory>
#include <limits>
#include <unordered_map>
#include <queue>
#include <list>
#include <functional>

using std::cout, std::cin;

struct Delimiter: std::ctype<char> {
    static mask* make_table(const std::string& ds) {
        static std::vector<mask> table(classic_table(),
                                       classic_table() + table_size);
        for (auto m: table) {
            m &= ~space;
        }
        for (auto d: ds) {
            table[d] |= space;
        }
        return &table[0];
    }
    Delimiter(const std::string& ds, size_t refs = 0):
        ctype(make_table(ds), false, refs) {}
};

struct Node;
using NodePtr = std::shared_ptr<Node>;
using VectorNodePtr = std::vector<NodePtr>;
using ListNodePtr = std::list<NodePtr>;

struct Node {
    std::string label;
    VectorNodePtr adjs;
    NodePtr pred = nullptr;
    int dist = std::numeric_limits<int>::max();
    bool closed = false;
    Node(const std::string label):
        label(label) {}
};

using Graph = std::unordered_map<std::string, NodePtr>;
using WeightMap = std::unordered_map<std::string, std::unordered_map<std::string, int>>;

void addEntry(Graph& g, const std::string label) {
    if (g.find(label) == g.end()) {
        g[label] = std::make_shared<Node>(label);
    }
}

using Cmp = std::function<bool(NodePtr a, NodePtr b)>;
using PQueueNodePtr = std::priority_queue<NodePtr, VectorNodePtr, Cmp>;

ListNodePtr dijkstra(Graph& g,
                     WeightMap& wm,
                     const std::string& src,
                     const std::string& dst) {
    Cmp cmp = [](NodePtr a, NodePtr b) {
        return a->dist > b->dist;
    };
    PQueueNodePtr q(cmp);
    g[src]->dist = 0;
    q.push(g[src]);
    while (!q.empty()) {
        NodePtr u = q.top(); q.pop();
        for (const auto& v: u->adjs) {
            if (u->dist + wm[u->label][v->label] < v->dist) {
                v->dist = u->dist + wm[u->label][v->label];
                v->pred = u;
                q.push(v);
            }
        }
    }
    ListNodePtr result;
    NodePtr aux = g[dst];
    while (aux != nullptr) {
        result.push_front(aux);
        aux = aux->pred;
    }
    return result;
}


int main(int argc, char** argv) {
    cin.imbue(std::locale(cin.getloc(), new Delimiter(":")));
    Graph g; WeightMap wm;
    std::string src, dst;
    int weight;
    std::string from, to;
    cin >> from >> to;
    while (cin >> src >> weight >> dst) {
        cout << src << dst << " = " << weight;
        addEntry(g, src);
        addEntry(g, dst);
        g[src]->adjs.push_back(g[dst]);
        wm[src][dst] = weight;
    }
    ListNodePtr path = dijkstra(g, wm, from, to);
    size_t i = 0;
    for (const auto& p: path) {
        cout << p->label;
        if (i < path.size() - 1) cout << ", ";
    }
    cout << " = " << g[dst]->dist << "\n";
    return 0;
}
