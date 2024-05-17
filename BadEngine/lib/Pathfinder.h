#ifndef Pathfinder_H
#define Pathfinder_H

#include <vector>

class Pathfinder {
public:
	std::vector<std::pair<int,glm::vec2>> trees;
	void sortTrees() {
		for (int i = 0; i< trees.size(); i++) {
			for (int j = 0; j < trees.size(); j++) {
				if (glm::min(trees[i].second,trees[j].second) == trees[j].second) {
					std::pair<int, glm::vec2> temp = trees[i];
					trees[i] = trees[j];
					trees[j] = temp;
				}
			}
		}
	}

	glm::vec3 decideInitalDestination(int sector) {
		std::vector<glm::vec2> treePositions;
		for (auto tree : trees) {
			if (tree.first == sector) {
				treePositions.push_back(tree.second);
			}
		}
		if (treePositions.size() == 0) {
			return glm::vec3(0, 0, 0);
		}
		int treeIndex = rand() % treePositions.size();
		glm::vec2 treePosition = treePositions[treeIndex];
		return glm::vec3(treePosition.x, 0, treePosition.y);
	
	}

	glm::vec3 decideDestination(glm::vec3 chosenTreePos, glm::vec3 position, int sector) {
		std::vector<glm::vec2> treePositions;
		for (auto tree : trees) {
			if (tree.first == sector) {
				treePositions.push_back(tree.second);
			}
		}
		if (trees.size() < 3) {
			return chosenTreePos;
		}
		glm::vec2 closestTree;
		glm::vec2 secondClosestTree;
		/*for (auto tree : treePositions) {
			if (glm::distance(tree, glm::vec2(position.x, position.z)) < glm::distance(closestTree, glm::vec2(position.x, position.z))) {
				secondClosestTree = closestTree;
				closestTree = tree;
			}
			else if (glm::distance(tree, glm::vec2(position.x, position.z)) < glm::distance(secondClosestTree, glm::vec2(position.x, position.z))) {
				secondClosestTree = tree;
			}
		}*/
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