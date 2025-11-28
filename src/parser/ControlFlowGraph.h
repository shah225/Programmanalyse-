#pragma once

#include <iostream>
#include <vector>
#include <string>

struct CFGNode{
  int id;
  std::string label;
};

struct CFGEdge {
  int from;
  int to;
  std::string action;
};


class ControlFlowGraph{
public:
  int addNode(std::string label) {
    int id = static_cast<int>(nodes.size());
    nodes.push_back(CFGNode{id, label});
    return id;
  }

  void addEdge(int fromId, int toId, std::string label) {
    edges.push_back(CFGEdge{fromId, toId, label});
  }

  void setEntry(int id) {entryId = id;}
  void setExit(int id) {exitId = id;}

  int getEntry() {return entryId;}
  int getExit() {return exitId;}

  void dumpDot(std::ostream &out) const {
    out << "digraph CFG {\n";
    for (auto &n : nodes) {
      out << "  " << n.id << " [label=\"" << n.label << "\"];\n";
    }
    for (auto &e : edges) {
      out << "  " << e.from << " -> " << e.to
          << " [label=\"" << e.action << "\"];\n";
    }
    out << "}\n";
  }

  const std::vector<CFGNode>& getNodes() const {return nodes;}
  const std::vector<CFGEdge>& getEdges() const {return edges;}

private:
  std::vector<CFGNode> nodes;
  std::vector<CFGEdge> edges;
  int entryId;
  int exitId;
};