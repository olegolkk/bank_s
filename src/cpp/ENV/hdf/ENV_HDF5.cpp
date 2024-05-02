#include "ENV_HDF5.hpp"

#ifdef ENV_HDF5_HPP

namespace ENV_H5
{
#ifdef H5Cpp_H
    // Open or create HDF - file by H5::H5File
    // status - open or create
    // available actions - for open(read, read - write), for create(truncate)
    H5::H5File* HDF_file_open(const STRING& file_name, const STRING& status, const STRING& action)
    {
        V_STRING error_message = { "HDF-file open ERROR:", "", "" };
        // Create the file
        STRING hdf_action = action;
        if (hdf_action.empty())
        {
            if (status.compare("open") == 0) hdf_action = "read";
            else if (status.compare("create") == 0) hdf_action = "truncate";
        }
        unsigned int hdf_flags = 0;
        if (status.compare("open") == 0)
        {
            if (action.empty() || (action.compare("read") == 0)) hdf_flags = H5F_ACC_RDONLY;
            else if (action.compare("read-write") == 0) hdf_flags = H5F_ACC_RDWR;
            else
            {
                error_message[1] = " cannot open HDF file with status '" + status + "', and action '" + hdf_action + "'";
                StopByError(3005, &error_message);
            }
        }
        else if (status.compare("create") == 0)
        {
            if (hdf_action.compare("truncate") == 0)
            {
                hdf_flags = H5F_ACC_TRUNC;
                // delete file
                if (file_exists(file_name))
                {
                    if (auto iError = std::remove(file_name.c_str()) != 0)
                    {
                        std::cout << "Error #" << iError << "." << END_LINE;
                        std::cout << " Cannot delete file: '" + file_name + "'." << END_LINE;
                        error_message[1] = " Finally cannot open HDF-file with status '" + status + "' and action '" + hdf_action + "'.";
                        StopByError(3005, &error_message);
                    }
                }
            }
            else
            {
                error_message[1] = " Cannot open HDF-file with status '" + status + "' and action '" + hdf_action + "'.";
                StopByError(3005, &error_message);
            }
        }
        try
        {
            H5::H5File* hdf_file = new H5::H5File(file_name.c_str(), hdf_flags);
            return (hdf_file);
        }
        catch (const H5::FileIException&)
        {
            error_message[1] = " Cannot open HDF-file: '" + file_name + "'.";
            error_message[2] = " (status '" + status + "' and action '" + hdf_action + "').";
            StopByError(3005, &error_message);
            return nullptr;
        }
    }
    // HDF - file close by H5::H5File
    void HDF_file_close(H5::H5File* hdf_file)
    {
        STRING file_name = hdf_file->getFileName();
        try
        {
            hdf_file->close();
            PTR_DELETE(hdf_file);
        }
        catch (const H5::FileIException&)
        {
            V_STRING error_message = { "HDF-file open ERROR:", "", "" };
            error_message[1] = "ERROR in HDF-file close. Cannot close HDF-file: '" + file_name + "'.";
            StopByError(3005, &error_message);
        }
    }

#else

