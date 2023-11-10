#pragma once

#include "ENV_types.h"
#include "hdf5.h"

// for DEBUG
INT64
counter_h5fopen_f = ENV_0_I64,
counter_h5fcreate_f = ENV_0_I64,
counter_h5fclose_f = ENV_0_I64;
//
LOGICAL ENV_HDF5_IS_INITIALIZED = false;
// compression
INT32 const
ENV_COMPRESSION_LEVEL = 6i32,
ENV_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
/*
// INT32
INT32 const
ENV_INT32_COMPRESSION_LEVEL = 6i32,
ENV_INT32_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_INT32_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
// INT64
INT32 const
ENV_INT64_COMPRESSION_LEVEL = 6i32,
ENV_INT64_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_INT64_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
// REAL32
INT32 const
ENV_REAL32_COMPRESSION_LEVEL = 6i32,
ENV_REAL32_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_REAL32_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
// REAL64
INT32 const
ENV_REAL64_COMPRESSION_LEVEL = 6i32,
ENV_REAL64_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_REAL64_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
// REAL128
INT32 const
ENV_REAL128_COMPRESSION_LEVEL = 6i32,
ENV_REAL128_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_REAL128_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
// STR NAMES
INT32 const
ENV_STR_COMPRESSION_LEVEL = 6i32,
ENV_STR_COMPRESSION_START_LENGTH = 1100000i32;
hsize_t const
ENV_STR_COMPRESSION_CHUNK_SIZE[1] = { pow(2ul, 20ul) };
*/
STRING const
ARRAY_TYPE_INT08 = "INT:08",
ARRAY_TYPE_INT32 = "INT:32",
ARRAY_TYPE_INT64 = "INT:64",
ARRAY_TYPE_REAL32 = "REAL:32",
ARRAY_TYPE_REAL64 = "REAL:64",
ARRAY_TYPE_REAL128 = "REAL:64",
ARRAY_TYPE_CHARNAME = "STR:ENV_CHAR_NAME_LENGTH",
ARRAY_TYPE_STRING = "STR:ENV_STRING_LENGTH";
//
INT32 const ARRAY_DATA_COUNT = 4i32;
STRING const
ARRAY_DATA_NAMES[ARRAY_DATA_COUNT] =
{
"Length",
"Type",
"Data",
"Data"
};
//
hid_t
ENV_HDF5_INT08, ENV_HDF5_INT32, ENV_HDF5_INT64,
ENV_HDF5_REAL32, ENV_HDF5_REAL64, ENV_HDF5_REAL128,
ENV_HDF5_STRTYPE_NAME, ENV_HDF5_STRTYPE_STRING,
ENV_HDF5_LOGICAL;
VECTOR<hid_t> ENV_HDF5_TYPES;
hsize_t
ENV_HDF5_DIM1[1],
ENV_HDF5_DIM2[2],
ENV_HDF5_DIM3[3];

// HDF Group Exists check
herr_t HDF_Group_Exists(hid_t const parent_id, const char* group)
{
    // turn off warnings
    void* oldclientdata = NULL;
    /* Used to avoid stack trace to be displayed */
    H5E_auto2_t oldfunc;
    /* Save old error handler */
    H5Eget_auto2(H5E_DEFAULT, &oldfunc, &oldclientdata);
    /* Turn off error handling */
    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
    herr_t iError;
    try
    {
        iError = H5Gget_objinfo(parent_id, group, 0, NULL);
    }
    catch (...)
    {
        iError = -1;
    }
    /* Restore previous error handler */
    H5Eset_auto2(H5E_DEFAULT, oldfunc, oldclientdata);
    return iError;
}

// Open Group anyway even if it's existed already or not
// parent_id - Parent Group identifier
hid_t HDF_Group_Open(hid_t const parent_id, STRING const GROUP, herr_t& iError)
{
    iError = HDF_Group_Exists(parent_id, GROUP.c_str());
    return (iError < 0) ? H5Gcreate2(parent_id, GROUP.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT) : H5Gopen2(parent_id, GROUP.c_str(), H5P_DEFAULT);
}

