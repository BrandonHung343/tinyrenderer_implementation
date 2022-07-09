#include "imagedata.h"

ImageData::ImageData() {
	width = 800;
	height = 800;
	x = width / 2;
	y = width / 2;
	depth = 255;
	lightdir = vec3{ 0.0, 0.0, 1.0 };
	origin = vec3{ 0.0, 0.0, 0.0 };
	cam = vec3{ 0.0, 0.0, 1.0 };
	cam_up = vec3{ 0.0, 1.0, 0.0 };
	warp_pers();
	V = viewport();
	H = persTransform();
	C = camTransform();
}


ImageData::ImageData(int xC, int yC, int w, int h, int d) {
	x = xC;
	y = yC;
	width = w;
	height = h;
	depth = d;
	lightdir = vec3{ 0.0, 0.0, 1.0 };
	origin = vec3{ 0.0, 0.0, 0.0 };
	cam = vec3{ 0.0, 0.0, 1.0 };
	cam_up = vec3{ 0.0, 1.0, 0.0 };
	warp_pers();
	V = viewport();
	H = persTransform();
	C = camTransform();
}


ImageData::ImageData(int xC, int yC, int w, int h, int d, vec3 ld, vec3 o, vec3 c, vec3 u) {
	x = xC;
	y = yC;
	width = w;
	height = h;
	depth = d;
	lightdir = vec3{ 0.0, 0.0, 1.0 };
	set_ld(ld);
	origin = vec3{ 0.0, 0.0, 0.0 };
	set_origin(o);
	cam = vec3{ 0.0, 0.0, 1.0 };
	set_cam_origin(c);
	cam_up = vec3{ 0.0, 1.0, 0.0 };
	set_cam_up(u);
	warp_pers();
	V = viewport();
	H = persTransform();
	C = camTransform();
}


void ImageData::set_ld(vec3 ld) {
	lightdir.x = ld.x;
	lightdir.y = ld.y;
	lightdir.z = ld.z;
	lightdir.normalize();
}


void ImageData::set_origin(vec3 o) {
	origin.x = o.x;
	origin.y = o.y;
	origin.z = o.z;
	warp_pers();
	C = camTransform();
}


void ImageData::set_cam_origin(vec3 c) {
	cam.x = c.x;
	cam.y = c.y;
	cam.z = c.z;
	warp_pers();
	C = camTransform();
}


void ImageData::set_cam_up(vec3 u) {
	cam_up.x = u.x;
	cam_up.y = u.y;
	cam_up.z = u.z;
	C = camTransform();
}


void ImageData::warp_pers() {
	double normVal = (cam - origin).norm();
	if (normVal == 0.0) {
		perspective = 0.0;
	}
	else {
		perspective = -1.0 / normVal;
	}
	// Update the perspective matrix
	H = persTransform();
}


void ImageData::force_pers(double p) {
	if (p == 0.0) {
		perspective = 0.0;
	}
	else {
		perspective = -1.0 / p;
	}
	// Update the perspective matrix
	H = persTransform();
}


Eigen::MatrixXd ImageData::viewport() {
	/* Get a viewport matrix representing the projection of the item cube into our 
	   screen cube. */
	Eigen::MatrixXd vp = Eigen::MatrixXd::Identity(4, 4);
	vp(0, 0) = width / 2.0;
	vp(1, 1) = height / 2.0;
	vp(2, 2) = depth / 2.0;
	vp(0, 3) = x + vp(0, 0);
	vp(1, 3) = y + vp(1, 1);
	vp(2, 3) = vp(2, 2);
	return vp;
}


Eigen::MatrixXd ImageData::camTransform() {
	/* Given a camera point(cx, cy, cz), a point representing the origin of the rendered object's frame,
	   a vector pointing vertically in the camera frame (ux, uy, yz), return a matrix
	   which represents the homogeneous SE(3) transformation from the original frame to the camera frame */

	Eigen::MatrixXd Minv = Eigen::MatrixXd::Identity(4, 4);
	Eigen::MatrixXd T = Eigen::MatrixXd::Identity(4, 4);

	// First, we compute the three vectors we need. First is a basic vector from our point to the origin
	vec3 hz = cam - origin;
	hz.normalize();

	// Now, we compute two ortho ones using cross products and remembering which way is up
	vec3 hx = cross(cam_up, hz);
	hx.normalize();

	// The last one is just going to be proejcted on to up. We can't use up here, because cross/wedge aren't associative
	// despite being anti-commutative. Also, u and CE aren't always orthogonal
	vec3 hy = cross(hz, hx);
	hy.normalize();

	// Make the matrices, one rotation and one translation
	for (int i = 0; i < 3; i++) {
		Minv(0, i) = hx[i];
		Minv(1, i) = hy[i];
		Minv(2, i) = hz[i];
		T(i, 3) = -cam[i];
	}
	return Minv * T;
}


Eigen::MatrixXd ImageData::persTransform() {
	Eigen::MatrixXd hp = Eigen::MatrixXd::Identity(4, 4);
	hp(3, 2) = get_pers();
	return hp;
}


vec3 ImageData::transformV(Eigen::Vector4d v) {
	Eigen::Vector4d p = V * H * C * v;
	p /= p(3);
	return vec3{p[0], p[1], p[2]};
}


/* Eigen::Vector4d ImageData::transformN(Eigen::Vector4d n) {
	return C.transpose().llt().solve(n);
} */


