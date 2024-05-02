#pragma once

#ifndef ENV_HDF5_HPP
#define ENV_HDF5_HPP

#include "ENV_types.hpp"
#include "hdf5.h"
//#include "H5Cpp.h"

using namespace ENV;

namespace ENV_H5
{

#ifdef _DEBUG
    // for DEBUG
    static INT64
        counter_h5fopen_f = ENV_0_I64,
        counter_h5fcreate_f = ENV_0_I64,
        counter_h5fclose_f = ENV_0_I64;
#endif
    //
    static LOGICAL
        HDF5_IS_INITIALIZED = false;
    // compression
    const INT32
        COMPRESSION_LEVEL = INT32_C(6),
        COMPRESSION_START_LENGTH = INT32_C(1100000);
    const hsize_t
        COMPRESSION_CHUNK_SIZE[1] = { 1048576ul };
    // Array types for attributes
    const STRING
        ARRAY_TYPE_INT08    = "INT:08",
        ARRAY_TYPE_INT32    = "INT:32",
        ARRAY_TYPE_INT64    = "INT:64",
        ARRAY_TYPE_REAL32   = "REAL:32",
        ARRAY_TYPE_REAL64   = "REAL:64",
        ARRAY_TYPE_REAL128  = "REAL:64",
        ARRAY_TYPE_CHARNAME = "STR:ENV_NAME_LENGTH",
        ARRAY_TYPE_STRING   = "STR:ENV_STRING_LENGTH";
    //
    const V_STRING ARRAY_DATA_NAMES =
    {
    "Length",
    "Type",
    "Data",
    "Data"
    };
#ifdef H5Cpp_H
    // API consts
    const H5::IntType H5_LOGICAL(H5::PredType::STD_I32LE);
    const H5::IntType H5_INT08(H5::PredType::STD_I8LE);
    const H5::IntType H5_INT32(H5::PredType::STD_I32LE);
    const H5::IntType H5_INT64(H5::PredType::STD_I64LE);
    const H5::FloatType H5_REAL32(H5::PredType::NATIVE_FLOAT);
    const H5::FloatType H5_REAL64(H5::PredType::NATIVE_DOUBLE);
    const H5::FloatType H5_REAL128(H5::PredType::NATIVE_LDOUBLE);
    // Create string type for the String attribute.
    const H5::StrType H5_STRING(H5::PredType::C_S1, H5T_VARIABLE);
    //H5_STRING.setCset(H5T_CSET_UTF8); // just for fun, you don't need this
    const H5::VarLenType H5_V_STRING = H5::VarLenType(&H5_STRING);
    const hid_t
        HDF5_INT08 = H5_INT08.getId(),
        HDF5_INT32 = H5_INT32.getId(),
        HDF5_INT64 = H5_INT64.getId(),
        HDF5_REAL32 = H5_REAL32.getId(),
        HDF5_REAL64 = H5_REAL64.getId(),
        HDF5_REAL128 = H5_REAL128.getId(),
        HDF5_STRING = H5_STRING.getId(),
        HDF5_V_STRING = H5_V_STRING.getId(),
        HDF5_LOGICAL = HDF5_INT32;
    const VECTOR<hid_t> HDF5_TYPES =
    {
        HDF5_REAL64, HDF5_REAL32, HDF5_INT64, HDF5_INT32,
        HDF5_STRING, HDF5_V_STRING, HDF5_INT08
    };
    const VECTOR<H5::DataType> H5_TYPES =
    {
        H5_REAL64, H5_REAL32, H5_INT64, H5_INT32,
        H5_STRING, H5_V_STRING, H5_INT08
    };
#else

#define HDF5_STRING_LENGTH 300
#define HDF5_NAME_LENGTH 24
    //const H5::StrType H5_STRING_LONG(H5::PredType::C_S1, H5_STRING_LENGTH);
    //const H5::StrType H5_STRING_NAME(H5::PredType::C_S1, H5_NAME_LENGTH);
#define HDF5_TYPE_INT08 H5T_NATIVE_INT8
#define HDF5_TYPE_INT32 H5T_NATIVE_INT32
#define HDF5_TYPE_INT64 H5T_NATIVE_INT64
#define HDF5_TYPE_REAL32 H5T_NATIVE_FLOAT
#define HDF5_TYPE_REAL64 H5T_NATIVE_DOUBLE
#define HDF5_TYPE_REAL128 H5T_NATIVE_LDOUBLE
#define HDF5_TYPE_LOGICAL HDF5_TYPE_INT32
    // Create string type for attribute with any width.
#define HDF5_STRING_TYPE_WIDTH(str_type_id, width) \
    if (str_type_id == 0) \
    { \
        str_type_id = H5Tcopy(H5T_FORTRAN_S1); \
        H5Tset_size(str_type_id, width); \
    }
    // Create string type for the Name attribute with Checking.
#define is_HDF5_TYPE_NAME HDF5_STRING_TYPE_WIDTH(ENV_H5::HDF5_TYPE_NAME, HDF5_NAME_LENGTH)
    // Create string type for the String attribute with Checking.
#define is_HDF5_TYPE_STRING HDF5_STRING_TYPE_WIDTH(ENV_H5::HDF5_TYPE_STRING, HDF5_STRING_LENGTH)
    static hid_t
        HDF5_TYPE_NAME = 0,
        HDF5_TYPE_STRING = 0,
        HDF5_TYPE_V_STRING = 0;

#endif

