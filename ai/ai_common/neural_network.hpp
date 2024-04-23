#pragma once

namespace ai {
class NeuralNetwork {
public:
  NeuralNetwork() = default;
  ~NeuralNetwork() = default;

  NeuralNetwork(const NeuralNetwork &) = delete;
  NeuralNetwork &operator=(const NeuralNetwork &) = delete;
  NeuralNetwork(NeuralNetwork &&) noexcept = delete;
  NeuralNetwork &operator=(NeuralNetwork &&) noexcept = delete;
};
} // namespace ai
