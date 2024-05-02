//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * All rights reserved.                                             *
// ********************************************************************
//
/// @library HashLib
//
/// @file NuclearDataHash.cpp
//
/// @class HashLib::NuclearDataHash
//
/// @brief Std-compatible class for the inner hash with fixed length. Without direct object restore.
//
/// @author Kirill Chernov, Chernov_KG@nrcki.ru
//
/// @version 0.9.6
//

#include "NuclearDataHash.h"

namespace
{
    // For HashLib::NuclearDataHash
    const HashLib::Word64bitsType Rigthmost1bitPattern = 0x1;
    const HashLib::Word64bitsType Rigthmost2bitsPattern = 0x3;
    const HashLib::Word64bitsType Rigthmost20bitsPattern = 0xFFFFF;
    const HashLib::Word64bitsType Rigthmost31bitsPattern = 0x7FFFFFFF;
    const HashLib::Word64bitsType FlagsMultiplierPattern   = 0B0000000000000000000000000000000010000000000000000000000000000000; // for 12 bits total flags
    const HashLib::Word64bitsType IConfigMultiplierPattern = 0B0000000000000000000100000000000000000000000000000000000000000000; // for 20 bits iConfig
//    const int CharsInWords64bitsType = sizeof(HashLib::Word64bitsType);
    const int CharsInWords64bitsType = sizeof(HashLib::Word64bitsType);
    const int CharSizeInBits = sizeof(std::string::value_type) * 8;
    const int BitsInWords64bitsType = CharsInWords64bitsType * CharSizeInBits;
    const int BitShift = BitsInWords64bitsType / 4;

    // std::rotr std::bit C++20+
    // right bit circular shift
    HashLib::Word64bitsType ror(HashLib::Word64bitsType x, unsigned int moves)
    {
        return (x >> moves) | (x << (BitsInWords64bitsType - moves));
    }
}

void HashLib::NuclearDataHash::makeHash() const
{
    int counterValue = 0;
    realHashArraySize = MinHashArraySize;

    /* add flags (12+31 bits currently) */
    Word64bitsType flags = nData->iResonance() & Rigthmost2bitsPattern;
    flags <<= 2;
    flags += nData->iTM() & Rigthmost2bitsPattern;
    flags <<= 1;
    flags += nData->iUrr() & Rigthmost1bitPattern;
    flags <<= 2;
    flags += nData->nTM_temperatures() & Rigthmost2bitsPattern;
    flags <<= 2;
    flags += nData->n_temperatures() & Rigthmost2bitsPattern;
    flags <<= 2;
    flags += nData->isotropic() & Rigthmost1bitPattern;
    flags <<= 1;
    flags += nData->fission_type() & Rigthmost2bitsPattern;
    value[counterValue] = (nData->iConfig() & Rigthmost20bitsPattern) * IConfigMultiplierPattern | flags * FlagsMultiplierPattern | nData->iName() & Rigthmost31bitsPattern;
    ++counterValue;

    if (nData->nTM_temperatures() == 1 || nData->nTM_temperatures() == 2) {
        value[counterValue] = static_cast<Word64bitsType>(nData->TM_tleft());
        ++realHashArraySize;
        ++counterValue;
    }

    if (nData->n_temperatures() == 1 || nData->n_temperatures() == 2) {
        value[counterValue] = ror(static_cast<Word64bitsType>(nData->tleft()), BitShift * 2);
        ++realHashArraySize;
        ++counterValue;
    }

    if (nData->nTM_temperatures() == 2) {
        value[counterValue] = ror(static_cast<Word64bitsType>(nData->TM_tright()), BitShift);
        ++realHashArraySize;
        ++counterValue;
    }

    if (nData->n_temperatures() == 2) {
        value[counterValue] = ror(static_cast<Word64bitsType>(nData->tright()), BitShift * 3);
        ++realHashArraySize;
    }
    hashValue = std::accumulate(value.cbegin(), value.cbegin() + realHashArraySize, Words64bitsZero, std::bit_xor<Word64bitsType>());
    nData->fixStatus();
}

std::size_t HashLib::NuclearDataHash::hash() const
{
    // Make lazy initialization
    if (!hashObjInit && !nData->getStatus())
    {
        makeHash();
        hashObjInit = true;
    }
    return hashValue;
}

int HashLib::NuclearDataHash::iConfig() const
{
    return nData->iConfig();
}

bool HashLib::NuclearDataHash::operator==(const NuclearDataHash& h) const
{
    // Make lazy initialization for left operand
    if (!hashObjInit && !nData->getStatus())
    {
        makeHash();
        hashObjInit = true;
    }
    // Make lazy initialization for right operand
    if (!h.isInitialized() && !h.nData->getStatus())
    {
        h.makeHash();
        h.setInitialized();
    }
    return (realHashArraySize == h.realHashArraySize) && (hashValue == h.hashValue);
}

namespace HashLib
{
    std::ostream& operator<<(std::ostream& os, const NuclearDataHash& h)
    {
        os
            << h.nData;
        os
            << " hashObjInit: " << h.hashObjInit
            << " hashValue: " << h.hashValue;
        //os
        //	<< "_mTM: " << h.nData->iTM()
        //	<< " _mResonance: " << h.nData->iResonance()
        //	<< " _mUnresolved: " << h.nData->iUrr()
        //	<< " _nTM_temperatures: " << h.nData->nTM_temperatures()
        //	<< " _TM_temperature_left: " << h.nData->TM_tleft()
        //	<< " _TM_temperature_right: " << h.nData->TM_tright()
        //	<< " _n_temperatures: " << h.nData->n_temperatures()
        //	<< " _temperature_left: " << h.nData->tleft()
        //	<< " _temperature_right: " << h.nData->tright()
        //	<< " _isotropic: " << h.nData->isotropic()
        //	<< " _names_def_number: " << h.nData->iName()
        //	<< " hashObjInit: " << h.hashObjInit
        //	<< " hashValue: " << h.hashValue;
        if (h.hashObjInit) {
            os << std::hex << " ( ";
            std::copy(h.value.cbegin(), h.value.cbegin() + h.realHashArraySize, std::ostream_iterator<Word64bitsType>(os, " "));
            os << ")";
        }
        return os;
    }
}

