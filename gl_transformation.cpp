#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "gl_transformation.h"

Matrix4f TransMat::mat(int width, int height) {
	Matrix4f Viewport;
	Viewport << 
		width / 2.0, 0, 0, width / 2.0,
		0, height / 2.0, 0, height / 2.0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return Viewport * P * V * M;
}
Matrix4f TransMat::trans() {
	return P * V * M;
}
void TransMat::M_set(Vector3f n, float alpha, Vector3f delta, Vector3f scale) {
	Matrix4f Rotate, Translation, Scale;
	Matrix3f I;
	I << 1, 0, 0,
		 0, 1, 0,
		 0, 0, 1;
	Matrix3f R_n;
	R_n << 
		0, -n[2], n[1],
		n[2], 0, -n[0],
		-n[1], n[0], 0;
	Matrix3f R = I * std::cos(alpha) + n*n.transpose()*(1 - std::cos(alpha)) + R_n * std::sin(alpha);
	Matrix<float, 3, 1> add_col;
	add_col << 
		0,
		0,
		0;
	Matrix<float, 3, 4>added_col;
	added_col << R , add_col;
	Matrix<float, 1, 4> add_row;
	add_row << 
		0, 0, 0, 1;
	Rotate << added_col,
			  add_row;
	Translation << 
		1, 0, 0, delta[0],
		0, 1, 0, delta[1],
		0, 0, 1, delta[2],
		0, 0, 0, 1;
	Scale << 
		scale[0], 0, 0, 0,
		0, scale[1], 0, 0,
		0, 0, scale[2], 0,
		0, 0, 0, 1;
	M = Translation * Rotate * Scale;
}
void TransMat::V_set(Vector3f u, Vector3f v, Vector3f w, Vector3f pos) {
	Matrix4f R_view;
	R_view << 
		u[0], u[1], u[2], 0,
		v[0], v[1], v[2], 0,
		w[0], w[1], w[2], 0,
		0, 0, 0, 1;
	Matrix4f T_view;
	T_view << 
		1, 0, 0, -pos[0],
		0, 1, 0, -pos[1],
		0, 0, 1, -pos[2],
		0, 0, 0, 1;
	V = R_view * T_view;
}
void TransMat::P_set(float l, float r, float b, float t, float n, float f) {
	Matrix4f M_ortho, M_presp;
	M_presp << 
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, 1, 0;
	M_ortho << 
		2.0 / (r - l), 0, 0, -(r + l) / (r - l),
		0, 2.0 / (t - b), 0, -(t + b) / (t - b),
		0, 0, 2.0 / (n - f), -(n + f) / (n - f),
		0, 0, 0, 1;
	P = M_ortho * M_presp;
}