    // Open or create HDF file by hid_t
    // status - open or create
    // available actions - for open(read, read - write), for create(truncate)
    hid_t HDF_file_open(const STRING file_name, const STRING status, const STRING action)
    {
        V_STRING error_message = { "HDF-file open ERROR:", "", "" };
        // Create/Open the file
        STRING hdf_action = action;
        if (hdf_action.empty())
        {
            if (status.compare("open") == 0) hdf_action = "read";
            else if (status.compare("create") == 0) hdf_action = "truncate";
        }
        hid_t hdf_file = 0;
        unsigned int hdf_flags = 0;
        if (status.compare("open") == 0)
        {
            if (hdf_action.compare("read") == 0) hdf_flags = H5F_ACC_RDONLY;
            else if (hdf_action.compare("read-write") == 0) hdf_flags = H5F_ACC_RDWR;
            else
            {
                error_message[1] = " cannot open HDF file with status '" + status + "' and action '" + action + "'";
                StopByError(3005, &error_message);
            }
            hdf_file = H5Fopen(file_name.c_str(), hdf_flags, H5P_DEFAULT);
        }
        else if (status.compare("create") == 0)
        {
            if (hdf_action.compare("truncate") == 0)
            {
                hdf_flags = H5F_ACC_TRUNC;
                //// delete file
                //if (file_exists(file_name))
                //{
                //    if (auto iError = std::remove(file_name.c_str()) != 0)
                //    {
                //        std::cout << "Error #" << iError << "." << END_LINE;
                //        std::cout << " Cannot delete file: '" + file_name + "'." << END_LINE;
                //        error_message[1] = " Finally cannot open HDF-file with status '" + status + "' and action '" + hdf_action + "'.";
                //        StopByError(3005, &error_message);
                //    }
                //}
                hdf_file = H5Fcreate(file_name.c_str(), hdf_flags, H5P_DEFAULT, H5P_DEFAULT);
            }
            else
            {
                error_message[1] = " Cannot open HDF-file with status '" + status + "' and action '" + hdf_action + "'.";
                StopByError(3005, &error_message);
            }
        }
        if (hdf_file < 0)
        {
            error_message[1] = " Cannot open HDF-file: '" + file_name + "'.";
            error_message[2] = " (status '" + status + "' and action '" + hdf_action + "').";
            StopByError(3005, &error_message);
        }
        return (hdf_file);
    }

