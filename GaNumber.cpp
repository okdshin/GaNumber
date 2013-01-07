#ifdef GANUMBER_UNIT_TEST
#include "GaNumber.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "ga/Chromosome.h"
#include "ga/RandomRoutine.h"
#include "NumberEvaluator.h"

using namespace numga;

int main(int argc, char* argv[])
{
	std::vector<ga::Chromosome> chromosome_list;
	unsigned int chromosome_num = 60;
	unsigned int gene_length = 40;
	unsigned long int num = (pow(2, 59) - pow(2, 10));
	double probability = 0.05;
	double recombinate_gene_probability = 0.1;
	auto seed = ga::CreateRandomSeed();
	auto rand = std::bind(std::uniform_int_distribution<int>(0, 1), std::mt19937(seed));
	auto float_rand = std::bind(std::uniform_real_distribution<double>(0, 1), std::mt19937(seed));

	std::cout << "ChromosomeNum:" << chromosome_num << std::endl;
	std::cout << "GeneLen:" << gene_length << std::endl;
	std::cout << "Num:" << num << std::endl;
	std::cout << "MutateProb:" << probability << std::endl;
	std::cout << "RecombinateGeneProb:" 
		<< recombinate_gene_probability << std::endl;
	for(unsigned int i = 0; i < chromosome_num; ++i){
		chromosome_list.push_back(
			ga::CreateChromosome(gene_length, ga::ElementGenerator(std::ref(rand))));
	}

	for(const auto& chromosome : chromosome_list){
		std::cout << chromosome << std::endl;	
	}
	std::cout << "\n";

	auto evaluator = CreateNumberEvaluator(num);
	std::ofstream log_file("log.csv");
	
	unsigned int try_count = 40;
	for(unsigned int t = 0; t < try_count; ++t){
		std::cout << "Generation:" << t << std::endl;
		double fitness_sum = 0.0;
		double max_fitness = 0.0;
		for(const auto& chromosome : chromosome_list){
			const auto fitness = (evaluator()(chromosome))();
			fitness_sum += fitness;
			if(fitness > max_fitness){
				max_fitness = fitness;
			}
		}

		for(const auto& chromosome : chromosome_list){
			std::cout << chromosome << "RelativeFit:" << chromosome_num * ((evaluator()(chromosome))()/fitness_sum) << std::endl;	
		}
		log_file << t << "," << (fitness_sum / chromosome_num) << "," << max_fitness << "\n";
		std::cout << "AveF(x):" << (fitness_sum / chromosome_list.size()) << "\n";
		std::cout << "MaxF(x):" << max_fitness << "\n\n";


		std::vector<ga::Chromosome> child_list;
		{//CreateChildren
			std::vector<double> expected_value_integrated_list;
			expected_value_integrated_list.push_back(0);
			for(const auto& chromosome : chromosome_list){
				auto expected_value = ((evaluator()(chromosome))()/fitness_sum);
				expected_value_integrated_list.push_back(
					expected_value_integrated_list.back()+expected_value);
			}
			//erase top 0
			expected_value_integrated_list.erase(expected_value_integrated_list.begin());
			
			for(unsigned int i = 0; i < chromosome_num; ++i){
				double random_value = float_rand();
				int k = 0;
				while(random_value > expected_value_integrated_list.at(k)){
					++k;
				}	
				child_list.push_back(chromosome_list.at(k));
			}
		}

		/*	
		std::vector<ga::Chromosome> child_list;
		{//CreateChildren
			for(const auto& chromosome : chromosome_list){
				auto child_num = chromosome_num*((evaluator()(chromosome))()/fitness_sum);
				for(unsigned int i=0; i < static_cast<unsigned int>(child_num+0.4); ++i){
					child_list.push_back(chromosome);
				}
			}
		}
		*/

		std::vector<ga::Chromosome> recombinated_child_list;	
		{//RecombinateGene
			auto decider = 
				ga::CreateRandomRecombinateGeneDecider(recombinate_gene_probability, seed);
			auto divided_tuple = ga::DivideRandomly(child_list, seed);
			auto bigger_list = std::get<0>(divided_tuple)();
			auto smaller_list = std::get<0>(divided_tuple)();
			for(unsigned int i = 0; i < smaller_list.size(); ++i){
				auto recombinated_tuple = 
					RecombinateGene(bigger_list.at(i), smaller_list.at(i), decider());
				recombinated_child_list.push_back(std::get<0>(recombinated_tuple));
				recombinated_child_list.push_back(std::get<1>(recombinated_tuple));
			}
			if(child_list.size() % 2){
				recombinated_child_list.push_back(bigger_list.back());	
			}
		}
		
		std::vector<ga::Chromosome> mutated_child_list;
		{//Mutate
			auto decider = ga::CreateRandomMutateDecider(probability, seed);
			for(const auto& chromosome : recombinated_child_list){
				mutated_child_list.push_back(Mutate(chromosome, decider));
			}
		}

		chromosome_list = mutated_child_list;
	}


    return 0;
}

#endif
