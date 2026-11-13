
#include <iostream>
#include "Building.h"

using namespace std;

// Constructor
BuildingManager::BuildingManager() {
    root = nullptr;
}

// Insert function
BuildingManager::BuildingNode* BuildingManager::insert(
    BuildingNode* node, Building building) {

    if (node == nullptr) {
        BuildingNode* newNode = new BuildingNode;
        newNode->data = building;
        newNode->left = nullptr;
        newNode->right = nullptr;
        return newNode;
    }

    if (building.buildingId < node->data.buildingId) {
        node->left = insert(node->left, building);
    }
    else if (building.buildingId > node->data.buildingId) {
        node->right = insert(node->right, building);
    }
    else {
        cout << "Building ID already exists!\n";
    }

    return node;
}

// Add building
void BuildingManager::addBuilding(Building building) {
    root = insert(root, building);
}

// Search function
BuildingManager::BuildingNode* BuildingManager::search(
    BuildingNode* node, int id) {

    if (node == nullptr || node->data.buildingId == id) {
        return node;
    }

    if (id < node->data.buildingId) {
        return search(node->left, id);
    }

    return search(node->right, id);
}

// Search building
void BuildingManager::searchBuilding(int id) {
    BuildingNode* result = search(root, id);

    if (result == nullptr) {
        cout << "Building not found!\n";
    }
    else {
        cout << "Building ID: " << result->data.buildingId << endl;
        cout << "Building Name: " << result->data.buildingName << endl;
        cout << "Total Floors: " << result->data.totalFloors << endl;
    }
}

// Inorder traversal
void BuildingManager::inorder(BuildingNode* node) {
    if (node == nullptr) {
        return;
    }

    inorder(node->left);

    cout << node->data.buildingId << " "
         << node->data.buildingName << " "
         << node->data.totalFloors << endl;

    inorder(node->right);
}

// Display all buildings
void BuildingManager::displayBuildings() {
    if (root == nullptr) {
        cout << "No buildings registered!\n";
        return;
    }

    cout << "\nRegistered Buildings:\n";
    inorder(root);
}

// Find minimum node
BuildingManager::BuildingNode* BuildingManager::findMin(
    BuildingNode* node) {

    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }

    return node;
}

// Delete function
BuildingManager::BuildingNode* BuildingManager::deleteNode(
    BuildingNode* node, int id) {

    if (node == nullptr) {
        return nullptr;
    }

    if (id < node->data.buildingId) {
        node->left = deleteNode(node->left, id);
    }
    else if (id > node->data.buildingId) {
        node->right = deleteNode(node->right, id);
    }
    else {

        // Case 1: No child
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }

        // Case 2: One child
        else if (node->left == nullptr) {
            BuildingNode* temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr) {
            BuildingNode* temp = node->left;
            delete node;
            return temp;
        }

        // Case 3: Two children
        BuildingNode* temp = findMin(node->right);

        node->data = temp->data;

        node->right = deleteNode(
            node->right, temp->data.buildingId);
    }

    return node;
}

// Delete building
void BuildingManager::deleteBuilding(int id) {
    if (search(root, id) == nullptr) {
        cout << "Building not found!\n";
        return;
    }

    root = deleteNode(root, id);
    cout << "Building deleted successfully!\n";
}
void BuildingManager::destroyTree(BuildingNode* node) {

    if (node == nullptr) {
        return;
    }

    destroyTree(node->left);
    destroyTree(node->right);

    delete node;
}

BuildingManager::~BuildingManager() {
    destroyTree(root);
}
