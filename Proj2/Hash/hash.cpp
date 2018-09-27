#include <iostream>
#include <vector>

const int NUM_BUCKETS = 1000;

int hash(std::string str) {
	int seed = 0;
	// makes hash likely to be different for strings with differing chars,
	// and guaranteed to be different for strings with 1 char.
	for (size_t i = 0; i < str.length(); i++) {
		seed += str[i];
		seed *= str[i]; // helps spread distribution
	}
	// allows strings that are permutations of one another to be different.
	// ex. abc, cba, bac
	for (size_t i = 0; i < str.length(); i++) {
		seed += (i+1) ^ str[i];
	}
	if (seed < 0) {
		seed = seed + 1000;
	}
	return seed % 1000;
}

int main() {
	std::string input = "";
	int list[1000] = {0};
	int entries = 0;
	while (!std::cin.eof()) {
		std::cin >> input;
		inputs.push_back(input);
		entries += 1;
		int hsh = hash(input);
		list[hsh] += 1;
		//std::cout << hsh << std::endl;
	}
	float collisions = 0;
	int sumOfIndices = 0;
	int sumOfIndicesWeighted = 0;
	int max = 0;
	int maxEntries = 0;
	for (size_t i = 0; i < NUM_BUCKETS; i++) {
		if (list[i] > 1) {
			collisions += 1;
		}
		if (list[i] > 0) {
			sumOfIndices += i;
			sumOfIndicesWeighted += (i * list[i]);
		}
		if (list[i] > max) {
			max = i;
			maxEntries = list[i];
		}
	}

	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Collisions: " << (collisions/entries)*100 << " percent" << std::endl;
	std::cout << "Number of collisions: " << collisions << std::endl;
	std::cout << "Number of Entries: " << entries << std::endl;
	std::cout << "Entry location mean (unweighted): " << (float)sumOfIndices/(float)entries << std::endl;
	std::cout << "Entry location mean (weighted): " << (float)sumOfIndicesWeighted/(float)entries << std::endl;
	std::cout << "Maximally filled bucket: " << max << std::endl;
	std::cout << "With: " << maxEntries << " Entries" << std::endl;
	std::cout << "Number of matching strings: " << sameString << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	return 0;
}
