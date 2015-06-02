//=======================================================================
// Copyright (c) 2014-2015 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <deque>

#include "catch.hpp"

#include "dll/dbn.hpp"
#include "dll/stochastic_gradient_descent.hpp"

#include "mnist/mnist_reader.hpp"
#include "mnist/mnist_utils.hpp"

TEST_CASE( "dbn/sgd/1", "[dbn][mnist][sgd]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::init_weights>::rbm_t,
        dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>, dll::trainer<dll::sgd_trainer>>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>(500);

    REQUIRE(!dataset.training_images.empty());

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 20);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100, 10);

    REQUIRE(error < 5e-2);

    auto test_error = dll::test_set(dbn, dataset.test_images, dataset.test_labels, dll::predictor());

    std::cout << "test_error:" << test_error << std::endl;

    REQUIRE(test_error < 0.2);
}

TEST_CASE( "dbn/sgd/2", "[dbn][mnist][sgd]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::init_weights>::rbm_t,
        dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>, dll::momentum>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>(1000);

    REQUIRE(!dataset.training_images.empty());

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.05;

    dbn->pretrain(dataset.training_images, 20);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100, 10);

    REQUIRE(error < 5e-2);

    auto test_error = dll::test_set(dbn, dataset.test_images, dataset.test_labels, dll::predictor());

    std::cout << "test_error:" << test_error << std::endl;

    REQUIRE(test_error < 0.2);
}

TEST_CASE( "dbn/sgd/3", "[dbn][mnist][sgd][gaussian]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 200, dll::momentum, dll::batch_size<25>, dll::visible<dll::unit_type::GAUSSIAN>>::rbm_t,
        dll::rbm_desc<200, 500, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<500, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::deque, double>(1000);

    REQUIRE(!dataset.training_images.empty());

    mnist::normalize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 20);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100, 20);

    REQUIRE(error < 5e-2);

    auto test_error = dll::test_set(dbn, dataset.test_images, dataset.test_labels, dll::predictor());

    std::cout << "test_error:" << test_error << std::endl;

    REQUIRE(test_error < 0.2);
}

//This test should not perform well, but should not fail
TEST_CASE( "dbn/sgd/4", "[dbn][mnist][sgd][relu]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::RELU>, dll::init_weights>::rbm_t,
        dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::deque, double>(200);

    REQUIRE(!dataset.training_images.empty());

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 20);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100, 10);

    REQUIRE(std::isfinite(error));
}

TEST_CASE( "dbn/sgd/5", "[dbn][mnist][sgd]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::init_weights>::rbm_t,
        dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>, dll::weight_decay<dll::decay_type::L2>>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>();

    REQUIRE(!dataset.training_images.empty());
    dataset.training_images.resize(200);
    dataset.training_labels.resize(200);

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 5);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 200, 10);

    REQUIRE(error < 1e-1);
}

TEST_CASE( "dbn/sgd/6", "[dbn][mnist][sgd]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
        dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::init_weights>::rbm_t,
        dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
        dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>, dll::momentum, dll::weight_decay<dll::decay_type::L2>>::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>();

    REQUIRE(!dataset.training_images.empty());
    dataset.training_images.resize(200);
    dataset.training_labels.resize(200);

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 10);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 200, 100);

    REQUIRE(error < 5e-2);
}

TEST_CASE( "dbn/sgd/7", "[dbn][mnist][sgd][memory]" ) {
    typedef dll::dbn_desc<
        dll::dbn_layers<
            dll::rbm_desc<28 * 28, 100, dll::momentum, dll::batch_size<25>, dll::init_weights>::rbm_t,
            dll::rbm_desc<100, 200, dll::momentum, dll::batch_size<25>>::rbm_t,
            dll::rbm_desc<200, 10, dll::momentum, dll::batch_size<25>, dll::hidden<dll::unit_type::SOFTMAX>>::rbm_t>,
        dll::trainer<dll::sgd_trainer>,
        dll::memory
        >::dbn_t dbn_t;

    auto dataset = mnist::read_dataset<std::vector, std::vector, double>(500);

    REQUIRE(!dataset.training_images.empty());

    mnist::binarize_dataset(dataset);

    auto dbn = std::make_unique<dbn_t>();

    dbn->learning_rate = 0.1;

    dbn->pretrain(dataset.training_images, 20);
    auto error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100, 10);

    REQUIRE(error < 5e-2);

    auto test_error = dll::test_set(dbn, dataset.test_images, dataset.test_labels, dll::predictor());

    std::cout << "test_error:" << test_error << std::endl;

    REQUIRE(test_error < 0.2);
}