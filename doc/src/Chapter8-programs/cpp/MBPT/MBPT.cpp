#include <iomanip>
#include "MBPTfunctions.hpp"
#include "mbpt2_factory.hpp"
#include "mbSolver.hpp"
#include "minnesota_potential.hpp"

int main (int argc, char * argv[]) {
  Input_Parameters Parameters = parse_commandline_arguments(argc, argv);

  mbSolver *solver;
  Model_Space Space;
  Channels Chan;

  // setup structures to time functions
  struct timespec time1, time2, time3;
  double elapsed1 = 0.0, elapsed2 = 0.0, elapsed3 = 0.0;

  // set number of threads for parallel functions
  std::cout << std::setprecision(12);


  clock_gettime(CLOCK_MONOTONIC, &time1);
  Build_Model_Space(Parameters, Space);
  calculate_sp_energies(Parameters, Space);
  clock_gettime(CLOCK_MONOTONIC, &time2);
  elapsed3 = (time2.tv_sec - time1.tv_sec);
  elapsed3 += (time2.tv_nsec - time1.tv_nsec) / 1000000000.0;
  
  clock_gettime(CLOCK_MONOTONIC, &time1); // time1 before channel function
  // setup channels for MBPT_Function == 2, 3, 4
  if(Parameters.MBPT_Function > 1 && Parameters.MBPT_Function < 5){
    Setup_Channels_MBPT(Parameters, Space, Chan);
  }
  clock_gettime(CLOCK_MONOTONIC, &time2); // time2 after channel function, before MBPT function
  elapsed1 = (time2.tv_sec - time1.tv_sec);
  elapsed1 += (time2.tv_nsec - time1.tv_nsec) / 1000000000.0;

  clock_gettime(CLOCK_MONOTONIC, &time2);
  if (Parameters.MBPT_Approx == 2) {
    solver = mbpt2Factory(Parameters);

    clock_gettime(CLOCK_MONOTONIC, &time1); // time1 before channel function
    double energy = solver->getEnergy();
    clock_gettime(CLOCK_MONOTONIC, &time3); // time2 after channel function, before MBPT function

    elapsed2 = (time3.tv_sec - time1.tv_sec);
    elapsed2 += (time3.tv_nsec - time1.tv_nsec) / 1000000000.0;

    std::cout << "Energy: " << energy << std::endl;
    std::cout << "Elapsed: " << elapsed2 << std::endl;
  } else if (Parameters.MBPT_Approx == 3) {
    if(Parameters.MBPT_Function == 0){ MBPT3_0(Parameters, Space); }
    else if(Parameters.MBPT_Function == 1){ MBPT3_1(Parameters, Space); }
    else if(Parameters.MBPT_Function == 2){ MBPT3_2(Parameters, Space, Chan); }
    else if(Parameters.MBPT_Function == 3){ MBPT3_3(Parameters, Space, Chan); }
    else if(Parameters.MBPT_Function == 4){ MBPT3_4(Parameters, Space, Chan); }
  }
  
  clock_gettime(CLOCK_MONOTONIC, &time3); // time3 after MBPT function
  elapsed2 = (time3.tv_sec - time2.tv_sec);
  elapsed2 += (time3.tv_nsec - time2.tv_nsec) / 1000000000.0;
  
  std::cout << "size = " << Space.indtot << ", # of protons = " 
      << Parameters.numberOfProtons << ", # of neutrons = " << Parameters.numberOfNeutrons << std::endl; 
  std::cout << "Approx = " << Parameters.MBPT_Approx << ",  Function = " << Parameters.MBPT_Function << std::endl;
  
  std::cout << "Time to setup modelspace = " << elapsed3 << std::endl;
  std::cout << "Time to setup channels = " << elapsed1 << std::endl;
  std::cout << "Time to run MBPT = " << elapsed2 << std::endl;
  return 0;
}
