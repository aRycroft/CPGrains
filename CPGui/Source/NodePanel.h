#pragma once
#include <stack>
#define NUM_NODES 4


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
        for (auto con : allCons) {
            g.strokePath(con->getPath(), PathStrokeType(2));
        }
    }

    void resized() override
    {
        //int nodeSize = std::min(proportionOfHeight(0.12), proportionOfWidth(0.12));
        int nodeSize = 70.0f;
        for (auto node : allNodes) {
            node->setBounds(std::min(getParentWidth() - nodeSize, node->getX()), std::min(getParentHeight() - nodeSize, node->getY()), nodeSize, nodeSize);
        }
        for (auto con : allCons) {
            con->recalculatePath();
        }
    }

    CPGNode* clickedNode = nullptr;
    CPGConnection* clickedConnection = nullptr;
    std::stack<int> availableNodes;
    OwnedArray<CPGNode> allNodes;
    OwnedArray<CPGConnection> allCons;
};