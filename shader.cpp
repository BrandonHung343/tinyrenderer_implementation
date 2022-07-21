#include "shader.h"

Eigen::Vector4d Shader::vertex(int face, int vert) {
	assert(model != nullptr);
	assert((vert < 3) && (vert >= 0) && (face >= 0) && (face <= model->nfaces()));
	vec3 v = model->vert(face, vert);
	vec2 uv = model->uv(face, vert);
	textureX[vert] = uv.x;
	textureY[vert] = uv.y;
	Eigen::Vector4d v4(v.x, v.y, -v.z, 1.0 );
	return V * H * C * v4;
}

// Rotate the normals as well
bool Shader::frag(vec3 bary, TGAImage& diffuseMap, TGAImage& specMap, TGAColor& color) {
	// Calculate the texture mapped xy coordinates
	vec2 baryUV = vec2{ textureX * bary, textureY * bary };
	int tX = baryUV.x * diffuseMap.width();
	int tY = baryUV.y * diffuseMap.height();

	// Calculate the transformed normal 
	vec3 normal = model->normal(baryUV);
	Eigen::Vector4d n4(normal.x, normal.y, normal.z, 0.0);
	vec3 n = project(normalMap * n4).normalize();

	// Calculate the specular reflections
	vec3 reflect = (n * (n * l) * 2 - l).normalize();
	TGAColor specular = specMap.get(baryUV.x * specMap.width(), baryUV.y * diffuseMap.height());
	// TGAColor specular = specMap.get(tX, tY);
	double spec = std::pow(std::max(0.0, -reflect.z), specular.bgra[0]);


	// Calculate the diffuse lighting
	TGAColor diffuse = diffuseMap.get(tX, tY);
	double diff = std::max(0.0, n * l);

	// Push the values into the color
	for (int i = 0; i < 3; i++) {
		color[i] = std::min(ambiCoeff + diffuse[i] * (diffCoeff * diff + specCoeff * spec), 255.0);
	}
	return false;
}

vec3 Shader::project(Eigen::Vector4d V) {
	if (V[3] != 0.0) {
		return vec3{ V[0] / V[3], V[1] / V[3], V[2] / V[3] };
	}
	return vec3{ V[0], V[1], V[2] };
}

void Shader::set_model(Model* m) {
	model = m;
}

void Shader::set_normal_map() {
	normalMap = (H * C).transpose().inverse();
}

void Shader::set_l() {
	Eigen::Vector4d l4(lightdir.x, lightdir.y, lightdir.z, 0.0);
	l = project(H * C * l4).normalize();
}

void Shader::initialize(Model* m, int width) {
	model = m;
	normalMap = (H * C).inverse();
	Eigen::Vector4d l4(lightdir.x, lightdir.y, lightdir.z, 0.0);
	l = project(H * C * l4).normalize();
	vp_inv = viewport().inverse();
	specCoeff = 1.0;
	diffCoeff = 1.0;
	ambiCoeff = 10.0;
	imWidth = width;
}


vec3 Shader::unview(Eigen::Vector4d V0, Eigen::Vector4d V1, Eigen::Vector4d V2) {
	return vec3{(vp_inv * V0)[2], (vp_inv * V1)[2], (vp_inv * V2)[2] };
}