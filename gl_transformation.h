#ifndef __GL_TRANSFORMATION_H__
#define __GL_TRANSFORMATION_H__
#include<Eigen/Eigen>
using namespace Eigen;
class TransMat {
private:
	Matrix4f M, V, P;
public:
	Matrix4f mat(int width,int height);
	Matrix4f trans();
	void M_set(Vector3f n, float alpha,Vector3f delta, Vector3f scale);
	void V_set(Vector3f u,Vector3f v, Vector3f w, Vector3f pos);
	void P_set(float l,float r,float b,float t,float n, float f);
};


#endif
//define __GL_TRANSFORMATION_H__