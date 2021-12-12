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

  uint32_t containsNodeCount(const Node *nodeToFind) const {
    uint32_t count{0};
    for (const auto node : nodes) {
      if (node == nodeToFind) {
        ++count;
      }
    }
    return count;
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
      if (!nextNode->isSmall || path.containsNodeCount(nextNode) == 0) {
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

private:
  uint32_t walkPath2Recurse(Path path, Node *walkFrom,
                            const bool visitedTwice) const {
    if (walkFrom->name == "end") {
      path.addNode(walkFrom);
      path.print();
      return 1;
    }
    path.addNode(walkFrom);
    uint32_t walkedPaths{0};
    for (const auto nextNode : walkFrom->adjacent) {
      const auto nextNodeCountInPath = path.containsNodeCount(nextNode);
      if (!nextNode->isSmall || nextNodeCountInPath < (visitedTwice ? 1 : 2)) {
        if (nextNode->name != "start") {
          const bool mayVisitTwice =
              visitedTwice || (nextNode->isSmall && nextNodeCountInPath == 1);
          walkedPaths += walkPath2Recurse(path, nextNode, mayVisitTwice);
        }
      }
    }
    return walkedPaths;
  }

public:
  uint32_t walkPaths2() const {
    Path p;
    return walkPath2Recurse(p, start, false);
  }

public:
  void bucketDump () const {
    // dump std::unordered_map nodeList
    std::cout << "The graph contains " << nodeList.bucket_count() << " buckets.\n";
    for(size_t i = 0; i < nodeList.bucket_count(); ++i) {
      std::cout << "Bucket " << i << ": ";
      bool first=true;
      for(auto nodeIter = nodeList.begin(i); nodeIter != nodeList.end(i); ++nodeIter) {
        if(!first){std::cout<<", ";}
        std::cout<<(*nodeIter).second->name;
        first=false;
      }
      std::cout<<"\n";
    }
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

  std::cout << "-------------------------------------------------\n";
  const auto number = g.walkPaths();
  std::cout << "-------------------------------------------------\n";
  const auto numberTwice = g.walkPaths2();
  std::cout << "-------------------------------------------------\n";
  g.bucketDump();
  std::cout << "-------------------------------------------------\n";

  std::cout << number << " paths total in part 1\n";
  std::cout << numberTwice << " paths total in part 2\n";
}
