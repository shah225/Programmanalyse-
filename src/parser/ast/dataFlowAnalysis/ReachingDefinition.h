#pragma once
#include "../ControlFlowGraph.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>

struct VarDef {
    std::string var;
    int nodeId;
    bool operator==(const VarDef &o) const {
        return var == o.var && nodeId == o.nodeId;
    } //gleich wenn name & auftreten in node gleich
};

struct VarDefHash {
    std::size_t operator()(const VarDef &d) const noexcept {
        return std::hash<std::string>{}(d.var) ^ (std::hash<int>{}(d.nodeId) << 1);
    }
};

class ReachingDefinitions {
public:
    explicit ReachingDefinitions(const ControlFlowGraph &cfg) : cfg(cfg) {
        computeGenKill(); //gen: entstehende definitonen; kill: ungültig machen der def
        runWorklist(); //berechnung der errecihbaren def
    }

    const std::unordered_set<VarDef, VarDefHash>& inSet(int nodeId) const {
        return in.at(nodeId);
    } //eingehende gültige vars

    const std::unordered_set<VarDef, VarDefHash>& outSet(int nodeId) const {
        return out.at(nodeId);
    } //ausgehende gültige vars

    void dump(std::ostream &os) const {
        for (auto &n : cfg.getNodes()) {
            os << "Node " << n.id << " (" << n.label << ")\n";
            os << "  IN : " << setToString(in.at(n.id)) << "\n";
            os << "  OUT: " << setToString(out.at(n.id)) << "\n";
        } //für graphviz
    }

private:
    const ControlFlowGraph &cfg;
    std::unordered_map<int, std::unordered_set<VarDef, VarDefHash>> gen, kill, in, out;

    void computeGenKill() {
        for (auto &n : cfg.getNodes()) {
            std::string label = n.label;
            if (label.rfind("assign:", 0) == 0) { //nur aussign relevant
                auto pos = label.find(":=");
                std::string var = label.substr(8, pos - 9); //assign := //pos überspringen
                VarDef def{var, n.id}; //neues vardef objekt mit var und node id
                gen[n.id].insert(def); //wird teil der gen menge

                for (auto &m : cfg.getNodes()) {
                    if (m.id != n.id && m.label.rfind("assign:", 0) == 0) {
                        auto pos2 = m.label.find(":=");
                        std::string var2 = m.label.substr(8, pos2 - 9);
                        if (var2 == var) { //var def bereits vorhanden
                            kill[n.id].insert({var2, m.id}); //überschreiben = alte def in  kill menge
                        }
                    }
                }
            }
        }
    }

    void runWorklist() {
        for (auto &n : cfg.getNodes()) {
            in[n.id] = {};
            out[n.id] = gen[n.id]; //selbst setzende def
        } //in & out menge setzen

        std::queue<int> wl;
        for (auto &n : cfg.getNodes()) wl.push(n.id); //to-do list der knoten

      while (!wl.empty()) {
        int nid = wl.front(); wl.pop();

        //neue in menge berechnen
        std::unordered_set<VarDef, VarDefHash> newIn;
        for (auto &e : cfg.getEdges()) {
          if (e.to == nid) { //für jede edge: wenn die to id gleih node id
            newIn.insert(out[e.from].begin(), out[e.from].end());
          }
        }

        in[nid] = newIn; //vereinigung aller out der vorherigen knoten

        std::unordered_set<VarDef, VarDefHash> newOut = gen[nid];
        for (auto &d : newIn) {
          if (kill[nid].count(d) == 0) newOut.insert(d);
        } //gen und kill def als newout

        if (newOut != out[nid]) {
          out[nid] = newOut;
          for (auto &e : cfg.getEdges()) {
            if (e.from == nid) wl.push(e.to);
          } //worklist erneut hinzufügen bei änderung
        }
      }

    }

    static std::string setToString(const std::unordered_set<VarDef, VarDefHash> &s) {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (auto &d : s) {
            if (!first) ss << ", ";
            ss << d.var << "@" << d.nodeId;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
};
