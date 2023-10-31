#pragma once

#include "assert.hpp"
//#include "lite_block.hpp"
#include "utils.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <variant>
#include <vector>

constexpr static int GENOME_INITIAL_LENGTH_MIN = 0;
constexpr static int GENOME_INITIAL_LENGTH_MAX = 100;

constexpr static int NUMBER_INTERNAL_NEURONS = 10;
constexpr static double POINT_MUTATION_RATE = 0.001;

constexpr int MAX_GENERATIONS = 100;
constexpr int STEPS_PER_GENERATION = 1000;

constexpr int MAX_POPULATION = 1'000;

enum SensorType : uint16_t {
    // тут нужно дать ему познания о мире и где он находится

    // дадим ему 32 бита nonce

    POS,
    BIT,
    //VAL,

    NUM_SENSORS,
};

enum ActionType : uint16_t {
    // тут ему нужно делать какие-то действия на основании входа

    // дадим ему возможность изменять биты nonce

    LEFT,
    RIGHT,
    TOP,
    BOTTOM,

    NUM_ACTIONS,
};

struct Neuron {
    enum NeuronType {
        SENSOR,
        INTERNAL,
        ACTION,
    };

    /*struct SensorNeuron {
        // выплевывает значение [0, 1]
        SensorType type;
    };
    struct InternalNeuron {
        // выплевывает значение [-1, +1]
    };
    struct ActionNeuron {
        // выплевывает значение от [0, 1]
        ActionType type;
    };*/

    NeuronType type = INTERNAL;
    //double output = 0;
    //bool is_const = false;// если это так, то этот нейрон просто некоторая константа
    //SensorType sensor_type;
    //ActionType action_type;
};

struct Gene {
    int16_t weight;
    int16_t base;

    Neuron::NeuronType source_type;// SENSOR or INTERNAL
    uint16_t source;               // индекс начала соединения

    Neuron::NeuronType sink_type;// INTERNAL or ACTION
    uint16_t sink;               // индекс конца соединения

    void UpdateLinks() {
        if (source_type == Neuron::NeuronType::SENSOR) {
            source %= SensorType::NUM_SENSORS;
        } else {
            source %= NUMBER_INTERNAL_NEURONS;
        }

        if (sink_type == Neuron::NeuronType::INTERNAL) {
            sink %= NUMBER_INTERNAL_NEURONS;
        } else {
            sink %= ActionType::NUM_ACTIONS;
        }
    }

    [[nodiscard]] double getWeight() const {
        return static_cast<double>(weight) / 8192;
    }

    [[nodiscard]] double getBase() const {
        return static_cast<double>(base) / 8192;
    }
};

using Genome = std::vector<Gene>;

Gene makeRandomGene() {
    Gene gene;
    gene.weight = static_cast<int16_t>(rnd());
    gene.base = static_cast<int16_t>(rnd());
    gene.source = rnd();
    gene.sink = rnd();

    if (rnd() & 1) {
        gene.source_type = Neuron::NeuronType::SENSOR;
    } else {
        gene.source_type = Neuron::NeuronType::INTERNAL;
    }

    if (rnd() & 1) {
        gene.sink_type = Neuron::NeuronType::INTERNAL;
    } else {
        gene.sink_type = Neuron::NeuronType::ACTION;
    }
    return gene;
}

Genome makeRandomGenome() {
    Genome genome;
    int length = GENOME_INITIAL_LENGTH_MAX;//rnd(GENOME_INITIAL_LENGTH_MIN, GENOME_INITIAL_LENGTH_MAX);
    while (length > 0) {
        length--;
        genome.push_back(makeRandomGene());
    }
    return genome;
}

void randomBitFlip(Genome &genome) {
    ASSERT(!genome.empty(), "empty genome");

    uint32_t gene_index = rnd(0, genome.size() - 1);
    auto &gene = genome.at(gene_index);

    double chance = rnd_chance();

    uint16_t random_bit = (1 << (rnd() % 16));

    if (chance < 0.2) {
        if (gene.source_type == Neuron::NeuronType::SENSOR) {
            gene.source_type = Neuron::NeuronType::INTERNAL;
        } else {
            gene.source_type = Neuron::NeuronType::SENSOR;
        }
    } else if (chance < 0.4) {
        if (gene.sink_type == Neuron::NeuronType::INTERNAL) {
            gene.sink_type = Neuron::NeuronType::ACTION;
        } else {
            gene.sink_type = Neuron::NeuronType::INTERNAL;
        }
    } else if (chance < 0.5) {
        gene.source ^= random_bit;
    } else if (chance < 0.6) {
        gene.sink ^= random_bit;
    } else if (chance < 0.7) {
        gene.weight ^= random_bit;
    } else {
        gene.base ^= random_bit;
    }
}

void applyPointMutations(Genome &genome) {
    for (auto &gene: genome) {
        if (rnd_chance() < POINT_MUTATION_RATE) {
            randomBitFlip(genome);
        }
    }
}

