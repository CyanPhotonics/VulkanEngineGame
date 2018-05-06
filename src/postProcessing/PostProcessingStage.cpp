#include "PostProcessingStage.h"

const Texture &PostProcessingStage::getSourceTexture() const {
    return sourceTexture;
}

const std::array<uint32_t, 2> &PostProcessingStage::getSourceSize() const {
    return sourceSize;
}

const std::array<uint32_t, 2> &PostProcessingStage::getOutputSize() const {
    return outputSize;
}

uint32_t PostProcessingStage::getSubpassIndex() const {
    return subpassIndex;
}
