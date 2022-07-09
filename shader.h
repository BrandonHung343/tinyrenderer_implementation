#pragma once
#include "imagedata.h"
#include <Eigen/Dense>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

class Shader : public ImageData {
	public:
		using ImageData::ImageData;
		vec3 textureX;
		vec3 textureY;
		vec3 l;
		Eigen::MatrixXd normalMap;
		Eigen::MatrixXd vp_inv;
		Model* model;
		void set_model(Model* m);
		Eigen::Vector4d vertex(int face, int vert);
		bool frag(vec3 bary, TGAImage& diffuseMap, TGAColor& color);
		vec3 project(Eigen::Vector4d V);
		void set_normal_map();
		void set_l();
		void initialize(Model* m);
		vec3 unview(Eigen::Vector4d V0, Eigen::Vector4d V1, Eigen::Vector4d V2);
};

