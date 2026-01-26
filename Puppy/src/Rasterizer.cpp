#include "Rasterizer.h"

bool Rasterizer::isInsideTriangle2D(const glm::vec2& p, const glm::vec2& v0,
	const glm::vec2& v1, const glm::vec2& v2) {

	auto cross = [](const glm::vec2& a, const glm::vec2& b) {
		return a.x * b.y - a.y * b.x;
		};

	glm::vec2 edge0 = v1 - v0;
	glm::vec2 edge1 = v2 - v1;
	glm::vec2 edge2 = v0 - v2;

	glm::vec2 v0p = p - v0;
	glm::vec2 v1p = p - v1;
	glm::vec2 v2p = p - v2;

	float c0 = cross(edge0, v0p);
	float c1 = cross(edge1, v1p);
	float c2 = cross(edge2, v2p);

	return (c0 >= 0 && c1 >= 0 && c2 >= 0) || (c0 <= 0 && c1 <= 0 && c2 <= 0);
}

template<typename VecType, typename FrameBufferType>
void Rasterizer::getBoundingBox(const VecType& v0, const VecType& v1, const VecType& v2,
	int& minX, int& minY, int& maxX, int& maxY, FrameBufferType framebuffer) {

	minX = static_cast<int>(std::floor(std::min({ v0.x, v1.x, v2.x })));
	minY = static_cast<int>(std::floor(std::min({ v0.y, v1.y, v2.y })));
	maxX = static_cast<int>(std::ceil(std::max({ v0.x, v1.x, v2.x })));
	maxY = static_cast<int>(std::ceil(std::max({ v0.y, v1.y, v2.y })));

	minX = std::max(0, minX);
	minY = std::max(0, minY);
	maxX = std::min(framebuffer.getWidth()-1, maxX);
	maxY = std::min(framebuffer.getHeight()-1, maxY);

}

void Rasterizer::drawTriangle2D(const glm::vec2& v0, const glm::vec2& v1,
	const glm::vec2& v2, const glm::vec3& color, Framebuffer& framebuffer) {

	int minX, minY, maxX, maxY;

	getBoundingBox(v0, v1, v2, minX, minY, maxX, maxY, framebuffer);

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			float px = x + 0.5f;
			float py = y + 0.5f;

			if (isInsideTriangle2D(glm::vec2(px, py), v0, v1, v2)) {
				framebuffer.setPixel(x, y, color);
			}
		}
	}

}

void Rasterizer::drawTriangle3D(const Triangle3D& tri, 
	const glm::mat4& modelMat, 
	const glm::mat4& viewMat,
	const glm::mat4& projectionMat, 
	Framebuffer& framebuffer,
	MathUtils::CullingMode cullMode) {

	Vertex3D v0 = tri.v0;
	Vertex3D v1 = tri.v1;
	Vertex3D v2 = tri.v2;

	glm::vec4 worldV0 = modelMat * glm::vec4(v0.position, 1);
	glm::vec4 worldV1 = modelMat * glm::vec4(v1.position, 1);
	glm::vec4 worldV2 = modelMat * glm::vec4(v2.position, 1);

	glm::vec4 viewV0 = viewMat * worldV0;
	glm::vec4 viewV1 = viewMat * worldV1;
	glm::vec4 viewV2 = viewMat * worldV2;

	if (MathUtils::isCulled(glm::vec3(viewV0), 
		glm::vec3(viewV1), 
		glm::vec3(viewV2), 
		cullMode)) {
		return;
	}

	glm::vec4 clipV0 = projectionMat * viewV0;
	glm::vec4 clipV1 = projectionMat * viewV1;
	glm::vec4 clipV2 = projectionMat * viewV2;
	
	glm::vec4 ndcV0 = clipV0 / clipV0.w;
	glm::vec4 ndcV1 = clipV1 / clipV1.w;
	glm::vec4 ndcV2 = clipV2 / clipV2.w;

	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();

	glm::vec3 screenV0 = MathUtils::viewportTransform(glm::vec3(ndcV0), width, height);
	glm::vec3 screenV1 = MathUtils::viewportTransform(glm::vec3(ndcV1), width, height);
	glm::vec3 screenV2 = MathUtils::viewportTransform(glm::vec3(ndcV2), width, height);

	int minX, minY, maxX, maxY;
	getBoundingBox(screenV0, screenV1, screenV2, minX, minY, maxX, maxY, framebuffer);

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {

			glm::vec2 pixel = glm::vec2(x + 0.5f, y + 0.5f);
			glm::vec3 bary = MathUtils::barycentric2D(screenV0, screenV1, screenV2, pixel);
			
			float alpha = bary.x;
			float beta = bary.y;
			float gamma = bary.z;

			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) {  // 点在三角形内
				float z = 1/(alpha / clipV0.w + beta / clipV1.w + gamma / clipV2.w);  // 视图空间中的三角形这一点的z值
				
				// 将ndc的z值当作视图空间中三角形的属性进行插值
				float ndcZ = z * (alpha * ndcV0.z / clipV0.w +
					beta * ndcV1.z / clipV1.w +
					gamma * ndcV2.z / clipV2.w);
 
				float depth = ndcZ * 0.5f + 0.5f; // 映射到[0, 1]，0近1远（ndc与view空间的深度方向相反）
				
				if (framebuffer.depthTest(x, y, depth)) {
					// 透视矫正插值
					float r = z * (alpha * v0.color.r / clipV0.w + 
						beta * v1.color.r / clipV1.w +
						gamma * v2.color.r / clipV2.w);

					float g = z * (alpha * v0.color.g / clipV0.w + 
						beta * v1.color.g / clipV1.w + 
						gamma * v2.color.g / clipV2.w);

					float b = z * (alpha * v0.color.b / clipV0.w + 
						beta * v1.color.b / clipV1.w + 
						gamma * v2.color.b / clipV2.w);
					
					framebuffer.setPixel(x, y, { r,g,b });
				}

			}
		}
	}


}

