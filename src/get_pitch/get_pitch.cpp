/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -m FLOAT, --umaxnorm=FLOAT            umbral de l'autocorrelació a 0 [default: 0.298]
    -1 FLOAT, --ur1norm=FLOAT             umbral de l'autocorrelació a llarg termini [default: 0.4]
    -c FLOAT, --ucenterclipping=FLOAT     umbral de center clipping [default: 0.045]
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
  /// \DONE
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float umaxnorm = stof(args["--umaxnorm"].asString());
  float ur1norm = stof(args["--ur1norm"].asString());
  float ucenterclipping = stof(args["--ucenterclipping"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, umaxnorm, ur1norm, ucenterclipping, PitchAnalyzer::HAMMING, 60, 400);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  /// \DONE

  ///Normalitzar la senyal i aplicar center clipping
  float max_senyal = 0;
  for (unsigned int i = 0; i < x.size(); i++)
    if (x[i] > max_senyal)
      max_senyal = x[i];
  float th_cp = ucenterclipping;
  for (unsigned int i = 0; i < x.size(); i++) {
    x[i] = x[i] / max_senyal;
    if (abs(x[i]) < th_cp)
      x[i] = 0;
    if (x[i] > th_cp)
      x[i] -= th_cp;
    if (x[i] < -th_cp)
      x[i] += th_cp;
  }
  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  /// \DONE

  f0[0] = 0;

  //Filtre de mediana
  std::vector<float> f0_aux(f0);
  float max;
  float min;

  for (unsigned int j = 1; j < f0_aux.size() - 1; j++) {
    max = 0;
    min = 500;
    for (int i = -1; i < 2; i++) {
      if (f0_aux[j+i] > max)
        max = f0_aux[j+i];
      if (f0_aux[j+i] < min)
        min = f0_aux[j+i];
    }
    f0[j] = f0_aux[j-1] + f0_aux[j] + f0_aux[j+1] - max - min;
  }

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
