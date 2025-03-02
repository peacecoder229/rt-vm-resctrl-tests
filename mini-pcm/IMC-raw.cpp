/*
   Copyright (c) 2009-2020, Intel Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!     \file pcm-raw.cpp
  \brief Example of using CPU counters: implements a performance counter monitoring utility with raw events interface
  */

#include "global.h"

#include "imc.h"
#include "cha.h"
#include "iio.h"
#include "slidingwindow.h"
#include "functions.h"
#include <csignal>
#include <iostream>
#include <thread>
#include <chrono>

#include <vector>
#define PCM_DELAY_DEFAULT 1.0 // in seconds
#define PCM_DELAY_MIN 0.015 // 15 milliseconds is practical on most modern CPUs
#define MAX_CORES 4096
using namespace std;
// using namespace pcm;

bool show_partial_core_output = false;
bitset<MAX_CORES> ycores;
bool flushLine = false;
pcm::IMC* global_imc_ptr = nullptr;
pcm::CHA* global_cha_ptr = nullptr;
pcm::IIO* global_iio_ptr = nullptr;
bool use_imc_pmon_cnt4 = false;

double calibratedMicroSeconds(unsigned long delay_micros){
	//std::cout << "Sleeping for " << delay_micros << " micro seconds \n";
	auto start_time = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::microseconds(delay_micros));
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> elapsed_time = end_time - start_time;
	return elapsed_time.count();
	
}


void signal_handler(int signum)
{
    std::cout << "Caught signal " << signum << ", gracefully shutting down." << std::endl;

    // Call the initFreeze() function
if (global_imc_ptr) {
            global_imc_ptr->initFreeze();
        }

if (global_cha_ptr) {
            global_cha_ptr->initFreeze();
        }
    // Terminate the program
if (global_iio_ptr) {
            global_iio_ptr->initFreeze();
        }
    exit(signum);
}



void print_usage(const string progname)
{
    cerr << "\n Usage: \n " << progname
         << " --help | [delay] [options] [-- external_program [external_program_options]]\n";
    cerr << "   <delay>                               => time interval to sample performance counters.\n";
    cerr << "                                            If not specified, or 0, with external program given\n";
    cerr << "                                            will read counters only after external program finishes\n";
    cerr << " Supported <options> are: \n";
    cerr << "  -h    | --help      | /h               => print this help and exit\n";
    cerr << "  -csv[=file.csv]     | /csv[=file.csv]  => output compact CSV format to screen or\n"
         << "                                            to a file, in case filename is provided\n";
    cerr << "  [-e event1] [-e event2] [-e event3] .. => list of custom events to monitor\n";
    cerr << "  event description example: -e core/config=0x30203,name=LD_BLOCKS.STORE_FORWARD/ -e core/fixed,config=0x333/ \n";
    cerr << "                             -e cha/config=0,name=UNC_CHA_CLOCKTICKS/ -e imc/fixed,name=DRAM_CLOCKS/\n";
    cerr << "  -yc   | --yescores  | /yc              => enable specific cores to output\n";
    cerr << "  -f    | /f                             => enforce flushing each line for interactive output\n";
    cerr << "  -i[=number] | /i[=number]              => allow to determine number of iterations\n";
    //print_help_force_rtm_abort_mode(41);
    cerr << " Examples:\n";
    cerr << "  " << progname << " 1                   => print counters every second without core and socket output\n";
    cerr << "  " << progname << " 0.5 -csv=test.log   => twice a second save counter values to test.log in CSV format\n";
    cerr << "  " << progname << " /csv 5 2>/dev/null  => one sampe every 5 seconds, and discard all diagnostic output\n";
    cerr << "\n";
}

bool addEvent(string eventStr, pcm::IMC& imc, pcm::CHA& cha, pcm::IIO& iio)
{

    enum class pmuType {
        IMC,
        CHA,
        IIO
    };
    const std::map<std::string, pmuType> pmuNameMap{{"imc", pmuType::IMC},
                                                    {"cha", pmuType::CHA},
                                                    {"iio", pmuType::IIO}};


    std::cout <<"====addEvent eventStr:"<<eventStr<< std::endl;

    const auto typeConfig = pcm::split(eventStr, '/');
    if (typeConfig.size() < 2)
    {
        cerr << "ERROR: wrong syntax in event description \"" << eventStr << "\"\n";
        return false;
    }
    std::string pmuName = typeConfig[0];

    if (pmuName.empty())
    {
        pmuName = "core";
        // TODO:: what do we do here?
    }
    const auto configStr = typeConfig[1];
    if (configStr.empty())
    {
        cerr << "ERROR: empty config description in event description \"" << eventStr << "\"\n";
        return false;
    }

    switch(pmuNameMap.at(pmuName)){
        case pmuType::IMC:
            imc.program(configStr);
            break;
        case pmuType::CHA:
            cha.program(configStr);
            break;
        case pmuType::IIO:
            iio.program(configStr);
            break;
        default:
            return false;
    }

    return true;
}

