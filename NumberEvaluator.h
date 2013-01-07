#pragma once
//NumberEvaluator:20121205
#include <iostream>
#include "ga/GeneticAlgorithm.h"
namespace numga
{
auto CreateNumberEvaluator(int num) -> ga::Evaluator {
	return ga::Evaluator([num](const ga::Chromosome& chromosome) -> ga::Fitness {
		double level = 1.0;
		double sum = 0.0;
		for(const auto gene_element : chromosome.GetGene()){
			sum += gene_element*level;
			level*=2.0;
		}
		return ga::Fitness(((sum-num)*(sum-num)));
	});
	
}
}

