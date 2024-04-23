#include "neural_network.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace ai {
namespace {
float relu_activation(const float x) { return x < 0.0f ? 0.0f : x; }

float sigmoid_activation(const float x) { return 1.0f / (1.0f + std::exp(-x)); }
} // namespace

NeuralNetwork::NeuralNetwork(const std::size_t inputs, const std::vector<std::size_t> &layers_definition)
    : inputs_(inputs)
    , weights_(layers_definition.size())
    , biases_(layers_definition.size())
    , activations_(layers_definition.size()) {
  for (auto l_it = std::size_t{0u}; l_it < layers_definition.size(); l_it++) {
    weights_[l_it].resize(layers_definition[l_it]);
    biases_[l_it].resize(layers_definition[l_it]);
    activations_[l_it].resize(layers_definition[l_it]);
    const auto current_weights_size = l_it == 0u ? inputs : layers_definition[l_it - 1u];
    std::for_each(weights_[l_it].begin(), weights_[l_it].end(), [current_weights_size](auto &weights) {
      weights.resize(current_weights_size);
    });
  }
}

void NeuralNetwork::init_random(const float mid, const float spread) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> distribution(mid - spread, mid + spread);

  for (auto l_it = std::size_t{0u}; l_it < weights_.size(); l_it++) {
    for (auto n_it = std::size_t{0u}; n_it < weights_[l_it].size(); n_it++) {
      for (auto w_it = std::size_t{0u}; w_it < weights_[l_it][n_it].size(); w_it++) {
        weights_[l_it][n_it][w_it] = distribution(gen);
      }
      biases_[l_it][n_it] = distribution(gen);
    }
  }
}

void NeuralNetwork::set_input(const std::size_t index, const float value) { inputs_[index] = value; }

const std::vector<float> &NeuralNetwork::get_output() const { return activations_.back(); }

void NeuralNetwork::feed_forward() {
  for (auto l_it = std::size_t{0u}; l_it < weights_.size(); l_it++) {
    for (auto n_it = std::size_t{0u}; n_it < weights_[l_it].size(); n_it++) {
      auto sum = biases_[l_it][n_it];
      for (auto w_it = std::size_t{0u}; w_it < weights_[l_it][n_it].size(); w_it++) {
        sum += weights_[l_it][n_it][w_it] * (l_it == 0u ? inputs_[w_it] : activations_[l_it - 1u][w_it]);
      }
      activations_[l_it][n_it] = relu_activation(sum);
    }
  }
}
} // namespace ai
