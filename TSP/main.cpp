#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <numeric>
#include <utility>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>

using namespace std;

int populationSize = 250;

int RandInt(int r_min = numeric_limits <int> :: min(), int r_max = numeric_limits <int> :: max()) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<> distrib(r_min, r_max);

	return distrib(gen);
}

void RandShuffleVec(vector <int> &vec) {
	// randomly swap vector pairs
	for (int i = 0; i < vec.size(); i++) {
		int a = RandInt(0, vec.size() - 1);
		int b = RandInt(0, vec.size() - 1);

		swap(vec[a], vec[b]);
	}
}

long Distance_2(pair <int, int> a, pair <int, int> b) {
	pair <int, int> diff = make_pair(a.first - b.first, a.second - b.second);

	return diff.first * diff.first + diff.second * diff.second;
}

float Fitness(const vector <int> &chromosome, map <int, pair <int, int>> &cities) {
	float fitness = 0.0f;

	for (int i = 0; i < chromosome.size(); i++) {
		int a = chromosome[i];
		int b = chromosome[(i + 1) % chromosome.size()];
	
		fitness += sqrt(Distance_2(cities[a], cities[b]));
	}

	return fitness;
}

// chromosome mutation
void Mutate(vector <int> &chromosome) {
	// pick two random indices and swap the genes
	int i1 = RandInt(0, chromosome.size() - 1), i2 = RandInt(0, chromosome.size() - 1);

	swap(chromosome[i1], chromosome[i2]);
}

// Partially mapped crossover between two chromosomes
vector <vector <int>> PMXCrossover (const vector <int> &a, const vector <int> &b) {
    assert(a.size() == b.size());
    
    int i1 = RandInt(0, a.size() / 2 - 1), i2 = RandInt(a.size() / 2, a.size() - 1);
    vector <int> child_a(a.size(), -1);
    vector <int> child_b(a.size(), -1);
    
    // swap the genetic information
    for (int i = i1; i <= i2; i++) {
        child_a[i] = b[i];
        child_b[i] = a[i];
    }
	
	// determine mapping relations
	map <int, int> map_a, map_b;
	for (int i = i1; i <= i2; i++) {
        map_a[child_a[i]] = child_b[i];
        map_b[child_b[i]] = child_a[i];
    }

	// legalize offsprings with mapping relations
    for (int i = 0; i < a.size(); i++) {
        if (i == i1) {
            i = i2;
            continue;
        }

        // if there is no mapping, its safe to add it without change
        if (map_a.find(a[i]) == map_a.end()) {
            child_a[i] = a[i];
        }
		// else find the mapped value
		else {
            int j = a[i];
            while (map_a.find(j) != map_a.end()) j = map_a[j];
            child_a[i] = j;
        }

		// repeat for b
		if (map_b.find(b[i]) == map_b.end()) {
            child_b[i] = b[i];
        }
		else {
            int j = b[i];
            while (map_b.find(j) != map_b.end()) j = map_b[j];
            child_b[i] = j;
        }
    }
    
    return vector <vector <int>> {child_a, child_b};
}

void Evolve(vector <vector <int>> &population, map <int, pair <int, int>> &cities) {
	int halfSize = populationSize / 2;

	for (int i = 0; i < halfSize; i += 2) {
		// crossover between two cosecutive pairs
		vector <vector <int>> children = PMXCrossover(population[i], population[i + 1]);
		
		// 10% chance of mutation
		if (RandInt(0, 10) == 1) Mutate(children[0]);
		if (RandInt(0, 10) == 1) Mutate(children[1]);

		// replace old weak population with new offsprings
		population[halfSize + i] = children[0];
		population[halfSize + i + 1] = children[1];
	}

	// 5% chance of mutation for the old generation
	for (int i = 0; i < halfSize; i++) {
		if (RandInt(0, 20) == 1) Mutate(population[i]);
	}

	// sort the population based on fitness
	sort(population.begin(), population.end(), [&](const vector <int> &a, const vector <int> &b) {
		return Fitness(a, cities) < Fitness(b, cities);
	});
}

void printWorld(int worldSize, const vector <int> chromosome, map <int, pair <int, int>> &cities) {
	vector <string> world(worldSize, string(worldSize, '.'));
	int index = 0;
	for (auto &x: chromosome) {
		world[cities[x].first][cities[x].second] = 'A' + index++;
	}

	for (auto &x: world) {
		for (auto &y: x) {
			cout << y << ' ';
		}
		cout << '\n';
	}
}

int main() {
	int worldSize = 30;
	int citiesSize = 20;

	map <int, pair <int, int>> cities;
	for (int i = 0; i < citiesSize; i++) {
		cities[i] = make_pair(RandInt(0, worldSize - 1), RandInt(0, worldSize - 1));
	}

	// generate the first population	
	vector <int> chromosome(citiesSize, -1);
	vector <vector <int>> population;
	for (int i = 0; i < populationSize; i++) {

		for (int j = 0; j < citiesSize; j++) {
			chromosome[j] = j;
		}

		RandShuffleVec(chromosome);
		population.push_back(chromosome);
	}

	sort(population.begin(), population.end(), [&](const vector <int> &a, const vector <int> &b) {
		return Fitness(a, cities) < Fitness(b, cities);
	});

	for (int i = 0; i < 100; i++) {
		Evolve(population, cities);
	}

	printWorld(worldSize, population[0], cities);

	return 0;
}
