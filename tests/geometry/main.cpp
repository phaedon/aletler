#include <cmath>
#include <iostream>
#include <geometry/TriangleMesh.h>
#include "implicitSurfaces.h"
//#include <functional>

static double g_center[3];
static double g_radius;

double sphereFn(double x, double y, double z) {
  double xyz[3] {x, y, z};
  return implicitSphere(xyz, g_center, g_radius);
}

void testSphere() {

  int meshCounter = 0;

  TriangleMesh mesh;

  double radius_low = 1; 
  double radius_high = 4;
  double radius_inc = 0.2;

  double pos_low = 0;
  double pos_high = 5;
  double pos_inc = 5;
  
  double meshBuffer = 1.0;

  for (double r = radius_low; r < radius_high; r += radius_inc) {
    g_radius = r;

    // marching cubes resolution
    double mcResolution = r * 0.05;

    for (double x = pos_low; x < pos_high; x += pos_inc) {
      for (double y = pos_low; y < pos_high; y += pos_inc) {
	for (double z = pos_low; z < pos_high; z += pos_inc) {

	  g_center[0] = x;
	  g_center[1] = y;
	  g_center[2] = z;

	  mesh.clearAll();
	  mesh.triangulateImplicitFunc(x - r - meshBuffer,
				       x + r + meshBuffer,
				       y - r - meshBuffer,
				       y + r + meshBuffer,
				       z - r - meshBuffer,
				       z + r + meshBuffer,
				       mcResolution,
				       sphereFn);

	  double meshVol = mesh.volume();
	  double analyticVol = sphereVolume(g_radius);
	  double volRatio = meshVol / analyticVol;
	  double pctErr = 100 * (volRatio - 1);
	  std::cout << "volume of sphere:  " 
		    << "MESH:" << mesh.volume() 
		    << "    ANALYTIC:" << sphereVolume(g_radius)
		    << "    PCT ERROR:" << pctErr
		    << std::endl;
				       
	  
	}
      }
    }
	
  }
  
}

int main() {

  testSphere();
  //mesh.write("data/sphere.obj");

  return 0;
}
