/* Copyright 2015-2017, SINTEF Ocean.
   Distributed under the 3-Clause BSD License.
   (See accompanying file LICENCE.txt or copy at
   https://github.com/viproma/demo-fmus/raw/master/LICENCE.txt.) */

#include <cstring>
#include <cppfmu_cs.hpp>
#include "mass_1d-fmu-uuid.h"

enum
{
    VR_force1   = 0,
    VR_force2   = 1,
    VR_pos      = 2,
    VR_vel      = 3,
    VR_der_vel  = 4,
    VR_mass     = 5,
    VR_COUNT    = 6
};


class Mass1DInstance : public cppfmu::SlaveInstance
{
public:
    Mass1DInstance()
    {
        Mass1DInstance::Reset();
    }

private:
    void Reset() override
    {
        for (int i = 0; i < VR_COUNT; ++i) m_values[i] = 0.0;
        m_values[VR_mass] = 1.0;
    }

    void SetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiReal value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            m_values[vr[i]] = value[i];
        }
    }

    void GetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiReal value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] == VR_der_vel) {
                value[i] = Acceleration();
            } else {
                value[i] = m_values[vr[i]];
            }
        }
    }

    bool DoStep(
        fmiReal /*currentCommunicationPoint*/,
        fmiReal dt,
        fmiBoolean /*newStep*/,
        fmiReal& /*endOfStep*/) override
    {
        const auto acc = Acceleration();
        m_values[VR_pos] += dt*m_values[VR_vel] + 0.5*dt*dt*acc;
        m_values[VR_vel] += dt*acc;
        return true;
    }

    fmiReal Acceleration() const
    {
        return (m_values[VR_force1] + m_values[VR_force2]) / m_values[VR_mass];
    }

    fmiReal m_values[VR_COUNT];
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
    return cppfmu::AllocateUnique<Mass1DInstance>(memory);
}