void Rasterizer::drawTriangle3D(const Triangle3D& tri,
	const Shader& shader,
	const ShaderContext& context,
	Framebuffer& framebuffer,
	MathUtils::CullingMode cullMode) {

	Vertex3D v0 = tri.v0;
	Vertex3D v1 = tri.v1;
	Vertex3D v2 = tri.v2;

	glm::vec4 worldV0 = context.modelMatrix * glm::vec4(v0.position, 1);
	glm::vec4 worldV1 = context.modelMatrix * glm::vec4(v1.position, 1);
	glm::vec4 worldV2 = context.modelMatrix * glm::vec4(v2.position, 1);

	glm::vec4 viewV0 = context.viewMatrix * worldV0;
	glm::vec4 viewV1 = context.viewMatrix * worldV1;
	glm::vec4 viewV2 = context.viewMatrix * worldV2;

	if (MathUtils::isCulled(glm::vec3(viewV0),
		glm::vec3(viewV1),
		glm::vec3(viewV2),
		cullMode)) {
		return;
	}

	// 顶点着色
	VertexShaderOutput vo0 = shader.vertexShader(v0, context);
	VertexShaderOutput vo1 = shader.vertexShader(v1, context);
	VertexShaderOutput vo2 = shader.vertexShader(v2, context);

	//透视除法
	glm::vec3 ndc0 = glm::vec3(vo0.clipPos) / vo0.clipPos.w;
	glm::vec3 ndc1 = glm::vec3(vo1.clipPos) / vo1.clipPos.w;
	glm::vec3 ndc2 = glm::vec3(vo2.clipPos) / vo2.clipPos.w;


	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();

	glm::vec3 screen0 = MathUtils::viewportTransform(ndc0, width, height);
	glm::vec3 screen1 = MathUtils::viewportTransform(ndc1, width, height);
	glm::vec3 screen2 = MathUtils::viewportTransform(ndc2, width, height);

	int minX, minY, maxX, maxY;
	getBoundingBox(screen0, screen1, screen2, minX, minY, maxX, maxY, framebuffer);

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {

			glm::vec2 pixel = glm::vec2(x + 0.5f, y + 0.5f);
			glm::vec3 bary = MathUtils::barycentric2D(screen0, screen1, screen2, pixel);

			float alpha = bary.x;
			float beta = bary.y;
			float gamma = bary.z;

			if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) {  // 点在三角形内
				float interpolatedOneOverW = alpha * vo0.oneOverW + beta * vo1.oneOverW + gamma * vo2.oneOverW;  // 插值1/w

				// 将ndc的z值当作视图空间中三角形的属性进行插值
				float ndcZ = (alpha * ndc0.z * vo0.oneOverW +
					beta * ndc1.z * vo1.oneOverW +
					gamma * ndc2.z * vo2.oneOverW) / interpolatedOneOverW;

				float depth = ndcZ * 0.5f + 0.5f; // 映射到[0, 1]，0近1远（ndc与view空间的深度方向相反）

				if (framebuffer.depthTest(x, y, depth)) {
					// 插值顶点属性
					FragmentShaderInput fragment = interpolateVertex(vo0, vo1, vo2, bary, interpolatedOneOverW, context);

					glm::vec3 color = shader.fragmentShader(fragment, context);

					framebuffer.setPixel(x, y, color);
				}

			}
		}
	}
}

