/* Copyright 2015-2017, SINTEF Ocean.
   Distributed under the 3-Clause BSD License.
   (See accompanying file LICENCE.txt or copy at
   https://github.com/viproma/demo-fmus/raw/master/LICENCE.txt.) */

/*
A sine function.

If x is an input; y is an output; a, b, w, k, d are parameters; and t is time;
then we have:

    y = a + b sin(w t + k x + d)
*/
#include <cmath>
#include <cstring>
#include <cppfmu_cs.hpp>
#include "sine-fmu-uuid.h"


enum
{
    // inputs and parameters
    VR_x    = 0,
    VR_a    = 1,
    VR_b    = 2,
    VR_w    = 3,
    VR_k    = 4,
    VR_d    = 5,
    VR_INPUT_COUNT = 6,

    // outputs
    VR_y    = 6,

    VR_COUNT = 7,
};


class SineInstance : public cppfmu::SlaveInstance
{
public:
    SineInstance()
    {
        SineInstance::Reset();
    }

    void SetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiReal value[])
        override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] >= VR_INPUT_COUNT) {
                throw std::out_of_range{"Value reference out of range"};
            }
            m_input[vr[i]] = value[i];
        }
    }

    void GetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiReal value[])
        const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] == VR_y) {
                value[i] = Calculate();
            } else if (vr[i] < VR_INPUT_COUNT) {
                value[i] = m_input[vr[i]];
            } else {
                throw std::out_of_range{"Value reference out of range"};
            }
        }
    }

private:
    void Initialize(
        fmiReal tStart,
        fmiBoolean /*stopTimeDefined*/,
        fmiReal /*tStop*/)
        override
    {
        m_time = tStart;
    }

    bool DoStep(
        fmiReal currentCommunicationPoint,
        fmiReal communicationStepSize,
        fmiBoolean /*newStep*/,
        fmiReal& /*endOfStep*/)
        override
    {
        m_time = currentCommunicationPoint + communicationStepSize;
        return true;
    }

    void Reset() override
    {
        m_time = 0.0;
        for (auto& v : m_input) v = 0.0;
        m_input[VR_b] = 1.0;
        m_input[VR_w] = 1.0;
    }

    fmiReal Calculate() const CPPFMU_NOEXCEPT
    {
        return m_input[VR_a]
             + m_input[VR_b] * std::sin(
                    m_input[VR_w] * m_time
                  + m_input[VR_k] * m_input[VR_x]
                  + m_input[VR_d]
               );
    }

    fmiReal m_time;
    fmiReal m_input[VR_INPUT_COUNT];
};


cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    fmiString  /*instanceName*/,
    fmiString  fmuGUID,
    fmiString  /*fmuLocation*/,
    fmiString  /*mimeType*/,
    fmiReal    /*timeout*/,
    fmiBoolean /*visible*/,
    fmiBoolean /*interactive*/,
    cppfmu::Memory memory,
    cppfmu::Logger /*logger*/)
{
    if (std::strcmp(fmuGUID, FMU_UUID) != 0) {
        throw std::runtime_error("FMU GUID mismatch");
    }
    return cppfmu::AllocateUnique<SineInstance>(memory);
}
