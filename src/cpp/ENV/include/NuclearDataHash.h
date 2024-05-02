//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * All rights reserved.                                             *
// ********************************************************************
//
/// @library HashLib
//
/// @file NuclearDataHash.h
//
/// @class HashLib::NuclearDataHash
//
/// @brief Header of std-compatible class for the inner hash with fixed length. Without direct object restore.
//
/// @author Kirill Chernov, Chernov_KG@nrcki.ru
//
/// @version 0.9.6
//

#pragma once

#include <array>
#include <iostream>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <iterator>

#include "NuclearData.h"

namespace HashLib
{
    using Word64bitsType = unsigned long long;
    const Word64bitsType Words64bitsZero = 0x0;

    // With fully uniq inner hash with floating length
    class NuclearDataHash
    {
    public:

        NuclearDataHash()
            : nData(new DataLib::NuclearData()),
            hashObjInit(false),
            value({ Words64bitsZero }),
            hashValue(0)
        {}

        // For back compatibility only!
        NuclearDataHash(
            /* for information only */
            /* 20 bits */ //int iConfig = 0,
            /* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 2 bits */ int nTM_temperatures,
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 2 bits */ int n_temperatures,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 1 bit  */ int isotropic,
            /* 2 bit  */ //int fission_type = 0,
            /* 31 bits */ int iName // Only positive values
        ) : nData(new DataLib::NuclearData(
            0,
            iTM,
            iResonance,
            iUrr,
            nTM_temperatures,
            TM_tleft,
            TM_tright,
            n_temperatures,
            tleft,
            tright,
            isotropic,
            0,
            iName)),
            hashObjInit(false),
            value({ Words64bitsZero }),
            hashValue(0)
        {}

        // For back compatibility only!
        NuclearDataHash(
            /* for information only */
            /* 20 bits */ //int iConfig = 0,
            /* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 2 bits */ int nTM_temperatures,
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 2 bits */ int n_temperatures,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 1 bit  */ int isotropic,
            /* 2 bit  */ int fission_type,
            /* 31 bits */ int iName // Only positive values
        ) : nData(new DataLib::NuclearData(
            0,
            iTM,
            iResonance,
            iUrr,
            nTM_temperatures,
            TM_tleft,
            TM_tright,
            n_temperatures,
            tleft,
            tright,
            isotropic,
            fission_type,
            iName)),
            hashObjInit(false),
            value({ Words64bitsZero }),
            hashValue(0)
        {}

        NuclearDataHash(
            /* for information only */
            /* 20 bits */ int iConfig,
            /* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 2 bits */ int nTM_temperatures,
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 2 bits */ int n_temperatures,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 1 bit  */ int isotropic,
            /* 2 bit  */ int fission_type,
            /* 31 bits */ int iName // Only positive values
        ) : nData(new DataLib::NuclearData(
            iConfig,
            iTM,
            iResonance,
            iUrr,
            nTM_temperatures,
            TM_tleft,
            TM_tright,
            n_temperatures,
            tleft,
            tright,
            isotropic,
            fission_type,
            iName)),
            hashObjInit(false),
            value({ Words64bitsZero }),
            hashValue(0)
        {}

        // Adapter for external classes. Class T has to implement all getters!
        template<typename T>
        NuclearDataHash(const T& external)
            :NuclearDataHash(
                external.iConfig(),
                external.iTM(),
                external.iResonance(),
                external.iUrr(),
                external.nTM_temperatures(),
                external.TM_tleft(),
                external.TM_tright(),
                external.n_temperatures(),
                external.tleft(),
                external.tright(),
                external.isotropic(),
                external.fission_type(),
                external.iName()
            ) {}

        NuclearDataHash(const NuclearDataHash& other)
            : nData{ other.nData },
            hashObjInit{ other.hashObjInit },
            value{ other.value },
            hashValue{ other.hashValue },
            realHashArraySize{ other.realHashArraySize }
        {
            if (hashObjInit) nData->fixStatus();
        }

