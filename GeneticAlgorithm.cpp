//
// Created by Thyago Stall on 14/09/2018.
//

#include "GeneticAlgorithm.hpp"

std::vector<State> GeneticAlgorithm::find() {
    return std::vector<State>();
}

int GeneticAlgorithm::fitness(const State &state) {
    return 0;
}

State GeneticAlgorithm::mutate(const State &state) {
    return state;
}

std::pair<State, State> GeneticAlgorithm::crossover(const State &parent1, const State &parent2) {
    return std::pair<State, State>{parent1, parent2};
}
