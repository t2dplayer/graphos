#include <iostream>
#include <array>
#include <vector>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <limits>
#include <charconv>

using std::cout;

using Input = std::array<std::string_view, 9>;
using VectorStrView = std::vector<std::string_view>;
using MatrixWeight = std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>>;

struct Node;

struct Aux {
    std::shared_ptr<Node> pred = nullptr;
    int dist = std::numeric_limits<int>::max();
    bool visited = false;
};

struct Node {
    std::string_view label;
    Aux aux;
    std::vector<std::shared_ptr<Node>> adj;
    Node(const std::string_view& label):
        label(label){}
};

using NodePtr = std::shared_ptr<Node>;
using NodesMap = std::unordered_map<std::string_view, NodePtr>;
using NodesPtrVec = std::vector<NodePtr>;

std::ostream& operator<<(std::ostream& os, const Aux& aux) {
    os << "("
       << "pred:" << (aux.pred ? aux.pred->label :  "null") << ", "
       << "dist:" << aux.dist << ", "
       << "visi:" << aux.visited
       << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NodesPtrVec& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i]->label;
        if (i < v.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NodePtr& ptr) {
    os << "  " << ptr->aux
       << " - ("
       << ptr->label
       << ")->"
       << ptr->adj << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const NodesMap& m) {
    os << "{\n";
    for (auto [key, value]: m) {
        os << value;
    }
    os << "}";
    return os;
}

void addNode(NodesMap& nodes,
             const std::string_view& label) {
    if (nodes.find(label) == nodes.end()) {
        nodes[label] = std::make_shared<Node>(label);
    }
}

VectorStrView split(const std::string_view& str,
                    char d = ' ') {
    VectorStrView result;
    result.reserve(str.size());
    int s = 0, e = str.find(d);
    while (e != std::string::npos) {
        result.push_back(str.substr(s, e - s));
        s = e + 1;
        e = str.find(d, s);
    }
    result.push_back(str.substr(s, e - s));
    return result;
}

bool isAllVisited(NodesMap& nodes) {
    for (auto [key, value]: nodes) {
        if (!value->aux.visited) return false;
    }
    return true;
}

NodePtr minDistance(NodesMap& nodes) {
    NodePtr result = nullptr;
    int m = std::numeric_limits<int>::max();
    for (auto [key, value]: nodes) {
        if (value->aux.visited) continue;
        if (value->aux.dist < m) {
            m = value->aux.dist;
            result = value;
        }
    }
    return result;
}

VectorStrView dijkstra(NodesMap& nodes,
                       MatrixWeight& mw,
                       const std::string_view& src,
                       const std::string_view& dst) {
    VectorStrView result;
    nodes[src]->aux.dist = 0;
    while (!isAllVisited(nodes)) {
        NodePtr u = minDistance(nodes);
        u->aux.visited = true;
        for (auto v: u->adj) {
            if (v->aux.visited) continue;
            if (u->aux.dist + mw[u->label][v->label] < v->aux.dist) {
                v->aux.dist = u->aux.dist + mw[u->label][v->label];
                v->aux.pred = u;
            }
        }
    }
    return result;
}

int main() {
    Input input = {
        "node0:10:node1",
        "node0:5:node2",
        "node1:1:node3",
        "node2:3:node1",
        "node2:8:node3",
        "node2:2:node4",
        "node3:4:node5",
        "node3:4:node4",
        "node4:6:node5"
    };
    NodesMap nodes;
    MatrixWeight mw;
    for (auto i: input) {
        VectorStrView arr = split(i, ':');
        addNode(nodes, arr[0]);
        addNode(nodes, arr[2]);
        nodes[arr[0]]->adj.push_back(nodes[arr[2]]);
        int weight;
        std::from_chars(arr[1].data(), arr[1].data() + arr[1].size(), weight);
        mw[arr[0]][arr[2]] = weight;
    }
    dijkstra(nodes, mw, "node0", "node5");
    cout << nodes << '\n';

    return 0;
}
