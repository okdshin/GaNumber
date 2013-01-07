#ifdef NUMBEREVALUATOR_UNIT_TEST
#include "NumberEvaluator.h"
#include <iostream>

using namespace numga;

int main(int argc, char* argv[])
{
	auto seed = ga::CreateRandomSeed();
	auto rand = ga::CreateDefaultRandomIntFunctor(0, 1, seed);
	const auto chromosome = ga::CreateRandomChromosome(5, rand);
	std::cout << chromosome << std::endl;
	auto fitness = CreateNumberEvaluator(6)()(chromosome);
	std::cout << fitness << std::endl;

    return 0;
}

#endif