    // Open or create HDF group by hid_t
    // status - open or create
    // available actions - for open(read, read - write), for create(truncate)
    hid_t HDF_group_open(const hid_t parent_id, const STRING group_name, const STRING status, V_STRING* p_error_message)
    {
        V_STRING error_message = { "HDF-file open ERROR:", "", "" };
        // Create/Open the group
        hid_t hdf_group = 0;
        unsigned int hdf_flags = 0;
        if (status.compare("open") == 0)
        {
            if (HDF_Group_Exists(parent_id, group_name.c_str()))
                hdf_group = H5Gopen(parent_id, group_name.c_str(), H5P_DEFAULT);
            else
            {
                hdf_group = -1;
                error_message.push_back(" Group doesn't exist.");
            }
        }
        else if (status.compare("create") == 0)
        {
            //hid_t lcpl_id = H5Pcreate(H5P_LINK_CREATE);     // create group creation property list
            //H5Pset_create_intermediate_group(lcpl_id, 1);   // set intermediate link creation
            hdf_group = H5Gcreate(parent_id, group_name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }
        else
        {
            error_message[1] = " Cannot open HDF-group with unknown status '" + status + "'.";
            //StopByError(3005, &error_message);
        }
        if (p_error_message && (hdf_group < 0))
        {
            error_message[1] = " Cannot " + status + " HDF-group: '" + group_name + "'.";
            error_message[2] = " (status '" + status + "').";
            //StopByError(3005, &error_message);
            *p_error_message = error_message;
        }
        return hdf_group;
    }

    // HDF file close by hid_t
    void HDF_file_close(hid_t hdf_unit, const STRING file_name)
    {
        herr_t iError, iError1, iError2;
        iError1 = H5Fflush(hdf_unit, H5F_SCOPE_GLOBAL);
        if (iError1 < 0)
        {
            std::cout << "Error #" << iError1 << END_LINE;
            std::cout << " Cannot flush HDF-file: '" + file_name + "'." << END_LINE;
        }
        iError2 = H5garbage_collect();  // close first or after collect ?
        iError = H5Fclose(hdf_unit);
        if (iError2 < 0)
        {
            std::cout << "Error #" << iError2;
            std::cout << " Cannot garbage collect HDF-file: '" + ENV::trim(file_name) + "'.";
        }
        if (iError < 0)
        {
            V_STRING error_message = { "HDF-file close ERROR:", "", "" };
            if (file_name.empty())
            {
                error_message[1] = "ERROR in HDF-file close. Cannot close HDF-file.";
                StopByError(3005, &error_message);
            }
            else
            {
                error_message[1] = "ERROR in HDF-file close. Cannot close HDF-file: '" + file_name + "'.";
                StopByError(3005, &error_message);
            }
        }
    }

    // HDF group close by hid_t
    void HDF_group_close(hid_t group, const STRING group_name)
    {
        herr_t iError = H5Gclose(group);
        if (iError < 0)
        {
            V_STRING error_message = { "HDF-group close ERROR:", "", "" };
            if (group_name.empty())
            {
                error_message[1] = "ERROR in HDF-file close. Cannot close HDF-group.";
                StopByError(3005, &error_message);
            }
            else
            {
                error_message[1] = "ERROR in HDF-file close. Cannot close HDF-group: '" + group_name + "'.";
                StopByError(3005, &error_message);
            }
        }
    }

#endif

    // HDF5 initialization
    herr_t HDF_Init()
    {
        herr_t iError = HDF5_IS_INITIALIZED;
        if (HDF5_IS_INITIALIZED) return iError;
        HDF5_IS_INITIALIZED = false;
#ifdef H5Cpp_H
        iError = H5open();
        if (iError < 0) return iError;
        //H5::Exception::dontPrint();
#else
        HDF5_TYPE_V_STRING = 0;
        // Create string type for the Name attribute.
        is_HDF5_TYPE_NAME
            // Create string type for the String attribute.
            is_HDF5_TYPE_STRING
#endif
            HDF5_IS_INITIALIZED = true;
        return iError;
    }

    // HDF5 finalization
    herr_t HDF_Finalize()
    {
        herr_t iError = HDF5_IS_INITIALIZED;
        if (!HDF5_IS_INITIALIZED) return iError;
#ifdef H5Cpp_H
        iError = H5close();
        if (iError < 0) return iError;
#endif
        HDF5_IS_INITIALIZED = false;
        iError = H5Tclose(HDF5_TYPE_NAME) & H5Tclose(HDF5_TYPE_STRING);
        return iError;
    }

    // HDF5 initialization for FORTRAN
    extern "C" void c_HDF_init() { HDF_Init(); };

    // HDF5 finalization for FORTRAN
    extern "C" void c_HDF_finalize() { HDF_Finalize(); };

    // HDF Group Exists check by hid_t
    bool HDF_Group_Exists(const hid_t parent_id, const char* group)
    {
        return (H5Lexists(parent_id, group, H5P_DEFAULT) > 0);
    }

    // HDF Group Exists check by hid_t for FORTRAN
    extern "C" void isGroupExists(hid_t * parent_id, const char group[], int* iError)
    {
        *iError = HDF_Group_Exists(*parent_id, group);
    }

    // Open Group anyway even if it's existed already or not
    // parent_id - Parent Group identifier as hid_t
    hid_t HDF_Group_Open(const hid_t parent_id, const STRING group, herr_t& iError)
    {
        return (HDF_Group_Exists(parent_id, group.c_str())) ? H5Gopen(parent_id, group.c_str(), H5P_DEFAULT) : H5Gcreate(parent_id, group.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

#ifdef H5Cpp_H

    // Reading HDF5 attributes based on given vectors of names.
    // parent_id - Parent Group identifier as H5::Group
    LOGICAL HDF_HeaderReadNames(const H5::Group& parent_id, const STRING sHeader, V_STRING sHeadName, V_STRING& cHead, int* pError)
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
                if (pError) *pError = -1;
                return -1;
            }
        }
        else group_id = parent_id;
        // check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead))
        {
            if (pError) *pError = -2;
            return -2;
        }
        // cycle by attribute
        LOGICAL iError = 0;
        for (auto id = 0; id < iSizeOfHead; ++id)
        {
            if (!group_id.attrExists(sHeadName[id]))
            {
                iError = -3;
                break;
            }
            try
            {
                // Open the Header attribute
                H5::Attribute attr_id = group_id.openAttribute(sHeadName[id].c_str());   // Attribute identifier
                STRING a_ptr;     // Attribute Data Pointer
                // Read the Header attribute
                H5::StrType st_id = attr_id.getStrType();
                STRING s_ptr;   // Attribute String Data Pointer
                attr_id.read(st_id, s_ptr);
                cHead[id] = s_ptr;
                // Save attribute to Header
                // Close and release resources
                attr_id.close();
            }
            catch (const H5::AttributeIException&)
            {
                iError = -4;
                break;
            }
        }
        try
        {
            if (!sHeader.empty()) group_id.close();
        }
        catch (const H5::GroupIException&)
        {
            iError = -5;
        }
        if (pError) *pError = iError;
        return iError;
    }

