#cmake_minimum_required(VERSION 3.23)
project(miner)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenSSL REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ROOT "C:/Program Files/boost/boost_1_82_0")

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(PROJECT_LINK_LIBS ${CMAKE_SOURCE_DIR}/Release/opennn/libopennn.a)
endif()

if(WIN32)
    set(PROJECT_LINK_LIBS ../../opennn/Release/opennn)
endif()

#IF (OpenSSL_FOUND)
#ENDIF()
#cmake_minimum_required(VERSION 2.8.12)

find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS}")

add_library(OPENNN OBJECT
        opennn/adaptive_moment_estimation.cpp
        opennn/batch_normalization_layer.cpp
        opennn/bounding_box_regressor_layer.cpp
        opennn/bounding_layer.cpp
        opennn/codification.cpp
        opennn/conjugate_gradient.cpp
        opennn/convolutional_layer.cpp
        opennn/correlations.cpp
        opennn/cross_entropy_error.cpp
        opennn/data_set.cpp
        opennn/flatten_layer.cpp
        opennn/genetic_algorithm.cpp
        opennn/gradient_descent.cpp
        opennn/growing_inputs.cpp
        opennn/growing_neurons.cpp
        opennn/inputs_selection.cpp
        opennn/json_to_xml.cpp
        opennn/layer.cpp
        opennn/learning_rate_algorithm.cpp
        opennn/levenberg_marquardt_algorithm.cpp
        opennn/long_short_term_memory_layer.cpp
        opennn/loss_index.cpp
        opennn/mean_squared_error.cpp
        opennn/minkowski_error.cpp
        opennn/model_selection.cpp
        opennn/neural_network.cpp
        opennn/neurons_selection.cpp
        opennn/non_max_suppression_layer.cpp
        opennn/normalized_squared_error.cpp
        opennn/numerical_differentiation.cpp
        opennn/opennn_images.cpp
        opennn/opennn_strings.cpp
        opennn/optimization_algorithm.cpp
        opennn/perceptron_layer.cpp
        opennn/pooling_layer.cpp
        opennn/probabilistic_layer.cpp
        opennn/pruning_inputs.cpp
        opennn/quasi_newton_method.cpp
        opennn/recurrent_layer.cpp
        opennn/region_based_object_detector.cpp
        opennn/region_proposal_layer.cpp
        opennn/response_optimization.cpp
        opennn/scaling.cpp
        opennn/scaling_layer.cpp
        opennn/statistics.cpp
        opennn/stochastic_gradient_descent.cpp
        opennn/sum_squared_error.cpp
        opennn/tensor_utilities.cpp
        opennn/testing_analysis.cpp
        opennn/text_analytics.cpp
        opennn/tinyxml2.cpp
        opennn/training_strategy.cpp
        opennn/unit_testing.cpp
        opennn/unscaling_layer.cpp
        opennn/weighted_squared_error.cpp)


add_executable(miner
        $<TARGET_OBJECTS:OPENNN> main.cpp sha256/openssl_sha256.cpp bits_manipulation.cpp block.cpp utils.cpp pool_client.cpp miner.cpp logger.cpp lite_block.cpp fast_string.cpp statistic.cpp)

target_link_libraries(miner OpenSSL::SSL)

target_link_libraries(miner ${Boost_LIBRARIES})