bool addEvent_imc_pmon_cnt4(string eventStr, pcm::IMC& imc, pcm::CHA& cha, pcm::IIO& iio)
{

    imc.program_imc_pmon_cnt4();

    return true;
}

int main(int argc, char* argv[])
{
    //set_signal_handlers();

    pcm::IMC imc;
    pcm::CHA cha;
    pcm::IIO iio;
    global_imc_ptr = &imc;
    global_cha_ptr = &cha;
    global_iio_ptr = &iio;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    cerr << "\n";
    cerr << " Processor Counter Monitor: Raw Event Monitoring Utility \n";
    cerr << "\n";

    double delay = -1.0;
    string program = string(argv[0]);
    int iteration = 1;
    double dlymicro = -1;
    double actualmicro;

    if (argc > 1) do
    {
        argv++;
        argc--;
        if (strncmp(*argv, "--help", 6) == 0 ||
            strncmp(*argv, "-h", 2) == 0 ||
            strncmp(*argv, "/h", 2) == 0)
        {
            print_usage(program);
            exit(EXIT_FAILURE);
        }

        else if (strncmp(*argv, "-i", 2) == 0)
        {
            argv++;
            argc--;
            iteration = atoi(*argv);
            cout << "Iteration = " << iteration << "\n";
        }

        else if (strncmp(*argv, "-e", 2) == 0)
        {
            argv++;
            argc--;
            
            if(imc.eventCount <= 3 )
	    {
            if (addEvent(*argv, imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }
	    }else
	    {
		use_imc_pmon_cnt4 = true;
            	if (addEvent_imc_pmon_cnt4("imc/config=0x0000000000000080,name=UNC_M_RPQ_OCCUPANCY_PCH0", imc, cha, iio) == false)
            	{
                exit(EXIT_FAILURE);
            	}

	    }

            continue;
        }
        else if (strncmp(*argv, "-d", 2) == 0)
        {
            argv++;
            argc--;
            delay = atof(*argv);
	    dlymicro = delay * 1000000;
            cout << "Delay in seconds " << delay  << "\n";

            // any other options positional that is a floating point number is treated as <delay>,
            // while the other options are ignored with a warning issues to stderr
        }
	else if (strncmp(*argv, "-s", 2) == 0)
	{
            use_imc_pmon_cnt4 = true;
	    //specified parameters
	    /*if (addEvent("imc/config=0x000000000000f005,name=UNC_M_CAS_COUNT.WR", imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }
	    if (addEvent("imc/config=0x000000000000cf05,name=UNC_M_CAS_COUNT.RD", imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }

	    if (addEvent("imc/config=0x0000000000000082,name=UNC_M_WPQ_OCCUPANCY_PCH0", imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }
            if (addEvent("imc/config=0x0000000000000080,name=UNC_M_RPQ_OCCUPANCY_PCH0", imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }*/
	    if (addEvent_imc_pmon_cnt4("imc/config=0x0000000000000080,name=UNC_M_RPQ_OCCUPANCY_PCH0", imc, cha, iio) == false)
            {
                exit(EXIT_FAILURE);
            }
            continue;
	}
    } while (argc > 1); // end of command line parsing loop

    imc.run();
    cha.run();
    iio.run();

    if (delay <= 0.0) delay = PCM_DELAY_DEFAULT;

    cerr << "Update every " << delay << " seconds\n";

    std::cout.precision(4);
    std::cout << std::fixed;

    while (1){
        //::sleep(delay);
	actualmicro =  calibratedMicroSeconds(dlymicro);
	double tot_sample_in_sec = 1e6 / actualmicro;
	//std::cout << "Actual sleep duration: " << actualmicro << " microseconds and total samples in seconds is " << tot_sample_in_sec << "\n"  ;

        // imc.print();
        // cha.print();
        // iio.print();
        // iio.printFR();
        //chaPost(cha, tot_sample_in_sec, "iobw");
        chaPost(cha, tot_sample_in_sec, "latency");

        if (use_imc_pmon_cnt4)
	{
         if (imc.eventCount > 1 )
            {
            imcPost(imc, tot_sample_in_sec);
	    }
        
        imcPost_pmon_cnt4(imc, tot_sample_in_sec);
        }else
        {
        imcPost(imc, tot_sample_in_sec);
        }

        iioPost(iio, tot_sample_in_sec);
    }
}