        // For service only
        auto& getHash() const
        {
            if (!hashObjInit && !nData->getStatus())
            {
                makeHash();
                hashObjInit = true;
            }
            return value;
        }

        std::size_t hash() const;

        void place(/* 20 bits */ int iConfig,
            /* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 31 bits */ int iName, // Only positive values
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 2 bits */ int nTM_temperatures,
            /* 2 bits */ int n_temperatures,
            /* 1 bit  */ int isotropic,
            /* 2 bit  */ int fission_type
            )
        {
            nData = new DataLib::NuclearData(
                iConfig,
                iTM,
                iResonance,
                iUrr,
                nTM_temperatures,
                TM_tleft,
                TM_tright,
                n_temperatures,
                tleft,
                tright,
                isotropic,
                fission_type,
                iName);
            hashObjInit = false;
            value = { Words64bitsZero };
            hashValue = 0;
        }

        // For back compatibility only!
        void place(/* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 31 bits */ int iName, // Only positive values
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 2 bits */ int nTM_temperatures,
            /* 2 bits */ int n_temperatures,
            /* 1 bit  */ int isotropic
            /* 2 bit  */ //int fission_type = 0
        )
        {
            place(
                0,
                iTM,
                iResonance,
                iUrr,
                nTM_temperatures,
                TM_tleft,
                TM_tright,
                n_temperatures,
                tleft,
                tright,
                isotropic,
                0,
                iName);
        }

        // For back compatibility only!
        void place(/* 2 bits */ int iTM,
            /* 2 bits */ int iResonance,
            /* 1 bit  */ int iUrr,
            /* 31 bits */ int iName, // Only positive values
            /* 8 bytes */ double TM_tleft,
            /* 8 bytes */ double TM_tright,
            /* 8 bytes */ double tleft,
            /* 8 bytes */ double tright,
            /* 2 bits */ int nTM_temperatures,
            /* 2 bits */ int n_temperatures,
            /* 1 bit  */ int isotropic,
            /* 2 bit  */ int fission_type
        )
        {
            place(
                0,
                iTM,
                iResonance,
                iUrr,
                nTM_temperatures,
                TM_tleft,
                TM_tright,
                n_temperatures,
                tleft,
                tright,
                isotropic,
                fission_type,
                iName);
        }

        void place(DataLib::NuclearData* data)
        {
            nData = data;
            hashObjInit = false;
            value = { Words64bitsZero };
            hashValue = 0;
        }

        int iConfig() const;

        bool operator==(const NuclearDataHash& h) const;

        friend std::ostream& operator<<(std::ostream& os, const NuclearDataHash& h);

    private:
        // Not really const - with mutable
        void makeHash() const;

        // For NuclearDataHash::operator==
        bool& isInitialized() const
        {
            return hashObjInit;
        }
        void setInitialized() const
        {
            hashObjInit = true;
        }

        DataLib::NuclearData* nData;

        mutable bool hashObjInit; // For lazy hash
        static const int MinHashArraySize = 1; // without empty temperatures
        static const int MaxHashArraySize = 5; // it's enough to pack all data
        mutable std::array<Word64bitsType, MaxHashArraySize> value;
        mutable size_t hashValue;
        mutable int realHashArraySize = MinHashArraySize; // doesn't store empty temperatures
    };

} // namespace HashLib

// Hash definitions (for HashLib::NuclearDataHash)
namespace std
{
    template<>
    struct hash<const HashLib::NuclearDataHash>
    {
        std::size_t operator()(const HashLib::NuclearDataHash& h) const
        {
            return h.hash();
        }
    };

    template<>
    struct hash<HashLib::NuclearDataHash>
    {
        std::size_t operator()(const HashLib::NuclearDataHash& h) const
        {
            return h.hash();
        }
    };

} // namespace std

