#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

int populationSize = 500; // keep this an even number

int RandInt(int r_min = numeric_limits <int>::min(), int r_max = numeric_limits <int>::max()) {
	static random_device rd;
	static mt19937 gen(rd());

	uniform_int_distribution<> distr(r_min, r_max);

	int res =  distr(gen);
	return res; 
}

int fitness(int num, int target) {
	int score = abs(abs(target) - abs(num));
	
	// add a penalty for being the wrong sign
	bool t_sign = (target >= 0);
	bool n_sign = (num >= 0);

	if (t_sign != n_sign) score += 100000;

	if (score < 0) score = numeric_limits <int> :: max();

	return score;
}

// one point crossover between two genes
vector <int> crossover(unsigned int a, unsigned int b) {
	// do not index the extreme bits to ensure diversity
	unsigned int index = RandInt(4, 8 * sizeof(int) - 4);
	unsigned int index2 = (8u * sizeof(int) - index);

	return vector <int> {
		static_cast <int> (((a >> index2) << index2) | ((b << index) >> index)),
		static_cast <int> (((b >> index2) << index2) | ((a << index) >> index))
	};
}

// mutates nth bit from the end
int mutate(unsigned int a) {
	unsigned int index = RandInt(0, 8 * sizeof(int) - 1);

	return static_cast <int> (a ^ (1 << index));
}

// replace the entire gene around a certain index
int geneReplace(unsigned int a) {
	unsigned int index = RandInt(1, 8 * sizeof(int) - 2);
	unsigned int uMax = numeric_limits <unsigned int> :: max();

    int res;
    if ((int)a >= 0) {
        res = static_cast <int> (
            a & (~(uMax << index))
        );
    }
    else {
        res = static_cast <int> (
            a & (~(uMax >> index))
        );    
    }
    
    return res;
}

// evolve the old population to a  new one
void newGeneration(vector <int> &old, int target) {
	// sort the old population by fitness value
	sort(old.begin(), old.end(), [&](int a, int b) {
		return fitness(a, target) < fitness(b, target);
	});

	int pHalf = populationSize / 2;

	// specific mutation
	// flip the sign of the chromosome if that results into a better score
	for (int i = 0; i < pHalf / 2; i++) {
		if (fitness(old[i], target) > fitness(-old[i], target)) old[i] = -old[i];
	}

	// mutate the second half of the old generations
	for (int i = pHalf / 2; i < pHalf; i ++) {
		// 5% chance of mutation
		if (RandInt(0, 20) == 1) old[i] = mutate(old[i]);
	}

	// only keep the first 50 of the population and add two children per consecutive pair
	int j = pHalf;
	for (int i = 0; i < pHalf; i += 2) {
		// cross each chromosome with the next one
		vector <int> children = crossover(old[i], old[RandInt(0, pHalf)]);

		// 10% chance of mutation
		if (RandInt(0, 10) == 1) children[0] = mutate(children[0]);
		if (RandInt(0, 10) == 1) children[1] = mutate(children[1]);

		// 5% chance of gene replacement
		if (RandInt(0, 20) == 1) children[0] = geneReplace(children[0]);
		if (RandInt(0, 20) == 1) children[1] = geneReplace(children[1]);

		if (j + 1 >= populationSize) break;

		old[j] = children[0];
		old[j + 1] = children[1];

		j += 2;
	}
}

int main() {
	int target = numeric_limits <int> :: max() / 10000000;
	vector <int> population;
	
	// generate a random poppulation
	for (int i = 0; i < populationSize; i++) {
		population.push_back(RandInt());
	}

	// evolve iteratively
	for (int i = 0; i < 100; i++) {
		newGeneration(population, target);
	}

	cout << "best guess: " << population[0] << " Best fitness: " << fitness(population[0], target) << '\n';

	return 0;
}
