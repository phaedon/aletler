//
//  Bubble.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include "PhysicalConstants.h"
#include "Bubble.h"
#include <boost/numeric/odeint.hpp>
#include <io/FileNameGen.h>
#include <sstream>
#include <string>
#include <io/FileStringParsers.h>

using namespace boost::numeric::odeint;
typedef boost::array< double , 3 > state_type;

using namespace PhysicalConstants;


static std::vector<double> *g_samples = NULL;
static SoundFrequency *g_soundfreq = NULL;
static double g_r0 = 0;


static double forcingfn(double t) {
  
  // TURN IT OFF
  return 0;
  
  double a = 1.0/10000.0;
  
  // shifted t
  double st = t - a;
  
  return (1.0 / (a * sqrt(M_PI))) * exp(-st*st / (a*a));
}


static double omega(double t) {
  
  return g_soundfreq->frequencyAt(t, FREQ_OMEGA);
}

static void rhs( const state_type &x , state_type &dxdt ,  double t )
{
  
  double omg = omega(t);
  
  double drad = omg * g_r0 / Sound::C_WATER;
  
  double dvis = 4 * Fluids::MU_WATER / (Fluids::RHO_WATER * omg * g_r0*g_r0);
  
  
  
  double psi = (16 * Fluids::THERMAL_DAMPING * Dynamics::G) / (9 * omg * pow(Fluids::GAMMA_AIR - 1, 2));
  
  double dth = 2 * ( sqrt(psi-3) - (3*Fluids::GAMMA_AIR - 1)/(3* (Fluids::GAMMA_AIR - 1))) / (psi - 4);
  
  double delta = drad + dvis + dth;
  
  double damping = omg * delta / sqrt(delta*delta + 4);
  
  dxdt[0] = x[1];
  dxdt[1] = forcingfn(t) - omg * omg * x[0] - damping*x[1];
}

static void store_samples( const state_type &x , const double t ) {
  g_samples->push_back(x[0]);
}


double Bubble::frequency_omega(double capacitance) const {
  
  if (capacitance == 0) return 0;
  
  double four_pi_gamma = 4 * M_PI * PhysicalConstants::Fluids::GAMMA_AIR;
  
  double v = _bubble->volume();
  
  // TODO: doens't take hydrostatic pressure into account
  double p0 = PhysicalConstants::Fluids::P_ATM;
  
  return sqrt( four_pi_gamma * p0 * capacitance / (PhysicalConstants::Fluids::RHO_WATER * v) );
}


void Bubble::integrateVibrationODE() {
  
  // nothing to integrate...
  if (_soundfreq.isEmpty())
    return;
  
  double timeStep = 1.0 / Sound::SAMPLING_RATE;
  
  // temporarily store the address in a global var so the callback can access it
  g_samples = &_samples;
  g_soundfreq = &_soundfreq;
  g_r0 = _r0;
  
  runge_kutta4< state_type > stepper;
  state_type initcondits = { 0.0, -1.0 };
  integrate_const(stepper,
                  rhs,
                  initcondits,
                  _soundfreq.startTime(), _soundfreq.stopTime(),
                  timeStep,
                  store_samples );
  
  // no pointers left behind!
  g_samples = NULL;
  g_soundfreq = NULL;
  
  sample0 = floor(_soundfreq.startTime() * Sound::SAMPLING_RATE);
  samplef = sample0 + _samples.size() - 1;
}


void Bubble::setBubbleMesh(TriangleMesh *b) {
  _bubble = b;
  double bubbleVolume = _bubble->volume();
  double mass_air = bubbleVolume * Fluids::RHO_AIR;
  double mass_water = bubbleVolume * Fluids::RHO_WATER;
  _accel = Vector3d(0, Dynamics::G * (mass_water - mass_air) / (mass_air + mass_water), 0);
  
  if (_r0 < 0) {
    _r0 = pow( 3 * bubbleVolume / (4 * M_PI), 1.0/3.0);
  }
  
}