// HDF5 initialization
herr_t HDF_Init()
{
    herr_t iError = ENV_HDF5_IS_INITIALIZED;
    if (ENV_HDF5_IS_INITIALIZED) return iError;
    ENV_HDF5_IS_INITIALIZED = false;
    iError = H5open();
    if (iError < 0) return iError;
    ENV_HDF5_INT08 = H5T_NATIVE_INT8;
    ENV_HDF5_INT32 = H5T_NATIVE_INT32;
    ENV_HDF5_INT64 = H5T_NATIVE_INT64;
    ENV_HDF5_REAL32 = H5T_NATIVE_FLOAT;
    ENV_HDF5_REAL64 = H5T_NATIVE_DOUBLE;
    ENV_HDF5_REAL128 = H5T_NATIVE_LDOUBLE;
    ENV_HDF5_LOGICAL = ENV_HDF5_INT32;
    // Create string type for the String attribute.
    ENV_HDF5_STRTYPE_NAME = H5Tcopy(H5T_C_S1);
    iError = H5Tset_size(ENV_HDF5_STRTYPE_NAME, ENV_CHAR_NAME_LENGTH);  // H5T_VARIABLE
    if (iError < 0) return iError;
    ENV_HDF5_STRTYPE_STRING = H5Tcopy(H5T_C_S1);
    iError = H5Tset_size(ENV_HDF5_STRTYPE_STRING, ENV_CHAR_STRING_LENGTH);  // H5T_VARIABLE
    if (iError < 0) return iError;
    ENV_HDF5_TYPES =
    {
        ENV_HDF5_REAL64, ENV_HDF5_REAL32, ENV_HDF5_INT64, ENV_HDF5_INT32,
        ENV_HDF5_STRTYPE_NAME, ENV_HDF5_STRTYPE_STRING, ENV_HDF5_INT08
    };
    ENV_HDF5_IS_INITIALIZED = true;
}

// HDF5 finalization
INT32 HDF_Finalize()
{
    herr_t iError = ENV_HDF5_IS_INITIALIZED;
    if (!ENV_HDF5_IS_INITIALIZED) return iError;
    iError = H5close();
    if (iError < 0) return iError;
    iError = H5Tclose(ENV_HDF5_STRTYPE_NAME);
    iError = H5Tclose(ENV_HDF5_STRTYPE_STRING);
    ENV_HDF5_IS_INITIALIZED = false;
}

// Open or create HDF - file
// status - open or create
// available actions - for open(read, read - write), for create(truncate)
hid_t HDF_file_open(STRING const& file_name, STRING const& status = "open", STRING const& action = STRING_EMPTY)
{
    VECTOR<STRING> error_message = { "HDF-file open ERROR:", "", "" };
    // Create the file
    STRING hdf_action = action;
    if (hdf_action.empty())
    {
        if (status.compare("open") == 0) hdf_action = "read";
        else if (status.compare("create") == 0) hdf_action = "truncate";
    }
    int iError = ENV_0_I32;
    unsigned int hdf_flag;
    hid_t hdf_unit;
    if (status.compare("open") == 0)
    {
        if (action.compare("read") == 0) hdf_flag = H5F_ACC_RDONLY;
        else if (action.compare("read-write") == 0) hdf_flag = H5F_ACC_RDWR;
        else
        {
            error_message[1] = " cannot open HDF file with status '" + status + "', and action '" + hdf_action + "'";
            ENV_StopByError(3005, &error_message);
        }
        hdf_unit = H5Fopen(file_name.c_str(), hdf_flag, H5P_DEFAULT);
        if (hdf_unit == H5I_INVALID_HID) iError = ENV_1M_I32;
    }
    else if (status.compare("create") == 0)
    {
        if (hdf_action.compare("truncate") == 0)
        {
            hdf_flag = H5F_ACC_TRUNC;
            iError = std::remove(file_name.c_str());  // delete file
            if (iError != ENV_0_I32)
            {
                std::cout << "Error #" << iError << "." << ENDLINE;
                std::cout << " Cannot delete file: '" + file_name + "'." << ENDLINE;
                error_message[1] = " cannot open HDF file with status '" + status + "' and action '" + hdf_action + "'";
                ENV_StopByError(3005, &error_message);
            }
            hdf_unit = H5Fcreate(file_name.c_str(), hdf_flag, H5P_DEFAULT, H5P_DEFAULT);
            if (hdf_unit == H5I_INVALID_HID) iError = ENV_1M_I32;
        }
        else
        {
            error_message[1] = " cannot open HDF-file with status '" + status + "' and action '" + hdf_action + "'";
            ENV_StopByError(-3005, &error_message);
        }
    }
    if (iError < ENV_0_I32)
    {
        error_message[1] = " ERROR in HDF-file open. Cannot open hdf5-file (H5F" + status + "()): '" + file_name + "', error = " + std::to_string(iError);
        error_message[2] = " status '" + status + "' and action '" + hdf_action + "'";
        ENV_StopByError(3005, &error_message);
    }
    return hdf_unit;
}