    static hsize_t
        HDF5_DIM1[1],
        HDF5_DIM2[2],
        HDF5_DIM3[3];

#ifdef H5Cpp_H

    // Open or create HDF - file by H5::H5File
    // status - open or create
    // available actions - for open(read, read - write), for create(truncate)
    H5::H5File* HDF_file_open(const STRING& file_name, const STRING& status = "open", const STRING& action = STRING_EMPTY);
    // HDF - file close by H5::H5File
    void HDF_file_close(H5::H5File* hdf_file);

    // Reading HDF5 attributes based on given vectors of types.
    // parent_id - Parent Group identifier as H5::Group
    template<typename DataType>
    LOGICAL HDF_HeaderReadSimple(const H5::Group& parent_id, const STRING sHeader, V_STRING sHeadName, VECTOR<DataType>& cHead)
    {
        // Open the Header dataset and attributes.
        H5::Group group_id;   // Dataset identifier
        if (!sHeader.empty())
        {
            try
            {
                group_id = H5::Group(parent_id.openGroup(sHeader));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        else group_id = parent_id;
        // check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead))
        {
            return -2;
        }
        // Select type of Header
        H5::DataType stype_id;  // Dataset element type
        if (typeid(DataType) == typeid(REAL64))
            stype_id = H5_REAL64;
        else if (typeid(DataType) == typeid(INT32))
            stype_id = H5_INT32;
        else if (typeid(DataType) == typeid(INT08))
            stype_id = H5_INT08;
        else if (typeid(DataType) == typeid(INT64))
            stype_id = H5_INT64;
        else if (typeid(DataType) == typeid(REAL32))
            stype_id = H5_REAL32;
        else if (typeid(DataType) == typeid(REAL128))
            stype_id = H5_REAL128;
        else if (typeid(DataType) == typeid(LOGICAL))
            stype_id = H5_LOGICAL;
        else
        {
            return -3;
        }
        LOGICAL iError = 0;
        // cycle by attribute
        for (auto id = 0; id < iSizeOfHead; ++id)
        {
            if (!group_id.attrExists(sHeadName[id]))
            {
                iError = -4;
                break;
            }
            try
            {
                // Open the Header attribute
                H5::Attribute attr_id = group_id.openAttribute(sHeadName[id].c_str());   // Attribute identifier
                DataType a_ptr;     // Attribute Data Pointer
                // Read the Header attribute
                attr_id.read(stype_id, &a_ptr);
                cHead[id] = a_ptr;
                // Save attribute to Header
                // Close and release resources
                attr_id.close();
            }
            catch (const H5::AttributeIException&)
            {
                iError = -5;
                break;
            }
        }
        try
        {
            if (!sHeader.empty()) group_id.close();
        }
        catch (const H5::GroupIException&)
        {
            iError = -6;
        }
        return iError;
    }

    // Reading HDF5 attributes based on given vectors of names.
    // parent_id - Parent Group identifier as H5::Group
    LOGICAL HDF_HeaderReadNames(const H5::Group& parent_id, const STRING sHeader, V_STRING sHeadName, V_STRING& cHead);
    // Writing HDF5 attributes based on given vectors of types.
    // parent_id - Parent Group identifier as H5::Group
    template<typename DataType>
    LOGICAL HDF_HeaderWriteSimple(H5::Group* parent_id, const STRING sHeader, V_STRING sHeadName, VECTOR<DataType> cHead)
    {
        // Open group.
        H5::Group group_id; // Group identifier
        if (sHeader.empty()) group_id = *parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id->createGroup(sHeader));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        //  check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead)) return -2;
        // Select type of Header
        H5::DataType stype_id;  // Dataset element type
        if (typeid(DataType) == typeid(REAL64))
            stype_id = H5_REAL64;
        else if (typeid(DataType) == typeid(INT32))
            stype_id = H5_INT32;
        else if (typeid(DataType) == typeid(INT08))
            stype_id = H5_INT08;
        else if (typeid(DataType) == typeid(INT64))
            stype_id = H5_INT64;
        else if (typeid(DataType) == typeid(REAL32))
            stype_id = H5_REAL32;
        else if (typeid(DataType) == typeid(REAL128))
            stype_id = H5_REAL128;
        else if (typeid(DataType) == typeid(LOGICAL))
            stype_id = H5_LOGICAL;
        else return -3;
        // Create the Header dataset space.Set the size to be the current size.
        H5::DataSpace dspace_id(H5S_SCALAR);    // Dataspace identifier
        // cycle by attribute
        LOGICAL iError = 0;
        for (auto id = 0; id < iSizeOfHead; ++id)
        {
            try
            {
                // Open the Header attribute with Attribute identifier
                H5::Attribute attr_id = group_id.createAttribute(sHeadName[id].c_str(), stype_id, dspace_id);
                // Save Header to 1 attribute by type
                DataType a_ptr = cHead[id];     // Attribute Data Pointer
                // Write the Header attribute
                attr_id.write(stype_id, &a_ptr);
                // Close and release resources
                attr_id.close();
            }
            catch (const H5::AttributeIException&)
            {
                iError = -4;
                break;
            }
        }
        dspace_id.close();
        if (!sHeader.empty()) group_id.close();
        // iError = H5garbage_collect();
        return iError;
    }

    // Writing HDF5 attributes based on given vectors of names.
    // parent_id - Parent Group identifier as H5::Group
    LOGICAL HDF_HeaderWriteNames(H5::Group* parent_id, const STRING sHeader, V_STRING sHeadName, V_STRING cHead);
    // HDF Reader by H5::Group
    template<typename DataType>
    INT64 HDF_Read(DataType*& idata, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        // Open group.
        H5::Group group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id.openGroup(group));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = H5_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = H5_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = H5_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = H5_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = H5_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_LOGICAL;
        }
        else return -2;
        //
        INT64 length = ENV_0_I64;
        try
        {
            // 1) Read Length
            H5::Attribute attr_id = group_id.openAttribute(ARRAY_DATA_NAMES[0].c_str());	// Attribute identifier
            //attr_id.read(attr_id.getIntType(), &length);
            attr_id.read(h5type[0], &length);
            attr_id.close();
            // 2) Read type
            attr_id = group_id.openAttribute(ARRAY_DATA_NAMES[1].c_str());	// Attribute identifier
            STRING array_type = STRING_EMPTY;
            //attr_id.read(attr_id.getStrType(), array_type);
            attr_id.read(h5type[1], array_type);
            attr_id.close();
            if (ArrayType.compare(ENV::trim(array_type)) != 0) return -3;
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -4;
        }
        try
        {
            // 3) Read array
            PTR_DELETE(idata, []);
            if (length > ENV_0_I64)
            {
                HDF5_DIM1[0] = length;
                H5::DataSpace dspace_id(1, HDF5_DIM1);  // Dataspace identifier for array
                H5::DataSet dset_id;                        // Dataset identifier for array
                //H5::DSetAccPropList plist_id;               // Properties identifier = plist(compression)
                // Compression check
                bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
                if (is_compressed)
                {
                    // compressed data
                    //plist_id.getChunk(1, ENV_COMPRESSION_CHUNK_SIZE);
                    //plist_id.getDeflate(ENV_COMPRESSION_LEVEL);
                    dset_id = group_id.openDataSet(ARRAY_DATA_NAMES[3].c_str());
                }
                // uncompressed data
                else dset_id = group_id.openDataSet(ARRAY_DATA_NAMES[2].c_str());
                idata = new DataType[length];
                dset_id.read(idata, h5type[2], dspace_id);
                dset_id.close();
                dspace_id.close();
            }
            // close last group
            if (!group.empty()) group_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -5;
        }
        return length;
    }

    // HDF Reader of Datatype Vector by H5::Group
    template<typename DataType>
    LOGICAL HDF_Read(VECTOR<DataType>& idata, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        // Open group.
        H5::Group group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id.openGroup(group));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = H5_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = H5_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = H5_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = H5_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = H5_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_LOGICAL;
        }
        else return -2;
        //
        INT64 length = ENV_0_I64;
        try
        {
            // 1) Read Length
            H5::Attribute attr_id = group_id.openAttribute(ARRAY_DATA_NAMES[0].c_str());	// Attribute identifier
            attr_id.read(h5type[0], &length);
            attr_id.close();
            // 2) Read type
            attr_id = group_id.openAttribute(ARRAY_DATA_NAMES[1].c_str());	// Attribute identifier
            attr_id.read(h5type[0], &length);
            attr_id.close();
            STRING array_type = STRING_EMPTY;
            attr_id.read(h5type[1], array_type);
            attr_id.close();
            if (ArrayType.compare(ENV::trim(array_type)) != 0) return -3;
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -4;
        }
        try
        {
            // 3) Read array
            idata.clear();
            if (length > ENV_0_I64)
            {
                HDF5_DIM1[0] = length;
                H5::DataSpace dspace_id(1, HDF5_DIM1);  // Dataspace identifier for array
                H5::DataSet dset_id;                        // Dataset identifier for array
                //H5::DSetAccPropList plist_id;               // Properties identifier = plist(compression)
                // Compression check
                bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
                if (is_compressed)
                {
                    // compressed data
                    //plist_id.setChunk(1, ENV_COMPRESSION_CHUNK_SIZE);
                    //plist_id.setDeflate(ENV_COMPRESSION_LEVEL);
                    dset_id = group_id.openDataSet(ARRAY_DATA_NAMES[3].c_str());
                }
                // uncompressed data
                else dset_id = group_id.openDataSet(ARRAY_DATA_NAMES[2].c_str());
                DataType* p_idata = new DataType[length];
                dset_id.read(p_idata, h5type[2], dspace_id);
                for (INT64 i = ENV_0_I64; i < length; ++i) idata.push_back(p_idata[i]);
                PTR_DELETE(p_idata, []);
                //if (is_compressed) plist_id.close();
                dset_id.close();
                dspace_id.close();
            }
            // close last group
            if (!group.empty()) group_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -5;
        }
        return 0;
    }

    // HDF Reader of String Vector by H5::Group
    LOGICAL HDF_Read_Strings(V_STRING& idata, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false);

    // HDF Write by H5::Group
    template<typename DataType>
    LOGICAL HDF_Write(DataType* idata, const INT64 icount, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false, const INT64* slice = nullptr)
    {
        // Open group.
        H5::Group group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id.createGroup(group));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = H5_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = H5_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = H5_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = H5_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = H5_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_LOGICAL;
        }
        else return -2;
        //
        INT64 length = (idata) ? icount : ENV_0_I64;
        if (length && slice) length = slice[1] - slice[0] + ENV_1_I64;
        try
        {
            H5::DataSpace dspace_id(H5S_SCALAR);    // Dataspace identifier
            // 1) Write Length
            H5::Attribute attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[0].c_str(), h5type[0], dspace_id);	//  Attribute identifier
            attr_id.write(h5type[0], &length);
            attr_id.close();
            // 2) Write type
            attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[1].c_str(), h5type[1], dspace_id);
            attr_id.write(h5type[1], ArrayType);
            attr_id.close();
            dspace_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -3;
        }
        try
        {
            // 3) Write array
            if (length > ENV_0_I64)
            {
                HDF5_DIM1[0] = length;
                H5::DataSpace dspace_id(1, HDF5_DIM1);  // Dataspace identifier for array
                H5::DataSet dset_id;	        // Dataset identifier for array
                H5::DSetCreatPropList plist_id; // Properties identifier = plist(compression)
                // Compression check
                bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
                if (is_compressed)
                {
                    // compressed data
                    plist_id.setChunk(1, COMPRESSION_CHUNK_SIZE);
                    plist_id.setDeflate(COMPRESSION_LEVEL);
                    dset_id = group_id.createDataSet(ARRAY_DATA_NAMES[2].c_str(), h5type[2], dspace_id, plist_id);
                }
                // uncompressed data
                else dset_id = group_id.createDataSet(ARRAY_DATA_NAMES[3].c_str(), h5type[2], dspace_id);
                // write slice or full
                if (ArrayType.compare(ARRAY_TYPE_CHARNAME) == 0)
                {
                    STRING* p_idata = (STRING*)((slice) ? (idata + slice[0]) : idata);
                    // grab pointers to v_chars
                    VECTOR<const char*> v_chars;
                    for (INT64 i = ENV_0_I64; i < length; ++i)
                        v_chars.push_back(p_idata[i].data());
                    // create the variable length type
                    hvl_t hdf_buffer;
                    hdf_buffer.p = v_chars.data();
                    hdf_buffer.len = v_chars.size();
                    // write the output to a scalar dataset
                    dset_id.write(&hdf_buffer, H5_V_STRING);
                    v_chars.clear();
                }
                else
                {
                    DataType* p_idata = (slice) ? (idata + slice[0]) : idata;
                    dset_id.write(p_idata, h5type[2], dspace_id);
                }
                // plist(compression) close
                if (is_compressed) plist_id.close();
                dset_id.close();
                dspace_id.close();
            }
            // close last group
            if (!group.empty()) group_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -4;
        }
        return 0;
    }

    // HDF Write of Datatype Vector by H5::Group
    template<typename DataType>
    LOGICAL HDF_Write(VECTOR<DataType> idata, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        // Open group.
        H5::Group group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id.createGroup(group));
            }
            catch (const H5::GroupIException&)
            {
                return -1;
            }
        }
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = H5_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = H5_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = H5_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = H5_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = H5_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = H5_LOGICAL;
        }
        else return -2;
        //
        INT64 length = idata.size();
        try
        {
            H5::DataSpace dspace_id(H5S_SCALAR);    // Dataspace identifier
            // 1) Write Length
            H5::Attribute attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[0].c_str(), h5type[0], dspace_id);	// Attribute identifier
            attr_id.write(h5type[0], &length);
            attr_id.close();
            // 2) Write type
            attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[1].c_str(), h5type[1], dspace_id);
            attr_id.write(h5type[1], ArrayType);
            attr_id.close();
            dspace_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -3;
        }
        try
        {
            // 3) Write array
            if (length > ENV_0_I64)
            {
                HDF5_DIM1[0] = length;
                H5::DataSpace dspace_id(1, HDF5_DIM1);  // Dataspace identifier for array
                H5::DataSet dset_id;	        // Dataset identifier for array
                H5::DSetCreatPropList plist_id; // Properties identifier = plist(compression)
                // Compression check
                bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
                if (is_compressed)
                {
                    // compressed data
                    plist_id.setChunk(1, COMPRESSION_CHUNK_SIZE);
                    plist_id.setDeflate(COMPRESSION_LEVEL);
                    dset_id = group_id.createDataSet(ARRAY_DATA_NAMES[2].c_str(), h5type[2], dspace_id, plist_id);
                }
                // uncompressed data
                else dset_id = group_id.createDataSet(ARRAY_DATA_NAMES[3].c_str(), h5type[2], dspace_id);
                // write vector
                DataType* p_idata = new DataType[length];
                for (INT64 i = ENV_0_I64; i < length; ++i) p_idata[i] = idata[i];
                dset_id.write(p_idata, h5type[2], dspace_id);
                PTR_DELETE(p_idata, []);
                // plist(compression) close
                if (is_compressed) plist_id.close();
                dset_id.close();
                dspace_id.close();
            }
            // close last group
            if (!group.empty()) group_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -4;
        }
        return 0;
    }

    // HDF Write of String Vector by H5::Group
    LOGICAL HDF_Write_Strings(V_STRING idata, const H5::Group& parent_id, const STRING group = STRING("Array"), const bool compressed = false);

