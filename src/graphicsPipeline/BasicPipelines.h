#ifndef BASIC_PIPELINES_H
#define BASIC_PIPELINES_H

#include "../rendering/GraphicsPipelineManager.h"

class BasicPipelines {
private:
    void createTexturedPipeline(PipelineTemplate& pipeline);
    void createUntexturedUnlitPipeline(PipelineTemplate& pipeline);
public:
    std::vector<PipelineTemplate> pipelines;

    BasicPipelines(){
        pipelines.resize(2);
        createTexturedPipeline(pipelines[0]);
        createUntexturedUnlitPipeline(pipelines[1]);
    }

    struct PointLightUniformObject {
        glm::vec4 positions[MAX_POINT_LIGHTS];
        glm::vec4 attenuations[MAX_POINT_LIGHTS];
        glm::vec4 colours[MAX_POINT_LIGHTS];
        uint32_t count = 0;
    } pointLightUO;

};

#endif //BASIC_PIPELINES_H
