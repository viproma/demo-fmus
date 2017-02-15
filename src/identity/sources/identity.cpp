// Copyright 2015-2017, SINTEF Ocean.
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
        m_boolean = fmiFalse;
        m_string  = " ";
    }

    void SetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiReal value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_real = value[i];
        }
    }

    void SetInteger(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiInteger value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_integer = value[i];
        }
    }

    void SetBoolean(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiBoolean value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_boolean = value[i];
        }
    }

    void SetString(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiString value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            m_string = value[i];
        }
    }

    void GetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiReal value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_real;
        }
    }

    void GetInteger(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiInteger value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_integer;
        }
    }

    void GetBoolean(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiBoolean value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_boolean;
        }
    }

    void GetString(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiString value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] != 0) throw std::logic_error("Invalid value reference");
            value[i] = m_string.c_str();
        }
    }

    bool DoStep(
        fmiReal /*currentCommunicationPoint*/,
        fmiReal /*dt*/,
        fmiBoolean /*newStep*/,
        fmiReal& /*endOfStep*/) override
    {
        return true;
    }

private:
    fmiReal m_real;
    fmiInteger m_integer;
    fmiBoolean m_boolean;
    cppfmu::String m_string;
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
    return cppfmu::AllocateUnique<Identity>(memory, memory);
}
