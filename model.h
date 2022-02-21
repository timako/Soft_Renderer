#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "Eigen/Eigen"
class Model {
private:
	std::vector<Eigen::Vector3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<Eigen::Vector2f> vts_;
	std::vector<std::vector<int>> face_vts_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nvts();
	Eigen::Vector3f vert(int i);
	std::vector<int> face(int idx);
	Eigen::Vector2f vt(int i);
	std::vector<int> vt_coords(int i);
	void Trans(Eigen::Matrix4f T);
};

#endif //__MODEL_H__
