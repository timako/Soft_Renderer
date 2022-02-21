#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() , face_vts_(),vts_(){
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Eigen::Vector3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
			std::vector<int> vt;
            int itrash, idx,vt_ind;
            iss >> trash;
            while (iss >> idx >> trash >> vt_ind >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
				vt_ind--;
                f.push_back(idx);
				vt.push_back(vt_ind);
            }
            faces_.push_back(f);
			face_vts_.push_back(vt);
        }
		else if (!line.compare(0, 3, "vt ")) {
			int itrash;
			iss >> trash>>trash;
			Eigen::Vector2f vt;
			iss >> vt[0] >> vt[1] >> itrash;
			vts_.push_back(vt);
		}
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() <<" vt# "<< vts_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}
int Model::nvts() {
	return (int)vts_.size();
}
std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Eigen::Vector3f Model::vert(int i) {
    return verts_[i];
}
Eigen::Vector2f Model::vt(int i) {
	return vts_[i];
}
std::vector<int>  Model::vt_coords(int i) {
	return face_vts_[i];
}
void Model::Trans(Eigen::Matrix4f T) {
	for (int i = 0; i < verts_.size(); i++) {
		Eigen::Vector3f v = verts_[i];
		Eigen::Vector4f V;
		V << v,
			 1;
		V = T * V;
		V[0] /= V[3]; V[1] /= V[3]; V[2] /= V[3];
		v[0] = V[0]; v[1] = V[1]; v[2] = V[2];
		verts_[i] = v;
	}
}