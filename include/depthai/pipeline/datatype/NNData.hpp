#pragma once

#include <chrono>
#include <limits>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "Buffer.hpp"
#include "depthai-shared/datatype/RawNNData.hpp"
#include <eigen3/Eigen/Dense>

namespace dai {

/**
 * NNData message. Carries tensors and their metadata
 */
class NNData : public Buffer {
    static constexpr int DATA_ALIGNMENT = 64;
    [[deprecated("Use 'addTensor()' instead")]]
    std::shared_ptr<RawBuffer> serialize() const override;
    static uint16_t fp32_to_fp16(float);
    RawNNData& rawNn;

    // store the data
    // uint8_t
    std::unordered_map<std::string, std::vector<std::uint8_t>> u8Data;
    // FP16
    std::unordered_map<std::string, std::vector<std::uint16_t>> fp16Data;

   public:
    /**
     * Construct NNData message.
     */
    NNData();
    explicit NNData(std::shared_ptr<RawNNData> ptr);
    virtual ~NNData() = default;

    // Expose
    // uint8_t
    /**
     * Set a layer with datatype U8.
     * @param name Name of the layer
     * @param data Data to store
     */
    [[deprecated("Use 'addTensor()' instead")]]
    NNData& setLayer(const std::string& name, std::vector<std::uint8_t> data);

    /**
     * Set a layer with datatype U8. Integers are cast to bytes.
     * @param name Name of the layer
     * @param data Data to store
     */
    [[deprecated("Use 'addTensor()' instead")]]
    NNData& setLayer(const std::string& name, const std::vector<int>& data);

    // fp16
    /**
     * Set a layer with datatype FP16. Float values are converted to FP16.
     * @param name Name of the layer
     * @param data Data to store
     */
    [[deprecated("Use 'addTensor()' instead")]]
    NNData& setLayer(const std::string& name, std::vector<float> data);

    /**
     * Set a layer with datatype FP16. Double values are converted to FP16.
     * @param name Name of the layer
     * @param data Data to store
     */
    [[deprecated("Use 'addTensor()' instead")]]
    NNData& setLayer(const std::string& name, std::vector<double> data);

    // getters
    /**
     * @returns Names of all layers added
     */
    std::vector<std::string> getAllLayerNames() const;

    /**
     * @returns All layers and their information
     */
    std::vector<TensorInfo> getAllLayers() const;

    /**
     * Retrieve layers tensor information
     * @param name Name of the layer
     * @param[out] tensor Outputs tensor information of that layer
     * @returns True if layer exists, false otherwise
     */
    [[deprecated("Use 'getTensor()' instead")]]
    bool getLayer(const std::string& name, TensorInfo& tensor) const;

    /**
     * Checks if given layer exists
     * @param name Name of the layer
     * @returns True if layer exists, false otherwise
     */
    bool hasLayer(const std::string& name) const;

    /**
     * Retrieve datatype of a layers tensor
     * @param name Name of the layer
     * @param[out] datatype Datatype of layers tensor
     * @returns True if layer exists, false otherwise
     */
    bool getLayerDatatype(const std::string& name, TensorInfo::DataType& datatype) const;

    // uint8
    /**
     * Convenience function to retrieve U8 data from layer
     * @param name Name of the layer
     * @returns U8 binary data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<std::uint8_t> getLayerUInt8(const std::string& name) const;

    // fp16
    /**
     * Convenience function to retrieve float values from layers FP16 tensor
     * @param name Name of the layer
     * @returns Float data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<float> getLayerFp16(const std::string& name) const;

    // int32
    /**
     * Convenience function to retrieve INT32 values from layers tensor
     * @param name Name of the layer
     * @returns INT32 data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<std::int32_t> getLayerInt32(const std::string& name) const;

    // first layer
    /**
     * Convenience function to retrieve U8 data from first layer
     * @returns U8 binary data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<std::uint8_t> getFirstLayerUInt8() const;

    /**
     * Convenience function to retrieve float values from first layers FP16 tensor
     * @returns Float data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<float> getFirstLayerFp16() const;

    /**
     * Convenience function to retrieve INT32 values from first layers tensor
     * @returns INT32 data
     */
    [[deprecated("Use 'getTensor()' instead")]]
    std::vector<std::int32_t> getFirstLayerInt32() const;

    /**
     * Retrieves image timestamp related to dai::Clock::now()
     */
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> getTimestamp() const;

    /**
     * Retrieves image timestamp directly captured from device's monotonic clock,
     * not synchronized to host time. Used mostly for debugging
     */
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> getTimestampDevice() const;

    /**
     * Retrieves image sequence number
     */
    int64_t getSequenceNum() const;

    /**
     * Sets image timestamp related to dai::Clock::now()
     */
    NNData& setTimestamp(std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> timestamp);

    /**
     * Sets image timestamp related to dai::Clock::now()
     */
    NNData& setTimestampDevice(std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> timestamp);

