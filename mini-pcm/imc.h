#pragma once

#include "global.h"
#include "pci.h"
#include "pmu.h"
#include "utils.h"

namespace pcm
{

class IMC{
  public:
    int eventCount;
    
    IMC();
    bool program(std::string configStr);
    bool program_imc_pmon_cnt4();
    void enableFixed();
    void getDRAMClocks(std::vector<std::vector<uint64>>& M);
    void getCounter(std::vector<std::vector<uint64>>& M, int counterId);
    void getCounter_imc_pmon_cnt4(std::vector<std::vector<uint64>>& M, int counterId);
    void getFixed(std::vector<std::vector<uint64>>& M);
    void getDRAMReads(std::vector<uint64>& M);
    void getDRAMWrites(std::vector<uint64>& M);
    virtual ~IMC(){}

    void initFreeze();
    void run();
    void print();

  private:
    std::vector<std::pair<uint32, uint32>> socket2UBOX0bus;
    static const std::vector<uint32> UBOX0_DEV_IDS;
    std::vector<std::vector<UncorePMU>> imcPMUs;
    std::array<std::vector<std::shared_ptr<MMIORange>>, SERVER_UNCORE_COUNTER_MAX_SOCKETS> imcbasemmioranges;

    void initSocket2Ubox0Bus();
    std::vector<size_t> getServerMemBars(const uint32 numIMC,
        const uint32 root_segment_ubox0, const uint32 root_bus_ubox0);
    std::vector<std::string> names;

};   // class IMC

inline UncorePMU makeIMCPMU(std::shared_ptr<MMIORange>& handle);

}   // namespace pcm
