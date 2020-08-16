#include <iostream>
#include <locale>
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <limits>
#include <functional>
#include <list>

using std::cout, std::cin, std::locale;

struct Delimiter: std::ctype<char> {
    static const mask* make_table(std::string ds) {
        static std::vector<mask> v(classic_table(),
                                   classic_table() + table_size);
        for (auto mask: v) {
            mask &= ~space;
        }
        for (auto d: ds) {
            v[d] |= space;
        }
        return &v[0];
    }
    Delimiter(std::string ds, size_t refs = 0):
        ctype(make_table(ds), false, refs) {}
};

struct Node;
using NodePtr = std::shared_ptr<Node>;
struct Aux {
    NodePtr prev = nullptr;
    int dist = std::numeric_limits<int>::max();
    bool visited = false;
};

struct Node {
    std::string label;
    Aux aux;
    std::vector<NodePtr> adj;
    Node(std::string label):
        label(label){}
};

std::ostream& operator<<(std::ostream& os, const Aux& aux) {
    os << "(pred:" << (aux.prev ? aux.prev->label : "none")
       << ", dist:" << aux.dist
       << ", visi:" << aux.visited
       << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NodePtr n) {
    os << "  ("
       << n->label
       << ")[";
    for (size_t i = 0; i < n->adj.size(); ++i) {
        os << n->adj[i]->label;
        if (i < n->adj.size() - 1) os << ", ";
    }
    os<< "]" << n->aux << "\n";
    return os;
}

using NodeMap = std::unordered_map<std::string, NodePtr>;

void addEntry(NodeMap& nodes,
              const std::string& label) {
    if (nodes.find(label) == nodes.end()) {
        nodes[label] = std::make_shared<Node>(label);
    }
}

std::ostream& operator<<(std::ostream& os, const NodeMap& m) {
    os << "{\n";
    for (auto [key, value]: m) {
        os << value;
    }
    os << "}";
    return os;
}

using Cmp = std::function<bool(NodePtr, NodePtr)>;
using QNodePtr = std::priority_queue<NodePtr, std::vector<NodePtr>, Cmp>;
using NodeMatrix = std::unordered_map<std::string, std::unordered_map<std::string, int>>;

std::list<NodePtr> dijkstra(NodeMap& nodes,
                              NodeMatrix& weights,
                              const std::string& src,
                              const std::string& dst) {
    Cmp cmp = [](NodePtr a, NodePtr b) {
        return a->aux.dist > b->aux.dist;
    };
    QNodePtr q(cmp);
    std::list<NodePtr> result;
    nodes[src]->aux.dist = 0;
    q.push(nodes[src]);
    while (!q.empty()) {
        NodePtr u = q.top(); q.pop();
        u->aux.visited = true;
//        cout << "u:" << u << "\n";
        for (auto v: u->adj) {
            if (v->aux.visited) continue;
            if (u->aux.dist + weights[u->label][v->label] < v->aux.dist) {
                v->aux.dist = u->aux.dist + weights[u->label][v->label];
                v->aux.prev = u;
                q.push(v);
//                cout << "relax:\n" << u << v << "\n";
            }
        }
    }
    NodePtr aux = nodes[dst];
    while (aux != nullptr) {
        result.push_front(aux);
        aux = aux->aux.prev;

    }
    return result;
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin.imbue(locale(cin.getloc(), new Delimiter(":")));
    std::string src, dst;
    int weight;
    NodeMap nodes;
    NodeMatrix weights;
    std::string s, d;
    cin >> s >> d;
    while (cin >> src >> weight >> dst) {
        addEntry(nodes, src);
        addEntry(nodes, dst);
        nodes[src]->adj.push_back(nodes[dst]);
        weights[src][dst] = weight;
    }
    std::list<NodePtr> path = dijkstra(nodes, weights, s, d);
    size_t i = 0;
    for (auto n: path) {
        cout << n->label;
        if (i++ < path.size() - 1) cout << "->";
    }
    cout << " = " << nodes[dst]->aux.dist << '\n';
//    cout << nodes << '\n';
    return 0;
}
