#pragma once
#include <stack>
#define NUM_NODES 4
#define NODESIZE 70


struct nodeContainer : public Component {
    nodeContainer(int numNodes) {
        allNodes.ensureStorageAllocated(NUM_NODES);
        allCons.ensureStorageAllocated(NUM_NODES * NUM_NODES);
        for (int i = NUM_NODES; i > 0; i--) {
            availableNodes.push(i);
        }
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::cadetblue);
        for (auto& con : allCons) {
            g.fillPath(*con->getPath());
        }
    }

    void resized() override
    {
        for (auto node : allNodes) {
            node->setBounds(std::min(getParentWidth() - NODESIZE, node->getX()), 
                std::min(getParentHeight() - NODESIZE, node->getY()), 
                NODESIZE, NODESIZE);
        }
    }

    int addNode(ComponentListener* cL, MouseListener* mL, int x, int y) {
        if (availableNodes.empty()) return -1;
        int id = availableNodes.top();
        availableNodes.pop();
        addAndMakeVisible(allNodes.add(new CPGNode(id, x, y)));
        CPGNode* node = allNodes.getLast();
        node->addComponentListener(cL);
        node->setAlwaysOnTop(true);
        node->addMouseListener(mL, false);
        resized();
        return id;
    }

    CPGConnection* addConn(CPGNode* from, CPGNode* to) {
        if (from == to) return nullptr;
        for (auto con : allCons) {
            /*If connection exists going either way*/
            if ((con->getParent() == from &&
                con->getConnected() == to) ||
                (con->getParent() == to &&
                con->getConnected() == from)) {
                return nullptr;
            }
        }
        CPGConnection* con = allCons.add(new CPGConnection(from, to));
        return con;
    }

    CPGNode* clickedNode = nullptr;
    CPGConnection* clickedConnection = nullptr;
    std::stack<int> availableNodes;
    OwnedArray<CPGNode> allNodes;
    OwnedArray<CPGConnection> allCons;
};