Genome generateChildGenome(const std::vector<Genome> &parents) {
    Genome genome;
    ASSERT(!parents.empty(), "no parents");

    auto overlayWithSliceOf = [&](const Genome &other_genome) {
        uint32_t left = rnd() % other_genome.size();
        uint32_t right = rnd() % other_genome.size();
        if (left > right) {
            std::swap(left, right);
        }
        std::copy(other_genome.begin() + left, other_genome.begin() + right, genome.begin() + left);
    };

    if (parents.size() == 1) {
        genome = parents.at(0);
    } else if (parents.size() == 2) {
        const Genome &g1 = parents.at(0);
        const Genome &g2 = parents.at(1);

        if (g1.size() > g2.size()) {
            genome = g1;
            overlayWithSliceOf(g2);
        } else {
            genome = g2;
            overlayWithSliceOf(g1);
        }
    } else {
        ASSERT(false, "more then 2 parents");
    }
    // TODO: тут еще не все

    applyPointMutations(genome);

    return genome;
}

struct NeuralNet {
    // сначала идут все Sensor, потом Internal, затем Action
    std::vector<Neuron> neurons;
    Genome connections;

    NeuralNet() = default;

    NeuralNet(Genome genome) {
        // UpdateLinks
        for (auto &gene: genome) {
            gene.UpdateLinks();
        }

        // build neurons
        {
            for (uint32_t count = 0; count < SensorType::NUM_SENSORS; count++) {
                neurons.push_back({Neuron::NeuronType::SENSOR /*, SensorType::NONCE_BIT_1, ActionType::NUM_ACTIONS*/});
            }
            for (uint32_t count = 0; count < NUMBER_INTERNAL_NEURONS; count++) {
                neurons.push_back(
                        {Neuron::NeuronType::INTERNAL /*, SensorType::NUM_SENSORS, ActionType::NUM_ACTIONS*/});
            }
            for (uint32_t count = 0; count < ActionType::NUM_ACTIONS; count++) {
                neurons.push_back(
                        {Neuron::NeuronType::ACTION /*, SensorType::NUM_SENSORS, ActionType::FLIP_NONCE_BIT_1*/});
            }
        }

        // build connections
        {
            // sensor -> internal
            for (auto [weight, base, source_type, source, sink_type, sink]: genome) {
                if (source_type == Neuron::NeuronType::SENSOR && sink_type == Neuron::NeuronType::INTERNAL) {
                    connections.push_back({weight, base, source_type, source, sink_type, sink});
                }
            }

            // internal -> internal
            for (auto [weight, base, source_type, source, sink_type, sink]: genome) {
                if (source_type == Neuron::NeuronType::INTERNAL && sink_type == Neuron::NeuronType::INTERNAL) {
                    connections.push_back({weight, base, source_type, source, sink_type, sink});
                }
            }

            // sensor -> action
            for (auto [weight, base, source_type, source, sink_type, sink]: genome) {
                if (source_type == Neuron::NeuronType::SENSOR && sink_type == Neuron::NeuronType::ACTION) {
                    connections.push_back({weight, base, source_type, source, sink_type, sink});
                }
            }

            // internal -> action
            for (auto [weight, base, source_type, source, sink_type, sink]: genome) {
                if (source_type == Neuron::NeuronType::INTERNAL && sink_type == Neuron::NeuronType::ACTION) {
                    connections.push_back({weight, base, source_type, source, sink_type, sink});
                }
            }

            ASSERT(connections.size() == genome.size(), "lost gene");

            // переведем индексы source и sink в соответствии с индексацией neurons
            for (auto &[weight, base, source_type, source, sink_type, sink]: connections) {

                if (source_type == Neuron::NeuronType::SENSOR) {
                    source += 0;
                } else {// INTERNAL
                    source += SensorType::NUM_SENSORS;
                }

                if (sink_type == Neuron::NeuronType::INTERNAL) {
                    sink += SensorType::NUM_SENSORS;
                } else {// ACTION
                    sink += SensorType::NUM_SENSORS + NUMBER_INTERNAL_NEURONS;
                }
            }
        }
    }

    std::array<double, ActionType::NUM_ACTIONS>
    feedForward(const std::array<double, SensorType::NUM_SENSORS> &input_sensors) {
        std::vector<double> neuron_accumulators(neurons.size());

        // скопируем сенсоры в neuron_accumulators
        std::copy(input_sensors.begin(), input_sensors.end(), neuron_accumulators.begin());

        bool neuron_outputs_computed = false;
        for (Gene &conn: connections) {
            if (conn.sink_type == Neuron::NeuronType::ACTION && !neuron_outputs_computed) {
                // мы закончили вычислять значения internal neurons. приведем их к [-1, +1]
                neuron_outputs_computed = true;
                for (uint32_t neuron_index = SensorType::NUM_SENSORS;
                     neuron_index < SensorType::NUM_SENSORS + NUMBER_INTERNAL_NEURONS; neuron_index++) {
                    neuron_accumulators.at(neuron_index) = std::tanh(neuron_accumulators.at(neuron_index));
                }
            }

            neuron_accumulators.at(conn.sink) +=
                    neuron_accumulators.at(conn.source) * conn.getWeight() + conn.getBase();
        }

        std::array<double, ActionType::NUM_ACTIONS> result{};
        // скопируем вычисленные значения actions
        std::copy(neuron_accumulators.begin() + SensorType::NUM_SENSORS + NUMBER_INTERNAL_NEURONS - 1,
                  neuron_accumulators.end(), result.begin());
        return result;
    }
};

