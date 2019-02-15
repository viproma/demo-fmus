// Copyright 2015-2019, SINTEF Ocean.
// Distributed under the 3-Clause BSD License.
// (See accompanying file LICENCE.txt or copy at
// https://github.com/viproma/demo-fmus/raw/master/LICENCE.txt.)

#include <cmath>
#include <cstring>
#include <cppfmu_cs.hpp>
#include "identity-fmu-uuid.h"

class Identity : public cppfmu::SlaveInstance
{
public:
    Identity(const cppfmu::Memory& memory)
        : m_string(cppfmu::Allocator<char>{memory})
    {
        Identity::Reset();
    }

    void Reset() override
    {
        m_real    = 0.0;
        m_integer = 0;
        m_boolean = cppfmu::FMIFalse;
        m_string  = "";
    }

    void SetReal(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        const cppfmu::FMIReal value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_real = value[i];
        }
    }

    void SetInteger(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        const cppfmu::FMIInteger value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_integer = value[i];
        }
    }

    void SetBoolean(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        const cppfmu::FMIBoolean value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_boolean = value[i];
        }
    }

    void SetString(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        const cppfmu::FMIString value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_string = value[i];
        }
    }

    void GetReal(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        cppfmu::FMIReal value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_real;
        }
    }

    void GetInteger(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        cppfmu::FMIInteger value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_integer;
        }
    }

    void GetBoolean(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        cppfmu::FMIBoolean value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_boolean;
        }
    }

    void GetString(
        const cppfmu::FMIValueReference vr[],
        std::size_t nvr,
        cppfmu::FMIString value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_string.c_str();
        }
    }

    bool DoStep(
        cppfmu::FMIReal /*currentCommunicationPoint*/,
        cppfmu::FMIReal /*dt*/,
        cppfmu::FMIBoolean /*newStep*/,
        cppfmu::FMIReal& /*endOfStep*/) override
    {
        return true;
    }

private:
    cppfmu::FMIReal m_real;
    cppfmu::FMIInteger m_integer;
    cppfmu::FMIBoolean m_boolean;
    cppfmu::String m_string;
};


cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    cppfmu::FMIString  /*instanceName*/,
    cppfmu::FMIString  fmuGUID,
    cppfmu::FMIString  /*fmuResourceLocation*/,
    cppfmu::FMIString  /*mimeType*/,
    cppfmu::FMIReal    /*timeout*/,
    cppfmu::FMIBoolean /*visible*/,
    cppfmu::FMIBoolean /*interactive*/,
    cppfmu::Memory memory,
    cppfmu::Logger /*logger*/)
{
    if (std::strcmp(fmuGUID, FMU_UUID) != 0) {
        throw std::runtime_error("FMU GUID mismatch");
    }
    return cppfmu::AllocateUnique<Identity>(memory, memory);
}
