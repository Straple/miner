
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdexcept>

#include "opennn/opennn.h"

using namespace opennn;

void run_neural_network(){
    cout << "OpenNN. Simple classification example." << endl;

    // Data set

    DataSet data_set("data.csv", ';', true);

    std::cout << data_set.get_columns_number() << std::endl;
    //std::cout << data_set.get_rows_number() << std::endl;

    const Index input_variables_number = 467;
    const Index target_variables_number = 1;

    // Neural network

    NeuralNetwork neural_network(NeuralNetwork::ProjectType::Classification,
                                 {input_variables_number, 100, 100, 100, 100, target_variables_number});

    // Training strategy

    TrainingStrategy training_strategy(&neural_network, &data_set);

    training_strategy.set_loss_method(TrainingStrategy::LossMethod::CROSS_ENTROPY_ERROR);
    training_strategy.set_optimization_method(TrainingStrategy::OptimizationMethod::ADAPTIVE_MOMENT_ESTIMATION);

    std::cout << "OK1" << std::endl;
    const TrainingResults training_results = training_strategy.perform_training();
    std::cout << "OK2" << std::endl;

    // Testing analysis

    TestingAnalysis testing_analysis(&neural_network, &data_set);

    const Tensor<type, 1> binary_classification_tests = testing_analysis.calculate_binary_classification_tests();
    const Tensor<Index, 2> confusion = testing_analysis.calculate_confusion();

    cout << "\nConfusion matrix:\n" << confusion << "\n" << endl;

    // Save results

    neural_network.save("neural_network.xml");
    neural_network.save_expression_python("expression.py");

    cout << "Bye Simple Function Classification" << endl;
}