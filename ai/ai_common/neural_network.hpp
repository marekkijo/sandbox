#pragma once

#include <vector>

namespace ai {
class NeuralNetwork {
public:
  NeuralNetwork(const std::size_t inputs, const std::vector<std::size_t> &layers_definition);
  ~NeuralNetwork() = default;

  NeuralNetwork(const NeuralNetwork &) = delete;
  NeuralNetwork &operator=(const NeuralNetwork &) = delete;
  NeuralNetwork(NeuralNetwork &&) noexcept = delete;
  NeuralNetwork &operator=(NeuralNetwork &&) noexcept = delete;

  void init_random(const float mid, const float spread);
  void set_input(const std::size_t index, const float value);
  const std::vector<float> &get_output() const;
  void feed_forward();

private:
  std::vector<float> inputs_;
  std::vector<std::vector<std::vector<float>>> weights_;
  std::vector<std::vector<float>> biases_;
  std::vector<std::vector<float>> activations_;
};
} // namespace ai
