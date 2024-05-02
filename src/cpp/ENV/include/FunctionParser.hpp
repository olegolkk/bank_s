/*
//--------------------------------------------------------------------------------
// C++ function parser v1.0
//--------------------------------------------------------------------------------
//
// This function parser module is intended for applications where a set of mathematical
// C-style expressions is specified at runtime and is then evaluated for a large
// number of variable values. This is done by compiling the set of function strings
// into byte code, which is interpreted efficiently for the various variable values.
//
//--------------------------------------------------------------------------------
*/
#pragma once

#ifndef ENV_FunctionParser_HPP
#define ENV_FunctionParser_HPP

#include "FunctionParserType.hpp"
#include "ENV_types.hpp"

namespace ENV_FP
{
    // private variables
    namespace
    {
        V_STRING FP_HDF_DATA_NAMES =
        {
        "Function Parser",
        "Count of Computed Formulas",
        "Associates function strings",
        "Computed Formulas"
        };
        V_STRING VAR_DATA_NAMES =
        {
        "t", "x", "y", "z"
        };
    }
    // C++ Function Parser for many functions
    class FunctionParser
    {
    public:
        int iError = 0;     // =0: no error occured, >0: evaluation error
        INT32 Length = 0;  // Size of Byte Code Formulas
        VECTOR<FunctionParserType*>* Comp = nullptr;    // Byte Code Formulas
        // name of i-th function
        V_STRING getName(const int i);
        // Evaluator of i-th function
        forceinline REAL64 evaluate(const int i, const REAL64* Val)
        {
            return (*Comp)[i]->evalf(Val, &iError);
        }

        // Add compiled byte to ByteCode of ith function parser
        void fpAddCompiledByte(const int i, const INT08 b);
        // Adder of parser with new empty single formula
        void add();
        // Compile i-th function string F into ByteCode
        void fpCompile(FunctionParserType* ci, const STRING& F, V_STRING& Var);
        // Remove Spaces and all two-char commands from string, remember positions of characters in old string
        bool fpRemoveSpaces(STRING& sstr);
        // Parser of i-th function string FuncStr and compile it into ByteCode
        void parse(const int i, const STRING& FuncStr, V_STRING& Var, const STRING FunctionKey = STRING_EMPTY);
        // Reader of parser with formulas from String Vector
        void readText(const V_STRING& StrVector, const STRING& KeyName = STRING_EMPTY);
        // Reader of parser with formulas from Text File
        void readText(const STRING& FileName, const STRING& KeyName = STRING_EMPTY);
        // Writer of parser with formulas from String Vector
        void writeText(const STRING& FileName, const STRING& KeyName = STRING_EMPTY);
        // Public Nullifier of function parser for n functions
        void clear();
        // Get error message (Use only when iError > 0)
        const STRING errorMessage();
        // Print Error Message and terminate program
        void printError(const int j, const STRING& FuncStr, const STRING& Msg = STRING_EMPTY);
        // Initialize function parser for nFunc functions (no reading from file)
        void init(const int nFunc = 0);
        // Initialize function parser for functions (reading from FileName with key KeyName)
#ifdef ENV_HDF5_HPP
#ifdef H5Cpp_H
        // Write single formula to HDF
        void fwriteHDF(H5::Group* parent_id, const STRING* group = nullptr);
        // Read single formula from HDF
        void freadHDF(const H5::Group* parent_id, const STRING* group = nullptr);
#else
        // Write single formula to HDF
        void fwriteHDF(const hid_t parent_id, const STRING* group = nullptr);
        // Read single formula from HDF
        void freadHDF(const hid_t parent_id, const STRING* group = nullptr);
#endif
#endif
        FunctionParser(const STRING& FileName = STRING_EMPTY, const STRING& KeyName = STRING_EMPTY);
        // Destruct function parser for n functions
        ~FunctionParser();
    private:
        VECTOR<int> ipos;    // Associates function strings
        /*
        // add          =  fpAddFunction
        // enlarge      =  fpEnlarge        ! Enlarger of parser with new single formula
        // freadText    =  fpTextRead
        // fwriteHDF    =  fpHDFWrite       ! Writer of parser with n formulas to HDF
        // freadHDF     =  fpHDFRead        ! Reader of parser with n formulas from HDF
        // errorMessage =  fpGetErrorMessage
        // printError   =  fpPrintErrorMessage  ! Printer of Error Message
        // evaluate     =  fpEvaluateFunction   ! Evaluator of i-th function
        // parse        =  fpParseFunction
        // isEmpty      =  fpIsEmpty        ! Public Nullify Checker of function parser for n functions
        // clear        =  fpEmpty
        // init         =  FunctionParser
        // clear        =  fpDestroy        ! Public Destructor of function parser for n functions
        // final        =  ~FunctionParser  ! Private Destructor of function parser for n functions
        */
    };
}
#endif