// HDF - file close
void HDF_file_close(hid_t const hdf_unit, STRING const file_name = STRING_EMPTY)
{
    herr_t iError, iError1, iError2;
    iError1 = H5Fflush(hdf_unit, H5F_SCOPE_GLOBAL);
    if (iError1 < ENV_0_I32)
    {
        std::cout << "Error #" << iError1 << ENDLINE;
        std::cout << " Cannot flush HDF-file: '" + file_name + "'." << ENDLINE;
    }
    iError = H5Fclose(hdf_unit);
    iError2 = H5garbage_collect();
    if (iError2 < 0)
    {
        std::cout << "Error #", iError2;
        std::cout << " Cannot garbage collect HDF-file: '" + trim(file_name) + "'.";
    }
    if (iError < 0)
    {
        VECTOR<STRING> error_message = { "HDF-file open ERROR:", "", "" };
        if (file_name.empty())
        {
            error_message[1] = "ERROR in HDF-file close. Cannot close HDF-file.";
            ENV_StopByError(3005, &error_message);
        }
        else
        {
            error_message[1] = "ERROR in HDF-file close. Cannot close HDF-file: '" + file_name + "'.";
            ENV_StopByError(3005, &error_message);
        }
    }
}

// Reading HDF5 attributes based on given vectors of names and types.
// parent_id - Parent Group identifier
template<typename DataType>
LOGICAL HDF_HeaderReadSimple(hid_t const parent_id, STRING const sHeader, VECTOR<STRING const>& sHeadName, VECTOR<DataType>& cHead, INT32* pError = NULL)
{
    INT32 iError = ENV_0_I32;
    // Open the Header dataset and attributes.
    hid_t dset_id;   // Dataset identifier
    if (sHeader.length())
    {
        if (H5Gopen(parent_id, sHeader.c_str(), dset_id) < 0)
        {
            iError = ENV_1M_I32;
            if (pError) *pError = iError;
            return false;
        }
    }
    else dset_id = parent_id;
    // check for dimensions
    size_t iSizeOfHead = cHead.size();
    if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead))
    {
        iError = ENV_1M_I32;
        if (pError) *pError = iError;
        return false;
    }
    // Select type of Header
    hid_t stype_id;  // Dataset element type
    if (typeid(DataType) == typeid(LOGICAL))
        stype_id = ENV_HDF5_LOGICAL;
    else if (typeid(DataType) == typeid(INT08))
        stype_id = ENV_HDF5_INT08;
    else if (typeid(DataType) == typeid(INT32))
        stype_id = ENV_HDF5_INT32;
    else if (typeid(DataType) == typeid(INT64))
        stype_id = ENV_HDF5_INT64;
    else if (typeid(DataType) == typeid(REAL32))
        stype_id = ENV_HDF5_REAL32;
    else if (typeid(DataType) == typeid(REAL64))
        stype_id = ENV_HDF5_REAL64;
    else if (typeid(DataType) == typeid(REAL128))
        stype_id = ENV_HDF5_REAL128;
    else if (typeid(DataType) == typeid(STRING))
        stype_id = (max_line_length(cHead) > ENV_CHAR_NAME_LENGTH) ? ENV_HDF5_STRTYPE_STRING : ENV_HDF5_STRTYPE_NAME;
    else
    {
        iError = ENV_1_I32;
        if (pError) *pError = iError;
        return false;
    }
    // cycle by attribute
    for (auto id = 0; id < iSizeOfHead; ++id)
    {
        // Open the Header attribute
        hid_t attr_id = H5Aopen(dset_id, (sHeadName)[id].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0)
        {
            iError = ENV_2M_I32;
            break;
        }
        DataType a_ptr;     // Attribute Data Pointer
        // Read the Header attribute
        if (H5Aread(attr_id, stype_id, &a_ptr) < 0)
        {
            iError = ENV_2M_I32;
            break;
        }
        // Save attribute to Header
        cHead[id] = a_ptr;
        // Close and release resources
        if (H5Aclose(attr_id) < 0)
        {
            iError = ENV_1M_I32;
            break;
        }
    }
    if (sHeader.length()) iError = H5Gclose(dset_id);
    if (pError) *pError = iError;
    return (iError == ENV_0_I32);
}

