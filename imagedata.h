#pragma once
#include "geometry.h"
#include <Eigen/Dense>


class ImageData {
	protected:
		vec3 lightdir;
		vec3 origin;
		vec3 cam;
		vec3 cam_up;
		double perspective;
	public:
		ImageData(); // default constructor
		ImageData(int xC, int yC, int w, int h, int d);
		ImageData(int xC, int yC, int w, int h, int d, vec3 ld, vec3 o, vec3 c, vec3 u);
		int width;
		int height;
		int depth;
		int x;
		int y;
		Eigen::MatrixXd C;
		Eigen::MatrixXd H;
		Eigen::MatrixXd V;

		// Getter  Methods
		vec3 get_ld() { return lightdir; };
		vec3 get_origin() { return origin; };
		vec3 get_cam_origin() { return cam; };
		vec3 get_cam_up() { return cam_up; };
		double get_pers() { return perspective; };
		
		// Setter methods
		void set_ld(vec3 ld);
		void set_origin(vec3 o);
		void set_cam_origin(vec3 c);
		void set_cam_up(vec3 u);
		void warp_pers();
		void force_pers(double p);

		// Matrix form manipulations
		Eigen::MatrixXd viewport(); 
		Eigen::MatrixXd camTransform();
		Eigen::MatrixXd persTransform();
		vec3 transformV(Eigen::Vector4d v);
		
		// Shader information

};