void Rasterizer::drawTriangle3D(const Triangle3D& tri,
	const Shader& shader,
	const ShaderContext& context,
	MultisampleFramebuffer& framebuffer,
	MathUtils::CullingMode cullMode) {

	Vertex3D v0 = tri.v0;
	Vertex3D v1 = tri.v1;
	Vertex3D v2 = tri.v2;

	glm::vec4 worldV0 = context.modelMatrix * glm::vec4(v0.position, 1);
	glm::vec4 worldV1 = context.modelMatrix * glm::vec4(v1.position, 1);
	glm::vec4 worldV2 = context.modelMatrix * glm::vec4(v2.position, 1);

	glm::vec4 viewV0 = context.viewMatrix * worldV0;
	glm::vec4 viewV1 = context.viewMatrix * worldV1;
	glm::vec4 viewV2 = context.viewMatrix * worldV2;

	if (MathUtils::isCulled(glm::vec3(viewV0),
		glm::vec3(viewV1),
		glm::vec3(viewV2),
		cullMode)) {
		return;
	}

	// 顶点着色
	VertexShaderOutput vo0 = shader.vertexShader(v0, context);
	VertexShaderOutput vo1 = shader.vertexShader(v1, context);
	VertexShaderOutput vo2 = shader.vertexShader(v2, context);

	//透视除法
	glm::vec3 ndc0 = glm::vec3(vo0.clipPos) / vo0.clipPos.w;
	glm::vec3 ndc1 = glm::vec3(vo1.clipPos) / vo1.clipPos.w;
	glm::vec3 ndc2 = glm::vec3(vo2.clipPos) / vo2.clipPos.w;


	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();

	glm::vec3 screen0 = MathUtils::viewportTransform(ndc0, width, height);
	glm::vec3 screen1 = MathUtils::viewportTransform(ndc1, width, height);
	glm::vec3 screen2 = MathUtils::viewportTransform(ndc2, width, height);

	int minX, minY, maxX, maxY;
	getBoundingBox(screen0, screen1, screen2, minX, minY, maxX, maxY, framebuffer);

	const auto& sampleOffsets = framebuffer.getSampleOffsets();
	int numSamples = framebuffer.getSamplesPerPixel();

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {

			for (int s = 0; s < numSamples; s++) {

				const glm::vec2& offset = sampleOffsets[s];
				
				float sampleX = x + 0.5f + offset.x;
				float sampleY = y + 0.5f + offset.y;

				glm::vec2 sample(sampleX, sampleY);

				glm::vec3 bary = MathUtils::barycentric2D(screen0, screen1, screen2, sample);

				float alpha = bary.x;
				float beta = bary.y;
				float gamma = bary.z;

				if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) {  // 采样点在三角形内
					float interpolatedOneOverW = alpha * vo0.oneOverW + beta * vo1.oneOverW + gamma * vo2.oneOverW;  // 插值1/w
					
					// 将ndc的z值当作视图空间中三角形的属性进行插值
					float ndcZ = (alpha * ndc0.z * vo0.oneOverW +
						beta * ndc1.z * vo1.oneOverW +
						gamma * ndc2.z * vo2.oneOverW) / interpolatedOneOverW;

					float depth = ndcZ * 0.5f + 0.5f; // 映射到[0, 1]，0近1远（ndc与view空间的深度方向相反）

					if (framebuffer.sampleDepthTest(x, y, s, depth)) {
						// 插值顶点属性
						FragmentShaderInput fragment = interpolateVertex(vo0, vo1, vo2, bary, interpolatedOneOverW, context);

						glm::vec3 color = shader.fragmentShader(fragment, context);

						framebuffer.setSample(x, y, s, color);
					}
				}
			}
		}
	}
}

FragmentShaderInput Rasterizer::interpolateVertex(const VertexShaderOutput& vo0,
	const VertexShaderOutput& vo1,
	const VertexShaderOutput& vo2,
	const glm::vec3& barycentric,
	float interpolatedOneOverW,
	const ShaderContext& context) {

	FragmentShaderInput result;

	// 使用着色器的插值函数
	result.worldPos = Shader::interpolateAttribute(
		vo0.worldPos, vo1.worldPos, vo2.worldPos, 
		barycentric, 
		vo0.oneOverW, vo1.oneOverW, vo2.oneOverW, 
		interpolatedOneOverW, context.usePerspective);

	result.worldNorm = Shader::interpolateAttribute(
		vo0.worldNorm, vo1.worldNorm, vo2.worldNorm,
		barycentric, 
		vo0.oneOverW, vo1.oneOverW, vo2.oneOverW, 
		interpolatedOneOverW, context.usePerspective);

	result.texcoord = Shader::interpolateAttribute(
		vo0.texcoord, vo1.texcoord, vo2.texcoord,
		barycentric, 
		vo0.oneOverW, vo1.oneOverW, vo2.oneOverW, 
		interpolatedOneOverW, context.usePerspective);

	result.color = Shader::interpolateAttribute(
		vo0.color, vo1.color, vo2.color,
		barycentric, 
		vo0.oneOverW, vo1.oneOverW, vo2.oneOverW, 
		interpolatedOneOverW, context.usePerspective);

	return result;
}