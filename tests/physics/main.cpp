//
//  main.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <physics/Bubble.h>
#include <physics/Electrostatics.h>
#include <iostream>

#include "vdb.h"

#include <sound/Monopole.h>
#include <sound/SoundTrack.h>
#include <sound/SoundFileManager.h>

/******** BOOST STUFF **********/
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric::odeint;
void rhs( const double x , double &dxdt , const double t )
{
  dxdt = 3.0/(2.0*t*t) + x/(2.0*t);
}

void write_cout( const double &x , const double t )
{
  cout << t << '\t' << x << endl;
}

// state_type = double
typedef runge_kutta_dopri5< double > stepper_type;

void boostmain()
{
  double x = 0.0;
  integrate_adaptive( make_controlled( 1E-12 , 1E-12 , stepper_type() ) ,
                     rhs , x , 1.0 , 10.0 , 0.1 , write_cout );
}
/******** BOOST STUFF **********/


int main() {

  
  std::string bubbledir = "/Users/phaedon/github/bem-laplace-simple/meshes/";
  std::string meshdir = "/Users/phaedon/github/aletler/meshes/";
  
  
  TriangleMesh bubblemesh, freesurfacemesh, solidsurfacemesh;
  
 
  //bubblemesh.read(meshdir + "plate_bubble.obj", MFF_OBJ);
  //freesurfacemesh.read(meshdir + "plate_freesurface.obj", MFF_OBJ);
   /*
  solidsurfacemesh.read(meshdir + "insulators.obj", MFF_OBJ);
   */
  
  bubblemesh.read(bubbledir + "sphere3.obj", MFF_OBJ);
  //freesurfacemesh.read(bubbledir + "plane_h4_s16_t200.obj", MFF_OBJ);

  //solidsurfacemesh.read(bubbledir + "plane_h128_s16_t200.obj", MFF_OBJ);

  
  //bubblemesh.read(bubbledir + "sphere_5mm_h95cm.obj", MFF_OBJ);
  //solidsurfacemesh.read(bubbledir + "plane_h1_s100mm_t2048.obj", MFF_OBJ);

  //solidsurfacemesh.read(bubbledir + "plane_h1_s100mm_t2048.obj", MFF_OBJ);
  //solidsurfacemesh.read(bubbledir + "insulators.obj", MFF_OBJ);
  
  
  // TEST INTEGRATOR
  
  double sa = bubblemesh.surfaceArea();
  
  
  Electrostatics e;
  e.setBubble(&bubblemesh);
  e.setSurface(&freesurfacemesh);
  e.setSolid(&solidsurfacemesh);
  
  bubblemesh.flipNormals();
  
  for (size_t i = 0; i < bubblemesh.size(); i++) {
    Triangle t = bubblemesh.triangle(i);
    
    std::cout << "TRIANGLE AREA " << i << ":  " << t.area() << std::endl;
    
    Vector3d c = t.centroid();
    vdb_color(0, 1, 0);
    vdb_point(c.x(), c.y(), c.z());
    
    Vector3d n = 1.0 * t.normal().normalized();
    vdb_color(1, 0, 0);
    /*vdb_line(c.x(), c.y(), c.z(),
             c.x() + n.x(), c.y() + n.y(), c.z() + n.z());
     */
  }
  
  for (size_t i = 0; i < freesurfacemesh.size(); i++) {
    Triangle t = freesurfacemesh.triangle(i);
    Vector3d c = t.centroid();
    vdb_color(0, 1, 0);
    vdb_point(c.x(), c.y(), c.z());
    
    Vector3d n = 1.0 * t.normal().normalized();
    vdb_color(1, 0, 0);
    /*
    vdb_line(c.x(), c.y(), c.z(),
             c.x() + n.x(), c.y() + n.y(), c.z() + n.z());
     */
  }
  
  
  std::cout << "computing dirichlet matrix" << std::endl;
  e.computeDirichletMatrix();
  
  std::cout << "computing neumann matrix" << std::endl;
  e.computeNeumannMatrix();
  
  std::cout << "computing combined matrix" << std::endl;
  e.computeCombinedMatrix();
  
  std::cout << "solving linear system" << std::endl;
  e.solveLinearSystem();
  
  std::cout << "bubble capacitance!    " << e.bubbleCapacitance() << std::endl;
  
  
  return 1;
  
  // FIELD EVALUATION HERE
  /*
  MatrixXd fld(41, 1);
  
  for (int i = -20; i <= 20; i++) {
    Vector3d pt(i * 0.25, 0, 0);
    fld(i + 20) = e.evaluateField(pt);
  }
  
  double fldmin = fld.minCoeff();
  double fldmax = fld.maxCoeff();
  for (int i = -20; i <= 20; i++) {
    Vector3d pt(i * 0.25, 0, 0);

    double fldval = fld(i + 20);
    std::cout << "field value at x=" << pt.x()  << " :  " << fldval << std::endl;
    double color = (fldval - fldmin) / (fldmax - fldmin);
    vdb_color(0, 0, color);
    vdb_point(pt.x(), pt.y(), pt.z());
  }
   */
  
  /*
  size_t nb = bubblemesh.size();
  size_t nfs = freesurfacemesh.size();
  size_t n = bubblemesh.size() + freesurfacemesh.size() + solidsurfacemesh.size();
  MatrixXd absNormalDerivs(freesurfacemesh.size(), 1);
  for (size_t i = 0; i < freesurfacemesh.size(); i++) {
    absNormalDerivs(i) = fabs(e.normalDerivAt(i + nb));
  }
  
  double minND = absNormalDerivs.minCoeff();
  double maxND = absNormalDerivs.maxCoeff();
  
  for (size_t i = 0; i < nfs; i++) {
    Triangle t = e.triangleAt(i + nb);
    Vector3d c = t.centroid();
   // double p = e.potentialAt(i);
    double dp = absNormalDerivs(i);
    
    double color = (dp - minND) / (maxND - minND);
    
    //std::cout << dp << std::endl;

    vdb_color(color, 0.1, 1-color);
    vdb_point(c.x(), c.y(), c.z());
  }
  */
  
  
  std::cout << "done!" << std::endl;
  return 1;
  
  
  
  Bubble b0(0);

  b0.set_directory(bubbledir);
 

  b0.compute_all_frequencies(0, 24);
  // b1.compute_all_frequencies(0, 48);
  //b2.compute_all_frequencies(0, 48);
  //b3.compute_all_frequencies(0, 48);

  /*
  for (size_t i = 0; i < 48; i++) {
    std::cout << "Freq: "
    << b0.get_frequency(i) << "  "
    << b1.get_frequency(i) << "  "
    << b2.get_frequency(i) << "  "
    << b3.get_frequency(i) << "  "
    << std::endl;
  }
  */
  
  
  /*
  Monopole m0, m1, m2, m3;

  double inv_SR = 1.0 / 44100;
  Vector3d earL (1,1,1);
  Vector3d earR (1,1,1);

  SoundTrack st(44100, 2);
  SoundFileManager sfm("/Users/phaedon/bubbles.aiff");
  
  
  for (double i = 0.0; i < 2.0; i += inv_SR) {
   // std::cout << "Freq @ time " << i << ":  " << b0.get_frequency(i, 24) << std::endl;
    //std::cout << b0.get_frequency(i, 24) << std::endl;
    
    double s = 0.0;
//    m0.frequency(b0.get_frequency(i, 24));
//    m1.frequency(b1.get_frequency(i, 24));
//    m2.frequency(b2.get_frequency(i, 24));
//    m3.frequency(b3.get_frequency(i, 24));
    
    m0.frequency(b0.get_frequency(i, 24));
    std::cout << "freq:  " << b0.get_frequency(i, 24) << std:: endl;
    s = m0.pressure(i, earL) * exp(-4*i);
    //s = m0.pressure(i, earL) + m1.pressure(i, earL) + m2.pressure(i, earL) + m3.pressure(i, earL);

    st.addSample(s, 0);
    st.addSample(s, 1);
  }
  
  st.normalize();
  sfm.open(WriteOnly);
  sfm.writeAudio(st);
  sfm.close();
  */
  
  
  return 0;
}


