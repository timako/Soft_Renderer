#include <vector>
#include <cmath>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "Eigen/Eigen"
#include "gl_transformation.h"
#define _pi 3.14159
using namespace Eigen;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;
float z_depth[width][height];
void init() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			z_depth[i][j] = -1*FLT_MAX;
		}
	}
}
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}
void Triangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2, 
	int y2, float z2,Vector2i tex_cood0, Vector2i tex_cood1, Vector2i tex_cood2,
	TGAImage &image, TGAImage texture, float grad) {
	int minx = std::min({ x0,x1,x2 });
	int maxx = std::max({ x0,x1,x2 });
	int miny = std::min({ y0,y1,y2 });
	int maxy = std::max({ y0,y1,y2 });
	for (int x = minx; x <= maxx; x++) {
		for (int y = miny; y <= maxy; y++) {
			float alpha = (-1.0*(x - x1)*(y2 - y1) + (y - y1)*(x2 - x1)) / ((-1.0*(x0 - x1)*(y2 - y1) + (y0 - y1)*(x2 - x1)));
			float beta = (-1.0*(x - x2)*(y0 - y2) + (y - y2)*(x0 - x2)) / ((-1.0*(x1 - x2)*(y0 - y2) + (y1 - y2)*(x0 - x2)));
			float gama = 1 - alpha - beta;
			if (alpha>=0&&beta>=0&& gama>=0) {
				float z = alpha * z0 + beta * z1 + gama * z2;
				int tex_x= (int)(alpha * tex_cood0[0] + beta * tex_cood1[0] + gama * tex_cood2[0]);
				int tex_y = (int)(alpha * tex_cood0[1] + beta * tex_cood1[1] + gama * tex_cood2[1]);
				TGAColor color = texture.get(tex_x, tex_y);
				color.r = color.r*grad;
				color.g = color.g*grad;
				color.b = color.b*grad;
				if (z > z_depth[x][y]) {
					image.set(x, y, color);
					z_depth[x][y] = z;
				}
			}
		}
	}
}
int main(int argc, char** argv) {
	init();
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
	TGAImage texture;
	texture.read_tga_file("african_head_diffuse.tga");
	texture.flip_vertically();
	int wid_tex = texture.get_width();
	int hei_tex = texture.get_height();
	TransMat trans;
	trans.M_set(Vector3f(0, 1, 0), _pi, Vector3f(0, -1, -1), Vector3f(1, 1, 1));
	trans.V_set(Vector3f(1, 0, 0),
		Vector3f(0, cos(_pi/6), -sin(_pi/6)),
		Vector3f(0, sin(_pi/6) , cos(_pi/6)), Vector3f(0, 0, 1));
	trans.P_set(-1, 1, -1, 1, -1, 1);
	Matrix4f T = trans.trans();
	model->Trans(T);
	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); //vertexs
		std::vector<int> vt_inds = model->vt_coords(i);
		Vector3f face_vector;
		Vector2i screen_coords[3];
		float screen_depth[3];
		Vector2i vt_coords[3];
		Vector3f norm_line;
		for (int j=0; j<3; j++) { 
			Vector3f world_coords = model->vert(face[j]);
			Vector2f world_coords_tex = model->vt(vt_inds[j]);
			screen_coords[j] = Vector2i((world_coords[0]+1.)*width/2., (world_coords[1]+1.)*height/2.);
			screen_depth[j] = world_coords[2];
			vt_coords[j] = Vector2i(world_coords_tex[0]*wid_tex, world_coords_tex[1] * hei_tex);
		} 
		Vector3f vec1 = Vector3f(model->vert(face[0])[0] - model->vert(face[1])[0], model->vert(face[0])[1] - model->vert(face[1])[1], model->vert(face[0])[2] - model->vert(face[1])[2]);
		Vector3f vec2 = Vector3f(model->vert(face[0])[0] - model->vert(face[2])[0], model->vert(face[0])[1] - model->vert(face[2])[1], model->vert(face[0])[2] - model->vert(face[2])[2]);
		norm_line = (vec1.cross( vec2)).normalized();
		float len = (norm_line .dot( Vector3f(0, 0, 1)));
		if(len>0)
		Triangle(screen_coords[0][0],screen_coords[0][1], screen_depth[0], screen_coords[1][0], 
			screen_coords[1][1], screen_depth[1], screen_coords[2][0], screen_coords[2][1],
			screen_depth[2], vt_coords[0], vt_coords[1], vt_coords[2],image, texture,len * len);
	}
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