    // Writing HDF5 attributes based on given vectors of names.
    // parent_id - Parent Group identifier as H5::Group
    LOGICAL HDF_HeaderWriteNames(H5::Group* parent_id, const STRING sHeader, V_STRING sHeadName, V_STRING cHead, int* pError)
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
                if (pError) *pError = -1;
                return -1;
            }
        }
        //  check for dimensions
        size_t iSizeOfHead = cHead.size();
        if (!((iSizeOfHead == sHeadName.size()) && iSizeOfHead))
        {
            if (pError) *pError = -2;
            return -2;
        }
        // Create the Header dataset space.Set the size to be the current size.
        H5::DataSpace dspace_id(H5S_SCALAR);    // Dataspace identifier
        // cycle by attribute
        LOGICAL iError = 0;
        for (auto id = 0; id < iSizeOfHead; ++id)
        {
            try
            {
                // Open the Header attribute with Attribute identifier
                H5::Attribute attr_id = group_id.createAttribute(sHeadName[id].c_str(), H5_STRING, dspace_id);
                // Save Header to 1 attribute by type
                // Write the Header attribute
                attr_id.write(H5_STRING, cHead[id]);
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
        if (pError) *pError = iError;
        return iError;
    }

    // HDF Reader of String Vector by H5::Group
    LOGICAL HDF_Read_Strings(V_STRING& idata, const H5::Group& parent_id, const STRING group, const bool compressed)
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
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_STRING };
        STRING ArrayType = ARRAY_TYPE_STRING;
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
            STRING array_type = STRING_EMPTY;
            attr_id.read(h5type[1], array_type);
            attr_id.close();
            if (ArrayType.compare(array_type) != 0)
            {
                if (array_type.compare(ARRAY_TYPE_CHARNAME) == 0)
                    ArrayType = ARRAY_TYPE_CHARNAME;
                else return -2;
            }
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
                char** hdf_buffer = new char* [length];
                dset_id.read(hdf_buffer, h5type[2], dset_id.getSpace());
                for (auto i = ENV_0_I64; i < length; ++i)
                {
                    idata.push_back(hdf_buffer[i]);
                    PTR_DELETE(hdf_buffer[i], []);
                }
                PTR_DELETE(hdf_buffer, []);
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
    LOGICAL HDF_Write_Strings(V_STRING idata, const H5::Group& parent_id, const STRING group, const bool compressed)
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
        VECTOR<H5::DataType> h5type{ H5_INT64, H5_STRING, H5_STRING };
        STRING ArrayType = ARRAY_TYPE_STRING;
        //
        INT64 length = idata.size();
        try
        {
            // 1) Write Length
            H5::Attribute attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[0].c_str(), h5type[0], H5S_SCALAR);	// Attribute identifier
            attr_id.write(h5type[0], &length);
            attr_id.close();
            // 2) Write type
            attr_id = group_id.createAttribute(ARRAY_DATA_NAMES[1].c_str(), h5type[1], H5S_SCALAR);
            attr_id.write(h5type[1], ArrayType);
            attr_id.close();
        }
        catch (H5::Exception& err)
        {
            throw std::runtime_error(STRING("HDF5 Error in ")
                + err.getFuncName()
                + ": "
                + err.getDetailMsg());
            return -2;
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
                // create the variable with length and type of array of char*
                char** hdf_buffer = new char* [length];
                // grab pointers to v_chars
                for (auto i = ENV_0_I64; i < length; ++i)
                {
                    auto lsize = idata[i].length() + 1;
                    hdf_buffer[i] = new char[lsize];
                    std::memcpy(hdf_buffer[i], idata[i].data(), lsize);
                }
                // write the output to a dataset
                dset_id.write(hdf_buffer, h5type[2], dset_id.getSpace());
                for (int i = ENV_0_I64; i < length; ++i)
                {
                    idata.push_back(hdf_buffer[i]);
                    PTR_DELETE(hdf_buffer[i], []);
                }
                PTR_DELETE(hdf_buffer, []);
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
            return -3;
        }
        return 0;
    }