#else

    // Open or create HDF file by hid_t
        // status - open or create
        // available actions - for open(read, read - write), for create(truncate)
    hid_t HDF_file_open(const STRING file_name, const STRING status = "open", const STRING action = STRING_EMPTY);
    // Open or create HDF group by hid_t
    // status - open or create
    // available actions - for open(read, read - write), for create(truncate)
    hid_t HDF_group_open(const hid_t parent_id, const STRING file_name, const STRING status = "open", V_STRING* p_error_message = nullptr);
    // HDF file close by hid_t
    void HDF_file_close(hid_t hdf_file, const STRING file_name = STRING_EMPTY);
    // HDF group close by hid_t
    void HDF_group_close(hid_t group, const STRING group_name = STRING_EMPTY);

#endif

    // HDF5 initialization
    herr_t HDF_Init();
    // HDF5 finalization
    herr_t HDF_Finalize();

    // HDF Group Exists check by hid_t
    bool HDF_Group_Exists(const hid_t parent_id, const char* group);

    // HDF Group Exists check by hid_t for FORTRAN
    extern "C" void isGroupExists(hid_t*, const char [], int*);

    // Open Group anyway even if it's existed already or not
    // parent_id - Parent Group identifier as hid_t
    hid_t HDF_Group_Open(const hid_t parent_id, const STRING group, herr_t& iError);

    // Reading HDF5 attributes based on given vectors of names and types except STRING.
    // parent_id - Parent Group identifier as hid_t
    template<typename DataType>
    LOGICAL HDF_Read_Attributes(VECTOR<DataType>& cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader = STRING_EMPTY)
    {
        // Open Group
        hid_t group_id;   // Group identifier
        if (sHeader.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, sHeader);
            if (group_id < 0) return -1;
        };
        // check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead)) return -2;
        // Select type of Header
        hid_t dtype_id;  // Dataset element type
        if (typeid(DataType) == typeid(INT08))
            dtype_id = HDF5_TYPE_INT08;
        else if (typeid(DataType) == typeid(INT32))
            dtype_id = HDF5_TYPE_INT32;
        else if (typeid(DataType) == typeid(INT64))
            dtype_id = HDF5_TYPE_INT64;
        else if (typeid(DataType) == typeid(REAL32))
            dtype_id = HDF5_TYPE_REAL32;
        else if (typeid(DataType) == typeid(REAL64))
            dtype_id = HDF5_TYPE_REAL64;
        else if (typeid(DataType) == typeid(REAL128))
            dtype_id = HDF5_TYPE_REAL128;
        else if (typeid(DataType) == typeid(LOGICAL))
            dtype_id = HDF5_TYPE_LOGICAL;
        else return -3;
        // cycle by attribute
        LOGICAL iError = 0;
        for (size_t id = 0; id < iSizeOfHead; ++id)
        {
            // Open the Header attribute
            hid_t attr_id = H5Aopen(group_id, sHeadName[id].c_str(), H5P_DEFAULT); // Attribute identifier
            if (attr_id < 0)
            {
                iError = -4;
                break;
            }
            DataType a_ptr;     // Attribute Data Pointer
            // Read the Header attribute
            if (H5Aread(attr_id, dtype_id, &a_ptr) < 0)
            {
                iError = -5;
                break;
            }
            // Save attribute to Header
            cHead[id] = a_ptr;
            // Close and release resources
            if (H5Aclose(attr_id) < 0)
            {
                iError = -6;
                break;
            }
        }
        if (sHeader.length()) HDF_group_close(group_id, sHeader);
        return iError;
    }

    // Reading HDF5 attributes based on given vectors of names of type STRING.
        // parent_id - Parent Group identifier as hid_t
    LOGICAL HDF_Read_Attributes_String(V_STRING& cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader = STRING_EMPTY);

    // Writing HDF5 attributes based on given vectors of names and types except STRING.
    // parent_id - Parent Group identifier as hid_t
    template<typename DataType>
    LOGICAL HDF_Write_Attributes(VECTOR<DataType> cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader = STRING_EMPTY)
    {
        // Open Group.
        hid_t group_id;   // Group identifier
        if (sHeader.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, sHeader, "create");
            if (group_id < 0) return -1;
        };
        //  check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead)) return -2;
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        // Select type of Header
        hid_t dtype_id;  // Dataset element type
        if (typeid(DataType) == typeid(INT08))
            dtype_id = HDF5_TYPE_INT08;
        else if (typeid(DataType) == typeid(INT32))
            dtype_id = HDF5_TYPE_INT32;
        else if (typeid(DataType) == typeid(INT64))
            dtype_id = HDF5_TYPE_INT64;
        else if (typeid(DataType) == typeid(REAL32))
            dtype_id = HDF5_TYPE_REAL32;
        else if (typeid(DataType) == typeid(REAL64))
            dtype_id = HDF5_TYPE_REAL64;
        else if (typeid(DataType) == typeid(REAL128))
            dtype_id = HDF5_TYPE_REAL128;
        else if (typeid(DataType) == typeid(LOGICAL))
            dtype_id = HDF5_TYPE_LOGICAL;
        else return -3;
        //
        // Create the Header dataset space.Set the size to be the current size.
        HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);
        // cycle by attribute
        LOGICAL iError = 0;
        for (size_t id = 0; id < iSizeOfHead; ++id)
        {
            // Open the Header attribute with Attribute identifier
            hid_t attr_id = H5Acreate(group_id, sHeadName[id].c_str(), dtype_id, dspace_id, H5P_DEFAULT, H5P_DEFAULT);
            if (attr_id < 0)
            {
                iError = -4;
                break;
            }
            // Save Header to 1 attribute by type
            DataType a_ptr = cHead[id];     // Attribute Data Pointer
            // Write the Header attribute
            if (H5Awrite(attr_id, dtype_id, &a_ptr) < 0)
            {
                iError = -5;
                break;
            }
            // Close and release resources
            if (H5Aclose(attr_id) < 0)
            {
                iError = -6;
                break;
            }
        }
        H5Sclose(dspace_id);
        if (!sHeader.empty()) HDF_group_close(group_id, sHeader);
        // iError = H5garbage_collect();
        return iError;
    }

    // Writing HDF5 attributes based on given vectors of names of type STRING.
    // parent_id - Parent Group identifier as hid_t
    LOGICAL HDF_Write_Attributes_String(V_STRING cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader = STRING_EMPTY);

    // HDF Reader of Datatype Vector by hid_t
    template<typename DataType>
    LOGICAL HDF_Read(VECTOR<DataType>& idata, const hid_t parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        //std::cerr << "'" + group + "'" << END_LINE;
        // Open group.
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group);
            if (group_id < 0) return -1;
        };
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = HDF5_TYPE_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = HDF5_TYPE_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = HDF5_TYPE_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = HDF5_TYPE_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = HDF5_TYPE_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_LOGICAL;
        }
        else return -2;
        //
        // 1) Read Length
        hid_t attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[0].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0) return -3;
        INT64 length = ENV_0_I64;
        if (H5Aread(attr_id, h5type[0], &length) < 0) return -4;
        if (H5Aclose(attr_id) < 0) return -5;
        // 2) Read type
        attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[1].c_str(), H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -6;
        char array_type[HDF5_NAME_LENGTH + 1]{};
        auto attr_type = H5Aget_type(attr_id);
        herr_t ierror = (H5Tget_class(attr_type) == H5T_STRING) ? H5Aread(attr_id, H5Tget_native_type(attr_type, H5T_DIR_ASCEND), &array_type) : -7;
        H5Tclose(attr_type);
        if (ierror < 0) return -7;
        if (H5Aclose(attr_id) < 0) return -8;
        if (ArrayType.compare(ENV::trim(array_type)) != 0) return -9;
        idata.clear();
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL); // Dataspace identifier for array
            if (dspace_id < 0) return -10;
            // Dataset identifier for array
            hid_t dset_id = (compressed && (length > COMPRESSION_START_LENGTH)) // Compression check
                ? H5Dopen2(group_id, ARRAY_DATA_NAMES[3].c_str(), H5P_DEFAULT)      // compressed data
                : H5Dopen2(group_id, ARRAY_DATA_NAMES[2].c_str(), H5P_DEFAULT);     // uncompressed data
            if (dset_id < 0) return -11;
            DataType* p_idata = new DataType[length];
            herr_t ierror = H5Dread(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, p_idata);
            idata.reserve(length);
            for (INT64 i = ENV_0_I64; i < length; ++i) idata.emplace_back(p_idata[i]);
            PTR_DELETE(p_idata, []);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -12;
        }
        // close last group
        if (!group.empty()) HDF_group_close(group_id, group);
        return 0;
    }

    // HDF Reader by hid_t
    template<typename DataType>
    INT64 HDF_Read(DataType*& idata, const hid_t parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        //std::cerr << "'" + group + "'" << END_LINE;
        // Open group.
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group);
            if (group_id < 0) return -1;
        };
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = HDF5_TYPE_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = HDF5_TYPE_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = HDF5_TYPE_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = HDF5_TYPE_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = HDF5_TYPE_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_LOGICAL;
        }
        else return -2;
        //
        // 1) Read Length
        hid_t attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[0].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0) return -3;
        INT64 length = ENV_0_I64;
        if (H5Aread(attr_id, H5Tget_native_type(H5Aget_type(attr_id), H5T_DIR_ASCEND), &length) < 0) return -4;
        if (H5Aclose(attr_id) < 0) return -5;
        // 2) Read type
        attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[1].c_str(), H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -6;
        char array_type[HDF5_NAME_LENGTH + 1]{};
        auto attr_type = H5Aget_type(attr_id);
        herr_t ierror = (H5Tget_class(attr_type) == H5T_STRING) ? H5Aread(attr_id, H5Tget_native_type(attr_type, H5T_DIR_ASCEND), &array_type) : -7;
        H5Tclose(attr_type);
        if (ierror < 0) return -7;
        if (H5Aclose(attr_id) < 0) return -8;
        if (ArrayType.compare(ENV::trim(array_type)) != 0) return -9;
        // 3) Read array
        PTR_DELETE(idata, []);
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL); // Dataspace identifier for array
            if (dspace_id < 0) return -10;
            // Dataset identifier for array
            hid_t dset_id = (compressed && (length > COMPRESSION_START_LENGTH)) // Compression check
                ? H5Dopen2(group_id, ARRAY_DATA_NAMES[3].c_str(), H5P_DEFAULT)      // compressed data
                : H5Dopen2(group_id, ARRAY_DATA_NAMES[2].c_str(), H5P_DEFAULT);     // uncompressed data
            if (dset_id < 0) return -11;
            idata = new DataType[length];
            herr_t ierror = H5Dread(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, idata);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -12;
        }
        if (!group.empty()) HDF_group_close(group_id, group);
        return length;
    }

    // HDF Reader of String Vector by hid_t
    LOGICAL HDF_Read_Strings(V_STRING& idata, const hid_t parent_id, const STRING group = STRING("Array"), const bool compressed = false);

    // HDF Write of Datatype Vector by hid_t
    template<typename DataType>
    LOGICAL HDF_Write(VECTOR<DataType>& idata, const hid_t parent_id, const STRING group = STRING("Array"), const bool compressed = false)
    {
        //std::cerr << "'" + group + "'" << END_LINE;
        // Open group.
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group, "create");
            if (group_id < 0) return -1;
        }
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = HDF5_TYPE_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = HDF5_TYPE_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = HDF5_TYPE_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = HDF5_TYPE_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = HDF5_TYPE_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_LOGICAL;
        }
        else return -2;
        //
        // Create the Header dataset space.Set the size to be the current size.
        HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);
        // 1) Write Length
        hid_t attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[0].c_str(), h5type[0], dspace_id, H5P_DEFAULT, H5P_DEFAULT);   // Attribute identifier
        if (attr_id < 0) return -4;
        INT64 length = idata.size();
        if (H5Awrite(attr_id, h5type[0], &length) < 0) return -5;
        if (H5Aclose(attr_id) < 0) return -6;
        // 2) Write type
        //h5type[1] = H5Tcopy(H5T_FORTRAN_S1);
        //H5Tset_size(h5type[1], HDF5_NAME_LENGTH);
        attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[1].data(), h5type[1], dspace_id, H5P_DEFAULT, H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -7;
        if (H5Awrite(attr_id, h5type[1], ArrayType.data()) < 0) return -8;
        if (H5Aclose(attr_id) < 0) return -9;
        //H5Tclose(h5type[1]);
        H5Sclose(dspace_id);
        // 3) Write array
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL); // Dataspace identifier for array
            hid_t dset_id = 0;  // Dataset identifier for array
            hid_t plist_id = 0; // Properties identifier = plist(compression)
            // Compression check
            bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
            if (is_compressed)
            {
                // compression and write
                hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
                if (plist_id < 0) return -10;
                if (H5Pset_chunk(plist_id, 1, COMPRESSION_CHUNK_SIZE) < 0) return -11;
                if (H5Pset_deflate(plist_id, COMPRESSION_LEVEL) < 0) return -12;
                dset_id = H5Dcreate2(group_id, ARRAY_DATA_NAMES[2].c_str(), h5type[2], dspace_id, H5P_DEFAULT, plist_id, H5P_DEFAULT);
            }
            // uncompressed write
            else dset_id = H5Dcreate2(group_id, ARRAY_DATA_NAMES[3].c_str(), h5type[2], dspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            // write vector
            DataType* p_idata = new DataType[length];
            for (INT64 i = ENV_0_I64; i < length; ++i) p_idata[i] = idata[i];
            herr_t ierror = H5Dwrite(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, p_idata);
            PTR_DELETE(p_idata, []);
            // plist(compression) close
            if (is_compressed) H5Pclose(plist_id);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -13;
        }
        // close last group
        if (!group.empty()) HDF_group_close(group_id, group);
        return 0;
    }

    // HDF Write by hid_t
    template<typename DataType>
    LOGICAL HDF_Write(DataType* idata, const INT64 icount, const hid_t parent_id, const STRING group = STRING("Array"), const bool compressed = false, const INT64* slice = nullptr)
    {
        //std::cerr << "'" + group + "'" << END_LINE;
        // Open group.
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group, "create");
            if (group_id < 0) return -1;
        }
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        STRING ArrayType;
        if (typeid(DataType) == typeid(REAL64))
        {
            ArrayType = ARRAY_TYPE_REAL64;
            h5type[2] = HDF5_TYPE_REAL64;
        }
        else if (typeid(DataType) == typeid(INT32))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_INT32;
        }
        else if (typeid(DataType) == typeid(INT08))
        {
            ArrayType = ARRAY_TYPE_INT08;
            h5type[2] = HDF5_TYPE_INT08;
        }
        else if (typeid(DataType) == typeid(INT64))
        {
            ArrayType = ARRAY_TYPE_INT64;
            h5type[2] = HDF5_TYPE_INT64;
        }
        else if (typeid(DataType) == typeid(REAL32))
        {
            ArrayType = ARRAY_TYPE_REAL32;
            h5type[2] = HDF5_TYPE_REAL32;
        }
        else if (typeid(DataType) == typeid(REAL128))
        {
            ArrayType = ARRAY_TYPE_REAL128;
            h5type[2] = HDF5_TYPE_REAL128;
        }
        else if (typeid(DataType) == typeid(LOGICAL))
        {
            ArrayType = ARRAY_TYPE_INT32;
            h5type[2] = HDF5_TYPE_LOGICAL;
        }
        else return -2;
        //
        // Create the Header dataset space.Set the size to be the current size.
        HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);
        // 1) Write Length
        hid_t attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[0].c_str(), h5type[0], dspace_id, H5P_DEFAULT, H5P_DEFAULT);   // Attribute identifier
        if (attr_id < 0) return -4;
        INT64 length = (idata == nullptr) ? ENV_0_I64 : icount;
        if (length && slice) length = slice[1] - slice[0] + ENV_1_I64;
        if (H5Awrite(attr_id, h5type[0], &length) < 0) return -5;
        if (H5Aclose(attr_id) < 0) return -6;
        // 2) Write type
        // Create string type for the Name attribute.
        //h5type[1] = H5Tcopy(H5T_C_S1);
        //H5Tset_size(h5type[1], HDF5_NAME_LENGTH);
        attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[1].data(), h5type[1], dspace_id, H5P_DEFAULT, H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -7;
        if (H5Awrite(attr_id, h5type[1], ArrayType.data()) < 0) return -8;
        if (H5Aclose(attr_id) < 0) return -9;
        //H5Tclose(h5type[1]);
        H5Sclose(dspace_id);
        // 3) Write array
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL); // Dataspace identifier for array
            hid_t dset_id = 0;  // Dataset identifier for array
            hid_t plist_id = 0; // Properties identifier = plist(compression)
            // Compression check
            bool is_compressed = (compressed && (length > COMPRESSION_START_LENGTH));
            if (is_compressed)
            {
                // compression and write
                hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
                if (plist_id < 0) return -10;
                if (H5Pset_chunk(plist_id, 1, COMPRESSION_CHUNK_SIZE) < 0) return -11;
                if (H5Pset_deflate(plist_id, COMPRESSION_LEVEL) < 0) return -12;
                dset_id = H5Dcreate2(group_id, ARRAY_DATA_NAMES[2].c_str(), h5type[2], dspace_id, H5P_DEFAULT, plist_id, H5P_DEFAULT);
            }
            // uncompressed write
            else dset_id = H5Dcreate2(group_id, ARRAY_DATA_NAMES[3].c_str(), h5type[2], dspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            // write slice or full
            DataType* p_idata = (slice) ? (idata + slice[0]) : idata;
            herr_t ierror = H5Dwrite(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, p_idata);
            // plist(compression) close
            if (is_compressed) H5Pclose(plist_id);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -13;
        }
        // close last group
        if (!group.empty()) HDF_group_close(group_id, group);
        return 0;
    }

    // HDF Write of String Vector by hid_t
    LOGICAL HDF_Write_Strings(V_STRING& idata, const hid_t parent_id, const STRING group = STRING("Array"), const bool isName = false, const bool compressed = false);

} // namespace ENV_H5

#endif
