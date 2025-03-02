#include "pmu.h"

namespace pcm
{

UncorePMU::UncorePMU(const HWRegisterPtr& unitControl_,
    const HWRegisterPtr& counterControl0,
    const HWRegisterPtr& counterControl1,
    const HWRegisterPtr& counterControl2,
    const HWRegisterPtr& counterControl3,
    const HWRegisterPtr& counterControl4,
    const HWRegisterPtr& counterValue0,
    const HWRegisterPtr& counterValue1,
    const HWRegisterPtr& counterValue2,
    const HWRegisterPtr& counterValue3,
    const HWRegisterPtr& counterValue4,
    const HWRegisterPtr& fixedCounterControl_,
    const HWRegisterPtr& fixedCounterValue_,
    const HWRegisterPtr& filter0,
    const HWRegisterPtr& filter1
) :
    cpu_model_(0),
    eventCount(0),
    unitControl(unitControl_),
    counterControl{ counterControl0, counterControl1, counterControl2, counterControl3,counterControl4 },
    counterValue{ counterValue0, counterValue1, counterValue2, counterValue3, counterValue4 },
    fixedCounterControl(fixedCounterControl_),
    fixedCounterValue(fixedCounterValue_),
    filter{ filter0 , filter1 }
{}

void UncorePMU::cleanup()
{
    for (int i = 0; i < SERVER_UNCORE_COUNTER_MAX_COUNTERS; ++i)
    {
        if (counterControl[i].get()) *counterControl[i] = 0;
    }
    if (unitControl.get()) *unitControl = 0;
    if (fixedCounterControl.get()) *fixedCounterControl = 0;
}

void UncorePMU::freeze()
{
    *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ ;
}

void UncorePMU::unfreeze()
{
    *unitControl =  0;
}

void UncorePMU::print(uint32 ctrl, uint32 counter){
	std::cout << ctrl << ": counter Control handle address is" << std::hex << counterControl[ctrl] << std::dec << std::endl;
	//std::cout << "Control Setting for ctr:" << ctrl << std::hex << *counterControl[ctrl] << std::dec << std::endl;
	//std::cout << "Counter Value for CTR:" << counter << std::hex << *counterValue[counter] << std::dec << std::endl;
}


bool UncorePMU::initFreeze()
{
        *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ; // freeze
        *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_CONTROL; // freeze and reset control registers
        return true;
    // freeze enable bit 0 is set to 1 to freeze unit counters and bit 8 to 1 to freeze counter controls 
    // freeze

}

void UncorePMU::resetUnfreeze()
{
        *unitControl = SPR_UNC_PMON_UNIT_CTL_FRZ + SPR_UNC_PMON_UNIT_CTL_RST_COUNTERS; // counters freeze and  additionaly counter are reset with bit 9 set to 1
        *unitControl = 0; // unfreeze and take everything our of freeze and reset.
        return;
}

FreeRunBWCounters::FreeRunBWCounters(const std::shared_ptr<SafeMsrHandle> & handle_, size_t offset_)
{
    BWIn[0] = std::make_shared<MSRRegister>(handle_, offset_);
    BWIn[1] = std::make_shared<MSRRegister>(handle_, offset_+1);
    BWIn[2] = std::make_shared<MSRRegister>(handle_, offset_+2);
    BWIn[3] = std::make_shared<MSRRegister>(handle_, offset_+3);
    BWIn[4] = std::make_shared<MSRRegister>(handle_, offset_+4);
    BWIn[5] = std::make_shared<MSRRegister>(handle_, offset_+5);
    BWIn[6] = std::make_shared<MSRRegister>(handle_, offset_+6);
    BWIn[7] = std::make_shared<MSRRegister>(handle_, offset_+7);
    BWOut[0] = std::make_shared<MSRRegister>(handle_, offset_+8);
    BWOut[1] = std::make_shared<MSRRegister>(handle_, offset_+9);
    BWOut[2] = std::make_shared<MSRRegister>(handle_, offset_+10);
    BWOut[3] = std::make_shared<MSRRegister>(handle_, offset_+11);
    BWOut[4] = std::make_shared<MSRRegister>(handle_, offset_+12);
    BWOut[5] = std::make_shared<MSRRegister>(handle_, offset_+13);
    BWOut[6] = std::make_shared<MSRRegister>(handle_, offset_+14);
    BWOut[7] = std::make_shared<MSRRegister>(handle_, offset_+15);
}

}   // namespace pcm
