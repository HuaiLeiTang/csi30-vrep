//Bruno Clemente e Thyago Stall
#include <random>
#include <map>

#include "GeneticAlgorithm.hpp"
#include "Map.hpp"
#include "AStarSearch.hpp"

const int INF = 10000;
const int POPULATION_SIZE = 10;

bool compare(const std::pair<int, Point> &a,const std::pair<int, Point> &b){
    return a.first < b.first;
}

std::vector<State> GeneticAlgorithm::find() {
    int generation = 0;
    std::vector<Point> population = this->initializePopulation();
    int currentEvaluation = this->evaluatePopulation(population);
    printf("Generation %3d -> fitness(P) = %d\n", generation, currentEvaluation);

    while (currentEvaluation >= INF) {
        generation++;
        population = this->createNewGeneration(population);
        currentEvaluation = this->evaluatePopulation(population);
        printf("Generation %3d -> fitness(P) = %d\n", generation, currentEvaluation);
    }

    Point best = this->select(population);
    printf("Best point -> (%d, %d)\n", best.x, best.y);
    return this->findPathGoingThrough(best);
}

std::vector<State> GeneticAlgorithm::findPathGoingThrough(Point best) {
    State best_state {best.x, best.y, 0};

    std::vector<State> endBestState;
    endBestState.push_back(best_state);

    AStarSearch finderFirstHalf {this->start_state, endBestState};
    std::vector<State> firstHalf = finderFirstHalf.find();

    AStarSearch finderSecondHalf {best_state, this->end_states};
    std::vector<State> secondHalf = finderSecondHalf.find();

    firstHalf.insert(firstHalf.end(), secondHalf.begin(), secondHalf.end());
    return firstHalf;
}

std::vector<Point> GeneticAlgorithm::initializePopulation() {
    std::vector<Point> population;
    std::random_device rd;

    printf("Initial population: [ ");
    for (int i = 0; i < POPULATION_SIZE; i++) {
        int x = rd() % 10;
        int y = rd() % 10;
        population.push_back({x, y});
        printf("{%d, %d} ", x, y);
    }
    printf("]\n");
    return population;
}

int GeneticAlgorithm::evaluatePopulation(const std::vector<Point> &points) {
    int result = 0;
    for (auto &point : points) {
        result += this->fitness(point);
    }
    return result;
}

std::vector<Point> GeneticAlgorithm::createNewGeneration(const std::vector<Point> &population) {
    std::vector<Point> newGeneration;

    for (int i = 0; i < population.size(); i += 2) {
        Point parent1 = this->select(population);
        Point parent2 = this->select(population);

        std::pair<Point, Point> offspring = this->crossover(parent1, parent2);

        Point offspring1 = this->mutate(offspring.first);
        Point offspring2 = this->mutate(offspring.second);

        newGeneration.push_back(offspring1);
        newGeneration.push_back(offspring2);
    }

    return newGeneration;
}

Point GeneticAlgorithm::select(const std::vector<Point> &population) {
    std::vector<std::pair<int, Point>> candidates;
    for (auto &point : population) {
        candidates.push_back(std::make_pair(this->fitness(point), point));
    }
    std::sort(candidates.begin(), candidates.end(), compare);

    auto it = candidates.begin();
    std::random_device rd;
    std::advance(it, rd() % candidates.size());
    return it->second;
}

int GeneticAlgorithm::fitness(const Point &point) {
    int distanceToStartState = this->distance(State::getStartState(), point);
    int minimumDistanceToAnEndState = INF;
    for (auto &endState : State::getEndState()) {
        minimumDistanceToAnEndState = std::min(minimumDistanceToAnEndState, this->distance(endState, point));
        if (this->areThereWallsInPath(point, endState)) {
            return INF;
        }
    }
    return abs(distanceToStartState - minimumDistanceToAnEndState);
}

bool GeneticAlgorithm::areThereWallsInPath(const Point &p1, const State &p2) {
    int x = p1.x;
    int y = p1.y;

    int x2 = p2.getX();
    int y2 = p2.getY();

    int w = x2 - x;
    int h = y2 - y;
    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

    if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
    if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
    if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;

    int longest = abs(w);
    int shortest = abs(h);

    if (longest <= shortest) {
        longest = abs(h);
        shortest = abs(w);
        if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
        dx2 = 0;
    }

    int numerator = longest >> 1;
    for (int i = 0; i <= longest; i++) {
        if (Map::isObstacle(x, y))
            return true;

        numerator += shortest;
        if (numerator >= longest) {
            numerator -= longest;
            x += dx1; y += dy1;
        } else {
            x += dx2; y += dy2;
        }
    }

    return false;
}

int GeneticAlgorithm::distance(const State &state, const Point &point) {
    return (int) sqrt(pow(point.x - state.getX(), 2) + pow(point.y - state.getY(), 2));
}

Point GeneticAlgorithm::mutate(const Point &point) {
    int increment[] = {-1, 0, 1};
    std::random_device rd;

    int x = point.x + increment[rd() % 3];
    int y = point.y + increment[rd() % 3];

    x = std::min(x, Map::getSize() - 1);
    y = std::min(y, Map::getSize() - 1);

    x = std::max(x, 0);
    y = std::max(y, 0);

    return {x, y};
}

std::pair<Point, Point> GeneticAlgorithm::crossover(const Point &parent1, const Point &parent2) {
    Point offspring1 {parent2.x, parent1.y};
    Point offspring2 {parent1.x, parent2.y};
    return std::pair<Point, Point>{offspring1, offspring2};
}
