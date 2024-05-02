//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * All rights reserved.                                             *
// ********************************************************************
//
/// @library HashLib
//
/// @file NuclearDataHashStore.hpp
//
/// @class HashLib::NuclearDataHashStore
//
/// @brief Std-compatible template class for the inner hash operationing.
//
/// @author Kirill Chernov, Chernov_KG@nrcki.ru
//
/// @version 0.8.0
//

#pragma once

#include "NuclearDataHash.h"

namespace HashLib
{

    template<typename T>
    class NuclearDataHashStore
    {
    public:

        NuclearDataHashStore() {}

        void add(const NuclearDataHash& key, const T& value)
        {
            if (umap.count(key) > 0)
            {
                umap[key] = value;
            }
            else
            {
                umap.insert({ key, value });
            }
        }

        void clear()
        {
            umap.clear();
        }

        bool has(const NuclearDataHash& key)
        {
            return umap.count(key) > 0;
        }

        const auto& value(NuclearDataHash& key)
        {
            return umap[key];
        }

        auto size()
        {
            return umap.size();
        }

        friend std::ostream& operator<<(std::ostream& os, const NuclearDataHashStore& h)
        {
            os << "( ";
            for (const auto& v : h.umap)
                os << "[ " << (v.first) << " , " << std::dec << v.second << " ] ";
            os << ")";
            return os;
        }

    private:
        std::unordered_map<NuclearDataHash, T> umap;
    };

} // namespace HashLib

