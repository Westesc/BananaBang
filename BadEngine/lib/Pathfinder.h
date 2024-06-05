#ifndef Pathfinder_H
#define Pathfinder_H

#include <vector>
#include "Tree.h"

class Pathfinder {
public:
	std::vector<std::pair<int,Tree*>> trees;
	void sortTrees() {
		ZoneScopedN("sortTrees");
		for (int i = 0; i< trees.size(); i++) {
			for (int j = 0; j < trees.size(); j++) {
				if (glm::min(glm::vec2(trees[i].second->getTransform()->localPosition.x, trees[i].second->getTransform()->localPosition.z), 
					glm::vec2(trees[j].second->getTransform()->localPosition.x, trees[j].second->getTransform()->localPosition.z)) == glm::vec2(trees[j].second->getTransform()->localPosition.x, trees[j].second->getTransform()->localPosition.z)) {
					std::pair<int, Tree*> temp = trees[i];
					trees[i] = trees[j];
					trees[j] = temp;
				}
			}
		}
	}

	std::pair<glm::vec3, Tree*> decideInitalDestination(int sector) {
		std::vector<Tree*> treesInSector;
		for (auto tree : trees) {
			if (tree.first == sector) {
				treesInSector.push_back(tree.second);
			}
		}
		if (treesInSector.size() == 0) {
			return std::make_pair<glm::vec3, Tree*>(glm::vec3(0, 0, 0), nullptr);
		}
		int treeIndex = rand() % treesInSector.size();
		//glm::vec3 treePosition = glm::vec3(treesInSector[treeIndex]->getTransform()->localPosition.x, 0 ,treesInSector[treeIndex]->getTransform()->localPosition.z);
		Tree* tree = treesInSector[treeIndex];
		return std::make_pair(glm::vec3(tree->getTransform()->localPosition.x, 0, tree->getTransform()->localPosition.z), tree);
	
	}

	glm::vec3 decideDestination(glm::vec3 chosenTreePos, glm::vec3 position, int sector) {
		ZoneScopedN("decideDestination");
		std::vector<glm::vec2> treePositions;
		for (auto tree : trees) {
			if (tree.first == sector) {
				treePositions.push_back(glm::vec2(tree.second->getTransform()->localPosition.x, tree.second->getTransform()->localPosition.z));
			}
		}
		if (trees.size() < 3) {
			return chosenTreePos;
		}
		glm::vec2 closestTree;
		glm::vec2 secondClosestTree;
		int treeIndex = findTreeByPosition(chosenTreePos, treePositions);
		if (treeIndex == -1) {
			return glm::vec3(0, 0, 0);
		}
		else if (treeIndex == treePositions.size() - 1) {
			closestTree = treePositions[treeIndex - 1];
			secondClosestTree = treePositions[treeIndex - 2];
		}
		else if (treeIndex == 0) {
			closestTree = treePositions[treeIndex + 1];
			secondClosestTree = treePositions[treeIndex + 2];
		}
		else {
			closestTree = treePositions[treeIndex - 1];
			secondClosestTree = treePositions[treeIndex + 1];
		}
		glm::vec3 middleof3 = glm::vec3((chosenTreePos.x + closestTree.x + secondClosestTree.x) / 3.f, 0.f, (chosenTreePos.y + closestTree.y + secondClosestTree.y) / 3.f);
		if (glm::distance(position, chosenTreePos) <= glm::distance(position, middleof3) || glm::distance(position, middleof3) < 5.f) {
			return chosenTreePos;
		}
		else {
			return middleof3;
		}
	}

	int findTreeByPosition(glm::vec3 position, std::vector<glm::vec2> vector) {
		for (int i = 0; i < vector.size(); i++) {
			if (glm::distance(glm::vec2(position.x, position.z), vector[i]) < 0.1f) {
				return i;
			}
		}
		return -1;
	}
};
#endif