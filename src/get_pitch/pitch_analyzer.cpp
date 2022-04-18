/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

#include "ffft/FFTReal.h"

using namespace std;

/// Name space of UPC
namespace upc {
  //intent cepstrum
  void PitchAnalyzer::cepstrum(const vector<float> &x, vector<float> &c) const {
    float a = log2(x.size());
    unsigned int n = ceil(a);
    long len = 2^n;
    ffft::FFTReal <float> fft_object (len);

    float x2 [len];
    float f [len];
    float f_log [len];
    float cep [len];

    for (unsigned int i = 0; i < x.size(); i++)
      x2[i] = x[i];
    fft_object.do_fft(f, x2);
    for (unsigned int i = 0; i < len; i++)
      f_log[i] = log10(abs(f[i]));
    fft_object.do_ifft(f_log, cep);
    for (unsigned int i = 0; i < c.size(); i++)
      c[i] = cep[i];
  } 
  
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l = 0; l < r.size(); l++) {
  		/// \TODO Compute the autocorrelation r[l]
      /** 
      \DONE Autocorrelation r[l] computed
      - autocorrelation set to 0
      - autocorrelation acumulated for all the signal
      - autocorrelation divided by length
      */
      r[l] = 0;
      
      for (unsigned int n = l; n < x.size(); n++)
        r[l] += x[n]*x[n-l];

      r[l] /= x.size();
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \DONE Finestra de Hamming implementada
      for (unsigned int i = 0; i < frameLen; i++)
        window[i] = 0.54 - 0.46 * std::cos(2.0 * 3.141592 / (frameLen - 1) * i);
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.

    /// \DONE Voice/unvoiced detector imlemented

    if (rmaxnorm > umaxnorm && r1norm > ur1norm) return false;
    return true;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);
    vector<float> c(npitch_max);

    //Compute correlation
    autocorrelation(x, r);
    //cepstrum(x, c);

    //Normalitzar autocorrelació i aplicar center clipping
    float max_r = *max_element(r.begin(), r.end());
    float th_cp_r = 0.1;
    for (unsigned int i = 0; i < r.size(); i++) {
      r[i] = r[i] / max_r;
      if (abs(r[i]) < th_cp_r)
        r[i] = 0;
      if (r[i] > th_cp_r)
        r[i] -= th_cp_r;
      if (r[i] < -th_cp_r)
        r[i] += th_cp_r;
    } 

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO 
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.

    /// \DONE Pitch trobat

    for (iR = iRMax = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++)
      if (*iR > *iRMax) iRMax = iR;

    unsigned int lag = iRMax - r.begin();

    float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0   //posar a 1 per debugejar
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]))
      return 0;
      //trama sorda
    else
      return (float) samplingFreq/(float) lag;
      //trama sonora
  }
}
