#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
public:
  const std::string name;
  const bool isSmall;
  std::vector<Node *> adjacent;

public:
  Node(const std::string &name) : name(name), isSmall(name[0] > 'Z') {}
};

class Path {
private:
  std::vector<Node *> nodes;

public:
  Path() = default;
  Path(Node *start) { nodes.push_back(start); }

  void addNode(Node *waypoint) { nodes.push_back(waypoint); }

  bool containsNode(const Node *nodeToFind) const {
    for (const auto node : nodes) {
      if (node == nodeToFind) {
        return true;
      }
    }
    return false;
  }

  void print() const {
    bool first = true;
    for (const auto node : nodes) {
      if (!first) {
        std::cout << ",";
      }
      std::cout << node->name;
      first = false;
    }
    std::cout << "\n";
  }
};

class Graph {
private:
  Node *start;
  Node *end;
  std::unordered_map<std::string, Node *> nodeList;

public:
  ~Graph() {
    for (auto node : nodeList) {
      delete node.second;
    }
    nodeList.clear();
  }

  Node *getNode(const std::string &nodeName) {
    auto lookup = nodeList.find(nodeName);
    if (lookup == nodeList.end()) {
      auto node = new Node(nodeName);
      nodeList[nodeName] = node;
      if (nodeName == "start") {
        start = node;
      }
      if (nodeName == "end") {
        end = node;
      }
      return node;
    } else {
      return lookup->second;
    }
  }

  void addEdge(const std::string &nodeName1, const std::string &nodeName2) {
    Node *node1 = getNode(nodeName1);
    Node *node2 = getNode(nodeName2);
    node1->adjacent.push_back(node2);
    node2->adjacent.push_back(node1);
  }

private:
  uint32_t walkPathRecurse(Path path, Node *walkFrom) const {
    if (walkFrom->name == "end") {
      path.addNode(walkFrom);
      path.print();
      return 1;
    }
    path.addNode(walkFrom);
    uint32_t walkedPaths{0};
    for (const auto nextNode : walkFrom->adjacent) {
      // TODO allow big nodes
      if (!nextNode->isSmall || !path.containsNode(nextNode)) {
        walkedPaths += walkPathRecurse(path, nextNode);
      }
    }
    return walkedPaths;
  }

public:
  uint32_t walkPaths() const {
    Path p;
    return walkPathRecurse(p, start);
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <caves.txt>\n";
    std::exit(EXIT_FAILURE);
  }

  Graph g;

  std::ifstream infile{argv[1]};
  std::string line;
  while (infile >> line) {
    const auto seperatorPos = line.find('-');
    g.addEdge(line.substr(0, seperatorPos), line.substr(seperatorPos + 1));
  }

  const auto number = g.walkPaths();
  std::cout << number << " paths total\n";
}