// Writing HDF5 attributes based on given vectors of names and types.
// parent_id - Parent Group identifier
template<typename DataType>
LOGICAL HDF_HeaderWriteSimple(hid_t const parent_id, STRING const sHeader, VECTOR<STRING const> sHeadName, VECTOR<DataType> cHead, INT32* pError = NULL)
{
    INT32 iError = ENV_0_I32;
    // Open dataspace.
    hid_t dset_id;   // Dataset identifier
    if (sHeader.length())
    {
        if (H5Gcreate2(parent_id, sHeader.c_str(), dset_id, 0, 0) < 0)
        {
            iError = ENV_1M_I32;
            if (pError) *pError = iError;
            return false;
        }
    }
    else dset_id = parent_id;
    //  check for dimensions
    size_t iSizeOfHead = cHead.size();
    if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead))
    {
        iError = ENV_1M_I32;
        if (pError) *pError = iError;
        return false;
    }
    size_t iSizeOfHead = cHead.size();
    // Select type of Header
    hid_t stype_id;  // Dataset element type
    if (typeid(DataType) == typeid(LOGICAL))
        stype_id = ENV_HDF5_LOGICAL;
    else if (typeid(DataType) == typeid(INT08))
        stype_id = ENV_HDF5_INT08;
    else if (typeid(DataType) == typeid(INT32))
        stype_id = ENV_HDF5_INT32;
    else if (typeid(DataType) == typeid(INT64))
        stype_id = ENV_HDF5_INT64;
    else if (typeid(DataType) == typeid(REAL32))
        stype_id = ENV_HDF5_REAL32;
    else if (typeid(DataType) == typeid(REAL64))
        stype_id = ENV_HDF5_REAL64;
    else if (typeid(DataType) == typeid(REAL128))
        stype_id = ENV_HDF5_REAL128;
    else if (typeid(DataType) == typeid(STRING))
        stype_id = (max_line_length(cHead) > ENV_CHAR_NAME_LENGTH) ? ENV_HDF5_STRTYPE_STRING : ENV_HDF5_STRTYPE_NAME;
    else
    {
        iError = ENV_1_I32;
        if (pError) *pError = iError;
        return false;
    }
    // Create the Header dataset space.Set the size to be the current size.
    ENV_HDF5_DIM1 = { 1 };
    hsize_t dspace_id;     // Dataspace identifier
    dspace_id = H5Screate_simple(1, ENV_HDF5_DIM1, ENV_HDF5_DIM1);
    // cycle by attribute
    for (auto id = 0; id < iSizeOfHead; ++id)
    {
        // Open the Header attribute with Attribute identifier
        hid_t attr_id = H5Acreate2(dset_id, sHeadName[id].c_str(), stype_id, dspace_id, H5P_DEFAULT, H5P_DEFAULT);
        if (attr_id < 0)
        {
            iError = ENV_2M_I32;
            break;
        }
        // Save Header to 1 attribute by type
        DataType a_ptr = cHead[id];     // Attribute Data Pointer
        // Write the Header attribute
        if (H5Awrite(attr_id, stype_id, &a_ptr) < 0)
        {
            iError = ENV_2M_I32;
            break;
        }
        // Close and release resources
        if (H5Aclose(attr_id) < 0)
        {
            iError = ENV_1M_I32;
            break;
        }
    }
    iError = H5Sclose(dspace_id);
    if (sHeader.length()) iError = H5Gclose(dset_id);
    // iError = H5garbage_collect();
    if (pError) *pError = iError;
    return (iError == ENV_0_I32);
}