double Bubble::getSample(size_t sampleIndex) {
  if (_soundfreq.isEmpty()) return 0;
  
  if (sampleIndex < sample0 || sampleIndex > samplef)
    return 0;
  
  size_t arrayIndex = sampleIndex - sample0;
  
  // not sure why this one is also necessary, as I thought I'd caught this case, but
  // I guess there's some edge case:
  if (arrayIndex >= _samples.size()) return 0;
  
  
  double t = (sampleIndex * 1.0) / (double)( Sound::SAMPLING_RATE );
  
  return _samples[arrayIndex] * getPressureScale(t);
}


void Bubble::saveBubbleFrequencyFile() const {
  std::string filename = bubbleFreqFilename("/Users/phaedon/github/aletler/meshes/geomsim3/", "bubblefreqs", _bubble_index);
  
  std::ofstream ofile;
  ofile.open(filename.c_str());
  
  ofile << std::setprecision(10) << std::fixed;
  ofile << _r0 << std::endl;
  _soundfreq.saveFrequencyFile(ofile);
  
  ofile.close();
}



bool Bubble::loadBubbleFrequencyFile(const std::string &filename) {
  std::ifstream ifile(filename.c_str());
  std::string line;
  
  const std::string &whitespace = " \t";
  
  if (ifile.is_open()) {
    
    // grab the first line, which contains the "radius":
    std::getline(ifile, line);
    _r0 = StringToNumber<double>(line);
 
    while (std::getline (ifile,line)) {
      
      size_t strBegin = line.find_first_not_of(whitespace);
      if (strBegin == std::string::npos  // empty line
          || line[strBegin] == '#')  // comment
        continue;
      
      double timestamp, freq;
      StringToDoublePair(line, timestamp, freq);
      
      _soundfreq.addFrequency(timestamp, freq, FREQ_OMEGA);
      
    }
    
    ifile.close();
    return true;
    
  } else {
    //std::cout << "Unable to open file" << std::endl;
    return false;
  }
  

}


// returns the frequency, in Hz
double Bubble::setFrequency(double timeStamp, double capacitance) {
  double f_omega = frequency_omega(capacitance);
  _soundfreq.addFrequency(timeStamp, f_omega, FREQ_OMEGA);
  
  return f_omega * 0.5 * M_1_PI;
}


void Bubble::timestep(double dt) {
  _vel += dt * _accel;
  _bubble->translate(dt * _vel);
}

void Bubble::loadExternalSolverFiles(const std::string &weightFilename,
                             const std::string &velFilename,
                                     double timestamp) {
  
  
  
  std::ifstream ifileWt(weightFilename.c_str());
  std::string lineWt;
  
  
  if (! ifileWt.is_open()) {
    // file doesn't exist. Fail quietly
    return;
  }
  
  // this will store the dot product!
  std::complex<double> pressure(0,0);
  std::ifstream ifileVel(velFilename.c_str());
  std::string lineVel;
  
  
  if (! ifileVel.is_open()) {
    // file doesn't exist. Fail quietly
    return;
  }
  

  while (std::getline (ifileWt, lineWt)) {
    std::getline (ifileVel, lineVel);
    
    std::complex<double> thisWt, thisVel;
    std::string thisVelStr;
    
    // parse both lines
    // multiply them
    
    thisWt = StringToComplex(lineWt);
    thisVel = std::complex<double>(StringToNumber<double>(lineVel), 0.0);
    
    // dot product, step by step!
    pressure += thisWt * thisVel;
  }
  
  ifileWt.close();
  ifileVel.close();
  
  std::cout << "pressure: " << std::abs(pressure) << std::endl;
  _cpressureScales.push_back(pressure);
  _pressureTimes.push_back(timestamp);
}


double Bubble::getPressureScale(double time) {
  return std::abs(interpLinearComplexVectors(time, _pressureTimes, _cpressureScales));
}
