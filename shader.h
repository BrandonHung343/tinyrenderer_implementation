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
		double specCoeff;
		double diffCoeff;
		double ambiCoeff;
		int imWidth;
		Eigen::MatrixXd normalMap; // H * C transpose inverse
		Eigen::MatrixXd vp_inv; // V inverse
		Model* model;


		void set_model(Model* m);
		Eigen::Vector4d vertex(int face, int vert);
		bool frag(vec3 bary, TGAImage& diffuseMap, TGAImage& specMap, TGAColor& color);
		vec3 project(Eigen::Vector4d V); // Project 4d vector to 3d by normalizing by last coordinate
		void set_normal_map();
		void set_l();
		void initialize(Model* m, int width);
		vec3 unview(Eigen::Vector4d V0, Eigen::Vector4d V1, Eigen::Vector4d V2); // Inversely transform the vector from the viewport frame
};