// индивидуум
struct Indiv {
    Genome genome;
    NeuralNet neural_net;

    uint32_t position = 0;// is same nonce in BTC
    uint8_t bit = 0;
    double best_val = 0;

    Indiv() = default;

    Indiv(const Genome &new_genome) : neural_net(new_genome), genome(new_genome) {
    }

    [[nodiscard]] std::array<double, SensorType::NUM_SENSORS> CalcSensors(lite_block b) const {
        std::array<double, SensorType::NUM_SENSORS> result{};
        result[SensorType::POS] = static_cast<double>(position) / UINT32_MAX;
        result[SensorType::BIT] = static_cast<double>(bit) / 31;
        //result[SensorType::VAL] = static_cast<double>(b.calc_hash(position).builtin_ctz()) / 255;
        return result;
    }

    void simulateStep(lite_block b) {
        auto input_sensors = CalcSensors(b);
        auto actions = neural_net.feedForward(input_sensors);

        uint32_t best_index = 0;
        for (uint32_t index = 0; index < ActionType::NUM_ACTIONS; index++) {
            if (actions.at(best_index) < actions.at(index)) {
                best_index = index;
            }
        }

        if (actions.at(best_index) > 0) {
            if (best_index == ActionType::BOTTOM) {
                bit++;
                if (bit > 31) {
                    bit = 0;
                }
            } else if (best_index == ActionType::TOP) {
                bit--;
                if (bit < 0) {
                    bit = 31;
                }
            } else if (best_index == ActionType::LEFT) {
                position -= (1ULL << bit);
            } else if (best_index == ActionType::RIGHT) {
                position += (1ULL << bit);
            }
        }

        uint32_t x = b.calc_hash(position).builtin_ctz();
        best_val = std::max(best_val, pow(2, x));
        //best_val = vals[x][y];

        //if (actions.at(best_index) > 0) {
        //position ^= (1 << best_index);
        //}
        //best_ctz = std::max(best_ctz, block.calc_hash(position).builtin_ctz());
    }
};

std::vector<Indiv> build_population() {
    std::vector<Indiv> population(MAX_POPULATION);
    for (auto &indiv: population) {
        indiv = makeRandomGenome();
        indiv.position = rnd();
        indiv.bit = rnd() % 32;
        indiv.best_val = 0;
    }
    return population;
}

void simulator(lite_block b) {
    std::vector<Indiv> population = build_population();

    for (int generation_number = 0; true /*generation_number < MAX_GENERATIONS*/; generation_number++) {
        // set random position
        for (auto &indiv: population) {
            indiv.position = 1787677545 - rnd() * (generation_number / 10000.0);
            indiv.bit = rnd() % 32;
            indiv.best_val *= 0.5;
        }

        for (Indiv &indiv: population) {
            for (int step = 0; step < STEPS_PER_GENERATION; step++) {
                indiv.simulateStep(b);
            }
        }

        std::sort(population.begin(), population.end(), [&](const Indiv &lhs, const Indiv &rhs) {
            return lhs.best_val > rhs.best_val;
        });

        double median = 0;
        {
            uint64_t sum = 0;
            uint64_t sum_pos = 0;
            for (const auto &indiv: population) {
                sum += indiv.best_val;
                sum_pos += indiv.position;
            }
            median = 1.0 * sum / population.size();
            std::cout << generation_number << ' ' << median
                      << ' ' << uint32_t(1.0 * sum_pos / population.size()) << ' '
                      << uint32_t(population[0].best_val) << std::endl;
        }

        while (!population.empty() && population.back().best_val < median) {
            population.pop_back();
        }

        if (population.empty()) {
            std::cout << "empty" << std::endl;
            population = build_population();
        }

        std::vector<Indiv> next_generation;
        for (auto &indiv: population) {
            next_generation.push_back(generateChildGenome({indiv.genome}));
            next_generation.back().best_val = indiv.best_val;
        }
        population = std::move(next_generation);

        while (population.size() < MAX_POPULATION) {
            int lhs = rnd() % population.size();
            int rhs = rnd() % population.size();
            Genome new_genome = generateChildGenome(
                    {population[lhs].genome, population[rhs].genome});
            population.push_back(new_genome);
            population.back().best_val = std::max(population[lhs].best_val, population[rhs].best_val);
        }

        for (int i = 0; i < 100; i++) {
            population.emplace_back(makeRandomGenome());
        }
    }
}
