#include "pch.hpp"
#include "GraphicsPipeline.hpp"

namespace ktw {
	GraphicsPipeline::GraphicsPipeline(ktw::Device& device, ktw::IRenderTarget& renderTarget, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, uint32_t vertexSize, std::vector<ktw::AttributeDescription>& attributeDescriptions) {
		ktw::Shader vertexShader(device, vertexShaderFile);
		ktw::Shader fragmentShader(device, fragmentShaderFile);

		auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(*(vertexShader.getModule()))
			.setPName("main");

		auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(*(fragmentShader.getModule()))
			.setPName("main");

		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		auto bindingDescription = vk::VertexInputBindingDescription()
			.setBinding(0)
			.setStride(vertexSize)
			.setInputRate(vk::VertexInputRate::eVertex);

		std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions(attributeDescriptions.size());
		for(int i=0; i<attributeDescriptions.size(); i++) {
			vertexInputAttributeDescriptions[i]
				.setBinding(0)
				.setLocation(attributeDescriptions[i].location)
				.setFormat((vk::Format) attributeDescriptions[i].format)
				.setOffset(attributeDescriptions[i].offset);
		}

		auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptionCount(1)
			.setPVertexBindingDescriptions(&bindingDescription)
			.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexInputAttributeDescriptions.size()))
			.setPVertexAttributeDescriptions(vertexInputAttributeDescriptions.data());

		auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(false);

		auto viewport = vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth((float) renderTarget.getExtent().width)
			.setHeight((float) renderTarget.getExtent().height)
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);

		auto scissor = vk::Rect2D()
			.setOffset({0, 0})
			.setExtent(renderTarget.getExtent());

		auto viewportState = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setPViewports(&viewport)
			.setScissorCount(1)
			.setPScissors(&scissor);

		auto rasterizer = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(false)
			.setRasterizerDiscardEnable(false)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)
			.setDepthBiasEnable(false)
			.setDepthBiasConstantFactor(0.0f) // Optional
			.setDepthBiasClamp(0.0f) // Optional
			.setDepthBiasSlopeFactor(0.0f); // Optional

		auto multisampling = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(false)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1)
			.setMinSampleShading(1.0f) // Optional
			.setPSampleMask(nullptr) // Optional
			.setAlphaToCoverageEnable(false) // Optional
			.setAlphaToOneEnable(false); // Optional

		auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(
				vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA
			)
			.setBlendEnable(false)
			.setSrcColorBlendFactor(vk::BlendFactor::eOne) // Optional
			.setDstColorBlendFactor(vk::BlendFactor::eZero) // Optional
			.setColorBlendOp(vk::BlendOp::eAdd) // Optional
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne) // Optional
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero) // Optional
			.setAlphaBlendOp(vk::BlendOp::eAdd); // Optional

		auto colorBlending = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(false)
			.setLogicOp(vk::LogicOp::eCopy) // Optional
			.setAttachmentCount(1)
			.setPAttachments(&colorBlendAttachment)
			.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

		vk::DynamicState dynamicStates[] = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};

		auto dynamicState = vk::PipelineDynamicStateCreateInfo()
			.setDynamicStateCount(2)
			.setPDynamicStates(dynamicStates);

		auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(0) // Optional
			.setPSetLayouts(nullptr) // Optional
			.setPushConstantRangeCount(0) // Optional
			.setPPushConstantRanges(nullptr); // Optional

		pipelineLayout = device.getDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

		auto pipelineInfo = vk::GraphicsPipelineCreateInfo()
			.setStageCount(2)
			.setPStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssembly)
			.setPViewportState(&viewportState)
			.setPRasterizationState(&rasterizer)
			.setPMultisampleState(&multisampling)
			.setPDepthStencilState(nullptr) // Optional
			.setPColorBlendState(&colorBlending)
			.setPDynamicState(nullptr) // Optional
			.setLayout(*pipelineLayout)
			.setRenderPass(renderTarget.getRenderPass())
			.setSubpass(0)
			.setBasePipelineHandle(nullptr) // Optional
			.setBasePipelineIndex(-1); // Optional

		pipeline = (device.getDevice().createGraphicsPipelineUnique(nullptr, pipelineInfo)).value;
		LOG_TRACE("Graphics Pipeline Created");
	}

	vk::Pipeline& GraphicsPipeline::getPipeline() {
		return *pipeline;
	}
}