    /**
     * Retrieves image sequence number
     */
    NNData& setSequenceNum(int64_t sequenceNum);

    /**
     * Set a layer with datatype FP16. Double values are converted to FP16.
     * @param name Name of the layer
     * @param data Data to store
     */
    template<typename _Ty = double>
    NNData& addTensor(const std::string& name, const std::vector<_Ty>& data)
    {
        return addTensor(name, (const Eigen::Matrix<_Ty, Eigen::Dynamic, 1>)Eigen::Map<const Eigen::Matrix<_Ty, Eigen::Dynamic, 1>>(data.data(), data.size(), 1));
    };

    /**
     * Add a tensor. Float values are converted to FP16 and integers are cast to bytes.
     * @param name Name of the tensor
     * @param tensor Tensor to store
     */
    template<typename _Ty = double, int _X = Eigen::Dynamic, int _Y = Eigen::Dynamic>
    NNData& addTensor(const std::string& name, const Eigen::Matrix<_Ty, _X, _Y>& tensor)
    {
        static_assert(std::is_integral<_Ty>::value || std::is_floating_point<_Ty>::value, "Tensor type needs to be integral or floating point");
        const size_t rows = _X == Eigen::Dynamic ? tensor.rows() : _X;
        const size_t cols = _Y == Eigen::Dynamic ? tensor.cols() : _Y;

        // Get size in bytes of the converted tensor data, u8 for integral and fp16 for floating point
        const size_t sConvertedData = std::is_integral<_Ty>::value ? rows * cols : 2 * rows * cols;

        // Append bytes so that each new tensor is DATA_ALIGNMENT aligned
        const size_t offset = rawNn.data.size() + ((rawNn.data.end() - rawNn.data.begin() + DATA_ALIGNMENT - 1) / DATA_ALIGNMENT) * DATA_ALIGNMENT;
        rawNn.data.insert(rawNn.data.end(), offset + sConvertedData, 0);

        // Convert data to u8 or fp16 and write to rawNn.data
        if(std::is_integral<_Ty>::value) {
            for(uint32_t i = 0; i < rows * cols; i++) {
                rawNn.data.data()[i + offset] = static_cast<std::uint8_t>(tensor.data()[i]);
            }
        }
        else {
            for(uint32_t i = 0; i < rows * cols; i++) {
                rawNn.data.data()[2*i + offset] = fp32_to_fp16(static_cast<float>(tensor.data()[i]));
            }
        }

        // Add entry in tensors
        TensorInfo info;
        info.name = name;
        info.offset = static_cast<unsigned int>(offset);
        if(std::is_integral<_Ty>::value)
        {
            info.dataType = TensorInfo::DataType::U8F;
        } else {
            info.dataType = TensorInfo::DataType::FP16;
        }
        info.numDimensions = cols;
        for(uint32_t i = 0; i < cols; i++)
        {
            info.dims.push_back(rows);
            info.strides.push_back(sizeof(_Ty) * cols);
        }

        rawNn.tensors.push_back(info);
        return *this;
    }

    /**
     * Convenience function to retrieve values from a tensor
     * @returns Eigen::Matrix<_Ty, _X, _Y> tensor
     */
    template<typename _Ty, int _X = Eigen::Dynamic, int _Y = Eigen::Dynamic>
    Eigen::Matrix<_Ty, _X, _Y> getTensor(const std::string& name)
    {
        const auto it = std::find_if(rawNn.tensors.begin(), rawNn.tensors.end(), [&name](const TensorInfo& ti){
            return ti.name == name;
        });

        if(it == rawNn.tensors.end())
            throw std::runtime_error("Tensor does not exist");
        
        Eigen::Matrix<_Ty, _X, _Y> mat(it->dims[0], it->numDimensions);
        Eigen::Matrix2i mati;

        if (it->dataType == TensorInfo::DataType::U8F) {
            for(uint32_t i = 0; i < it->numDimensions * it->dims[0]; i++) {
                mat.data()[i] = rawNn.data.data()[it->offset + i];
            }
        }
        else {
            for(uint32_t i = 0; i < it->numDimensions * it->dims[0]; i++) { // NOTE : This will probably convert ints and floats in a weird way
                mat.data()[i] = reinterpret_cast<uint16_t*>(rawNn.data.data())[it->offset + i];
            }
        }

        return mat;
    }

     /**
     * Convenience function to retrieve values from the first tensor
     * @returns Eigen::Matrix<_Ty, _X, _Y> tensor
     */
    template<typename _Ty, int _X = Eigen::Dynamic, int _Y = Eigen::Dynamic>
    Eigen::Matrix<_Ty, _X, _Y> getFirstTensor()
    {
        if(!rawNn.tensors.empty()) {
            return getTensor<_Ty, _X, _Y>(rawNn.tensors[0].name);
        }

        return {};
    }
    };

}  // namespace dai