// HDF Reader
template<typename DataType>
LOGICAL HDF_Read(DataType* idata, hid_t const parent_id, STRING const group = STRING("Array"), bool compressed = false)
{
    /*
    logical::HDF_Read_int8
    integer(int8), allocatable, intent(inout)   ::idata(:)
    integer(HID_T), intent(in)                  ::parent_id
    character(len = *), intent(in), optional::group
    logical, intent(in), optional::compressed
    logical::is_compressed
    integer(HID_T)  ::group_id
    character(len = ENV_CHAR_STRING_LENGTH)   ::GroupName
    integer(int32)  ::iError, RANK
    integer(HID_T)  ::dset_id     !Dataset identifier
    integer(HID_T)  ::dspace_id   !Dataspace identifier
    integer(HID_T)  ::attr_id
    */
    hid_t group_id = parent_id;
    if (!group.empty())
    {
        group_id = H5Gopen2(parent_id, group.c_str(), H5P_DEFAULT);
        if (group_id != H5I_INVALID_HID) return false;
    }
    VECTOR<hid_t> h5type = { ENV_HDF5_INT64, ENV_HDF5_STRTYPE_NAME, ENV_HDF5_INT32 };
    STRING ArrayType;
    if (typeid(DataType) == typeid(LOGICAL))
    {
        ArrayType = ARRAY_TYPE_INT32;
        h5type[2] = ENV_HDF5_INT32;
    }
    else if (typeid(DataType) == typeid(INT32))
    {
        ArrayType = ARRAY_TYPE_INT32;
        h5type[2] = ENV_HDF5_INT32;
    }
    else if (typeid(DataType) == typeid(INT64))
    {
        ArrayType = ARRAY_TYPE_INT64;
        h5type[2] = ENV_HDF5_INT64;
    }
    else if (typeid(DataType) == typeid(INT08))
    {
        ArrayType = ARRAY_TYPE_INT08;
        h5type[2] = ENV_HDF5_INT08;
    }
    else if (typeid(DataType) == typeid(REAL32))
    {
        ArrayType = ARRAY_TYPE_REAL32;
        h5type[2] = ENV_HDF5_REAL32;
    }
    else if (typeid(DataType) == typeid(REAL64))
    {
        ArrayType = ARRAY_TYPE_REAL64;
        h5type[2] = ENV_HDF5_REAL64;
    }
    else if (typeid(DataType) == typeid(REAL128))
    {
        ArrayType = ARRAY_TYPE_REAL128;
        h5type[2] = ENV_HDF5_REAL128;
    }
    else if (typeid(DataType) == typeid(STRING))
    {
        ArrayType = ARRAY_TYPE_STRING;
        h5type[2] = ENV_HDF5_STRTYPE_STRING;
    }
    else return false;
    //
    ENV_HDF5_DIM1 = { 1 };
    // 1) Read Length
    hid_t attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[0].c_str(), H5P_DEFAULT); // Attribute identifier
    if (attr_id < 0) return false;
    INT64 length;
    if (H5Aread(attr_id, h5type[0], &length) < 0) return false;
    if (H5Aclose(attr_id) < 0) return false;
    // 1.1) Compression
    bool is_compressed = (compressed && (length > ENV_COMPRESSION_START_LENGTH));
    // 2) Read type
    attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[1].c_str(), H5P_DEFAULT);
    if (attr_id < 0) return false;
    char array_type[ENV_CHAR_STRING_LENGTH];
    if (H5Aread(attr_id, h5type[1], &array_type) < 0) return false;
    if (H5Aclose(attr_id) < 0) return false;
    if (ArrayType.compare(array_type) != 0)
    {
        if (STRING(array_type).compare(ARRAY_TYPE_CHARNAME) == 0)
        {
            ArrayType = ARRAY_TYPE_CHARNAME;
            h5type[2] = ENV_HDF5_STRTYPE_NAME;
        }
        else return false;
    }
    // 3) read array
    if (length > ENV_0_I64)
    {
        idata = new DataType[length];
        ENV_HDF5_DIM1 = { length };
        hid_t dset_id = (is_compressed)
            ? H5Dopen2(group_id, ARRAY_DATA_NAMES[3].c_str(), H5P_DEFAULT)
            : H5Dopen2(group_id, ARRAY_DATA_NAMES[2].c_str(), H5P_DEFAULT);
        if (dset_id < 0) return false;
        H5Dread(dset_id, h5type[2], &idata, ENV_HDF5_DIM1);
        H5Dclose(dset_id);
    }
    if (!group.empty()) H5Gclose(group_id);
}
