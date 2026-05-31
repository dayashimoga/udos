#pragma once

/// @file inference_engine.hpp
/// @brief Abstract ONNX Runtime wrapper for deep learning model inference.

#include "uados/types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace uados::perception {

/// @brief Deep learning model inference wrapper (ONNX Runtime).
///
/// Under production targets, initializes an ONNX Runtime InferenceSession
/// with CPU/GPU/NPU execution providers. Under mock/virtual configurations,
/// performs high-fidelity mock model evaluations.
class InferenceEngine {
public:
    InferenceEngine() = default;
    virtual ~InferenceEngine() = default;

    /// Load a model from an ONNX file
    /// @param model_path Path to the .onnx file
    /// @return Status::Ok on success
    virtual Status load_model(const std::string& model_path);

    /// Run forward inference
    /// @param input_tensor Flat input float array (e.g. normalized image pixels)
    /// @param input_shape Shape of input tensor (e.g. {1, 3, 640, 640})
    /// @return Flat output float array containing raw model predictions
    virtual std::vector<float> run_inference(
        const std::vector<float>& input_tensor,
        const std::vector<int64_t>& input_shape);

    /// Check if a model is successfully loaded
    [[nodiscard]] bool is_loaded() const noexcept { return loaded_; }

protected:
    bool loaded_{false};
    std::string model_path_;
};

} // namespace uados::perception
