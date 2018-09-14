//Bruno Clemente e Thyago Stall
#ifndef CPPREMOTEAPI_GENETICALGORITHM_H
#define CPPREMOTEAPI_GENETICALGORITHM_H

#include "PathFinder.hpp"

class GeneticAlgorithm : public PathFinder {
public:
    std::vector<State> find() override;
private:
    int fitness(const State &state);
    State mutate(const State &state);
    std::pair<State, State> crossover(const State &parent1, const State &parent2);
};

#endif //CPPREMOTEAPI_GENETICALGORITHM_HPP
