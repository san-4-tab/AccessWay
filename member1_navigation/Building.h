
#pragma once

#include "../common/AccessPulseData.h"

class BuildingManager {

private:

    struct BuildingNode {
        Building data;
        BuildingNode* left;
        BuildingNode* right;
    };

    BuildingNode* root;

    BuildingNode* insert(BuildingNode* node, Building building);
    BuildingNode* search(BuildingNode* node, int id);
    void inorder(BuildingNode* node);
    BuildingNode* findMin(BuildingNode* node);
    BuildingNode* deleteNode(BuildingNode* node, int id);

    void destroyTree(BuildingNode* node);

public:

    BuildingManager();
    ~BuildingManager();

    void addBuilding(Building building);
    void searchBuilding(int id);
    void displayBuildings();
    void deleteBuilding(int id);
};