#endif

    // HDF Reader of String Vector by hid_t
    LOGICAL HDF_Read_Strings(V_STRING& idata, const hid_t parent_id, const STRING group, const bool compressed)
    {
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group);
            if (group_id < 0) return -1;
        };
        //
        STRING ArrayType = ARRAY_TYPE_STRING;
        //
        // 1) Read Length
        hid_t attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[0].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0) return -2;
        INT64 length = ENV_0_I64;
        if (H5Aread(attr_id, H5Tget_native_type(H5Aget_type(attr_id), H5T_DIR_ASCEND), &length) < 0) return -3;
        if (H5Aclose(attr_id) < 0) return -4;
        // 2) Read type
        attr_id = H5Aopen(group_id, ARRAY_DATA_NAMES[1].c_str(), H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -5;
        char array_type[HDF5_NAME_LENGTH + 1]{};
        auto attr_type = H5Aget_type(attr_id);
        herr_t ierror = (H5Tget_class(attr_type) == H5T_STRING) ? H5Aread(attr_id, H5Tget_native_type(attr_type, H5T_DIR_ASCEND), &array_type) : -6;
        H5Tclose(attr_type);
        if (ierror < 0) return -6;
        if (H5Aclose(attr_id) < 0) return -7;
        if (ArrayType.compare(ENV::trim(array_type)) != 0)
        {
            if (ARRAY_TYPE_CHARNAME.compare(ENV::trim(array_type)) == 0)
                ArrayType = ARRAY_TYPE_CHARNAME;
            else return -8;
        }
        // 3) Read array
        idata.clear();
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);	// Dataspace identifier for array
            if (dspace_id < 0) return -9;
            // Dataset identifier for array
            hid_t dset_id = (compressed && (length > COMPRESSION_START_LENGTH)) // Compression check
                ? H5Dopen2(group_id, ARRAY_DATA_NAMES[3].c_str(), H5P_DEFAULT)      // compressed data
                : H5Dopen2(group_id, ARRAY_DATA_NAMES[2].c_str(), H5P_DEFAULT);     // uncompressed data
            if (dset_id < 0) return -10;
            auto lsize = H5Dget_storage_size(dset_id);
            char* hdf_buffer = new char [length * lsize] {};
            char* buffer = new char [lsize + 1] {};
            herr_t ierror = H5Dread(dset_id, H5Dget_type(dset_id), H5Dget_space(dset_id), H5S_ALL, H5P_DEFAULT, hdf_buffer);
            lsize /= length;
            idata.reserve(length);
            for (auto i = ENV_0_I64; i < length; ++i)
            {
                std::memcpy(buffer, hdf_buffer + i * lsize, lsize);
                idata.emplace_back(trim(buffer));
            }
            PTR_DELETE(hdf_buffer, []);
            PTR_DELETE(buffer, []);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -11;
        }
        if (!group.empty()) HDF_group_close(group_id, group);
        return 0;
    }

    // HDF Write of StringVector by hid_t
    LOGICAL HDF_Write_Strings(V_STRING& idata, const hid_t parent_id, const STRING group, const bool isName, const bool compressed)
    {
        hid_t group_id; // Group identifier
        if (group.empty()) group_id = parent_id;
        else
        {
            group_id = HDF_group_open(parent_id, group, "create");
            if (group_id < 0) return -1;
        };
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, HDF5_TYPE_NAME, HDF5_TYPE_NAME };
        STRING ArrayType = (isName) ? ARRAY_TYPE_CHARNAME : ARRAY_TYPE_STRING;
        //
        // Create the Header dataset space.Set the size to be the current size.
        HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);
        // 1) Write Length
        hid_t attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[0].c_str(), h5type[0], dspace_id, H5P_DEFAULT, H5P_DEFAULT);	// Attribute identifier
        if (attr_id < 0) return -3;
        INT64 length = idata.size();
        if (H5Awrite(attr_id, h5type[0], &length) < 0) return -4;
        if (H5Aclose(attr_id) < 0) return -5;
        // 2) Write type
        attr_id = H5Acreate(group_id, ARRAY_DATA_NAMES[1].c_str(), h5type[1], dspace_id, H5P_DEFAULT, H5P_DEFAULT);	// Attribute identifier
        if (attr_id < 0) return -6;
        if (H5Awrite(attr_id, h5type[1], ArrayType.c_str()) < 0) return -7;
        if (H5Aclose(attr_id) < 0) return -8;
        H5Sclose(dspace_id);
        // 3) Write array
        if (length > ENV_0_I64)
        {
            HDF5_DIM1[0] = length;
            hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);   // Dataspace identifier for array
            if (dspace_id < 0) return -9;
            // set size of the type of array of char*
            auto lsize = HDF5_NAME_LENGTH;
            if (max_line_length(idata) > HDF5_NAME_LENGTH)
            {
                if (isName) return -14;
                lsize = HDF5_STRING_LENGTH;
                h5type[2] = HDF5_TYPE_STRING;
            }
            hid_t dset_id = 0;   // Dataset identifier for array
            hid_t plist_id = 0;  // Properties identifier = plist(compression)
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
            // create the variable with length and type of array of char*
            char* hdf_buffer = new char[length * lsize] {};
            // grab pointers to v_chars
            for (auto i = ENV_0_I64; i < length; ++i)
            {
                //auto lsize = idata[i].length() + 1;
                //hdf_buffer[i] = new char[lsize] {};
                std::memcpy(hdf_buffer + i * lsize, idata[i].data(), idata[i].length());
            }
            // write the output to a dataset
            herr_t ierror = H5Dwrite(dset_id, h5type[2], H5Dget_space(dset_id), H5S_ALL, H5P_DEFAULT, hdf_buffer);
            PTR_DELETE(hdf_buffer, []);
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

    // Reading HDF5 attributes based on given vectors of names of type STRING.
    // parent_id - Parent Group identifier as hid_t
    LOGICAL HDF_Read_Attributes_String(V_STRING& cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader)
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
        // Check string type for the Name attribute.
        is_HDF5_TYPE_NAME;
        // Check string type for the String attribute.
        is_HDF5_TYPE_STRING;
        // cycle by attribute
        LOGICAL iError = 0;
        for (size_t id = 0; id < iSizeOfHead; ++id)
        {
            // Open the Header attribute
            hid_t attr_id = H5Aopen(group_id, (sHeadName)[id].c_str(), H5P_DEFAULT); // Attribute identifier
            if (attr_id < 0)
            {
                iError = -4;
                break;
            }
            STRING a_ptr;     // Attribute Data Pointer
            auto attr_type = H5Aget_type(attr_id);
            herr_t ierror = 0;
            if (H5Tget_class(attr_type) == H5T_STRING)
            {
                char array_type[HDF5_STRING_LENGTH + 1]{};
                // Read the Header attribute
                iError = H5Aread(attr_id, H5Tget_native_type(attr_type, H5T_DIR_ASCEND), &array_type);
                a_ptr = array_type;
            }
            else
                iError = -5;
            H5Tclose(attr_type);
            if (iError < 0) break;
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

    // Writing HDF5 attributes based on given vectors of names of type STRING.
    // parent_id - Parent Group identifier as hid_t
    LOGICAL HDF_Write_Attributes_String(V_STRING cHead, V_STRING sHeadName, const hid_t parent_id, const STRING sHeader)
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
        //
        // Create the Header dataset space.Set the size to be the current size.
        HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, HDF5_DIM1, NULL);
        // cycle by attribute
        LOGICAL iError = 0;
        for (size_t id = 0; id < iSizeOfHead; ++id)
        {
            // Save Header to 1 attribute by type
            STRING a_ptr = cHead[id];     // Attribute Data Pointer
            hid_t dtype_id = (a_ptr.length() > HDF5_NAME_LENGTH) ? HDF5_TYPE_STRING : HDF5_TYPE_NAME;
            // Open the Header attribute with Attribute identifier
            hid_t attr_id = H5Acreate(group_id, sHeadName[id].c_str(), dtype_id, dspace_id, H5P_DEFAULT, H5P_DEFAULT);
            if (attr_id < 0)
            {
                iError = -4;
                break;
            }
            // Write the Header attribute
            if (H5Awrite(attr_id, dtype_id, a_ptr.data()) < 0)
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

} // namespace ENV_H5

#endif

