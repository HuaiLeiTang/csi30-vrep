//Bruno Clemente e Thyago Stall
#ifndef CPPREMOTEAPI_GENETICALGORITHM_H
#define CPPREMOTEAPI_GENETICALGORITHM_H

#include "PathFinder.hpp"

struct Point {
    int x;
    int y;
};

class GeneticAlgorithm : public PathFinder {
public:
    GeneticAlgorithm(const State &start_state, const std::vector<State> &end_states) : PathFinder(start_state,
                                                                                                  end_states) {}

    std::vector<State> find() override;
private:
    int fitness(const Point &point);
    int distance(const State &state, const Point &point);
    Point mutate(const Point &point);
    std::pair<Point, Point> crossover(const Point &parent1, const Point &parent2);

    std::vector<Point> createNewGeneration(const std::vector<Point> &population);
    Point select(const std::vector<Point> &population);

    std::vector<Point> initializePopulation();

    int evaluatePopulation(const std::vector<Point> &points);
    bool areThereWallsInPath(const Point &p1, const State &p2);

    std::vector<State> findPathGoingThrough(Point best);
};

#endif //CPPREMOTEAPI_GENETICALGORITHM_HPP
