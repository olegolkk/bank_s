#pragma once

#ifndef ENV_FunctionParserType_HPP
#define ENV_FunctionParserType_HPP

#include "ENV_types.hpp"
#include "ENV_HDF5.hpp"

namespace ENV_FP
{
    //--------------------------------------------------------------------------------
    // private variables
    namespace
    {
        const INT08
            cImmed = INT8_C(0),
            cNeg = INT8_C(1),
            cLogicalOr = INT8_C(2),
            cLogicalAnd = INT8_C(3),
            cEqual = INT8_C(4),
            cNotEqual = INT8_C(5),
            cLowerEqual = INT8_C(6),
            cBiggerEqual = INT8_C(7),
            cLower = INT8_C(8),
            cBigger = INT8_C(9),
            cNot = INT8_C(10),
            cAdd = INT8_C(11),
            cSub = INT8_C(12),
            cMul = INT8_C(13),
            cDiv = INT8_C(14),
            cPow = INT8_C(15),
            cAbs = INT8_C(16),
            cSign = INT8_C(17),
            cExp = INT8_C(18),
            cLog10 = INT8_C(19),
            cLog = INT8_C(20),
            cSqrt = INT8_C(21),
            cPi = INT8_C(22),
            cSind = INT8_C(23),
            cCosd = INT8_C(24),
            cTand = INT8_C(25),
            cSinh = INT8_C(26),
            cCosh = INT8_C(27),
            cTanh = INT8_C(28),
            cSin = INT8_C(29),
            cCos = INT8_C(30),
            cTan = INT8_C(31),
            cAsin = INT8_C(32),
            cAcos = INT8_C(33),
            cAtan = INT8_C(34),
            VarBegin = INT8_C(35),
            Num_LogicalOr_Pow = cPow - cLogicalOr + ENV_1_I08,
            Num_Abs_Atan = cAtan - cAbs + ENV_1_I08;
        //--------------------------------------------------------------------------------
        const STRING
            OpsAll = "|&=~[]<>!+-*/^",      // length = 14 : [cPow - cLogicalOr + 2]
            OpsAllWideP9 = OpsAll + ")",    // length = 15
            OpsAllWideP6 = OpsAll + "(",    // length = 15
            OpsUnary = "!+-",               // length = 3
            FloatDigits = "0123456789.",    // length = 11
            ExpBaseChar = "eEdD",           // length = 4
            strAlpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",  // length = 52
            OpsArithmetic = ENV::s_slice(OpsAll, cMul, cPow),
            OpsLogic = ENV::s_slice(OpsAll, cLogicalOr, cBigger);
        //--------------------------------------------------------------------------------
        V_STRING Funcs = // [cAbs:cAtan]
        {
        "abs",
        "sign",
        "exp",
        "log10",
        "log",
        "sqrt",
        "pi",
        "sind",
        "cosd",
        "tand",
        "sinh",
        "cosh",
        "tanh",
        "sin",
        "cos",
        "tan",
        "asin",
        "acos",
        "atan"
        };
        //--------------------------------------------------------------------------------
        V_STRING FPT_HDF_DATA_NAMES =
        {
        "FORMULA",
        "Byte Code Size",
        "Immed Size",
        "Stack Size",
        "Stack Pointer",
        "Byte Code Array",
        "Immed Array",
        "Stack Array",
        "Formula Name",
        "Temperature Range"
        };
        //--------------------------------------------------------------------------------
        static V_STRING error_message{ "Function Parser Error: ", "", "", "" };
    }
    // Print error message and terminate program
    void PrintErrorMessage(const size_t j, const STRING& FuncStr, const STRING& Msg = STRING_EMPTY);
    // Check syntax of function string, returns 0 if syntax is OK
    void CheckSyntax(VECTOR<int>& ipos, const STRING& Func, const STRING& FuncStr, V_STRING& Var);
    // Function Parser Type for single function
    class FunctionParserType
    {
    public:
        V_STRING Name = { "", "" };    // two strings
        INT64   ByteCodeSize = ENV_0_I64, ImmedSize = ENV_0_I64,
                StackSize = ENV_0_I64, StackPtr = ENV_0_I64;
        INT08* ByteCode = nullptr;
        REAL64* Immed = nullptr;
        REAL64* Stack = nullptr;
        REAL64* tRange = nullptr;
        // Initialize arrays of function parser type for single function
        bool init();
        // Read single formula from String Vector
        STRING readText(const V_STRING& StrVector, const STRING& Group = STRING_EMPTY);
        // Read single formula from Text File
        STRING readText(FSTREAM& FileOfInput, const STRING& Group = STRING_EMPTY);
        // Print error message
        // void PrintErrorMessage(size_t const j, const STRING FuncStr, STRING Msg = "");
#ifdef ENV_HDF5_HPP
#ifdef H5Cpp_H
        // Write single formula to HDF
        void fwriteHDF(H5::Group* parent_id, const STRING& group = STRING_EMPTY);
        // Read single formula from HDF
        void freadHDF(const H5::Group& parent_id, const STRING& group = STRING_EMPTY);
#else
        // Write single formula to HDF
        void fwriteHDF(const hid_t parent_id, const STRING& group = STRING_EMPTY);
        // Read single formula from HDF
        void freadHDF(const hid_t parent_id, const STRING& group = STRING_EMPTY);
#endif
#endif
        // Evaluate single function
        forceinline  // REAL64 evalf(const REAL64* Val, int* iErrorOut = nullptr);
        REAL64 evalf(const REAL64* Val, int* iErrorOut = nullptr)
        {
            REAL64 res = 0.0;
            int iError = 0;
            int DP = 0;     // Data pointer
            int SP = -1;    // Stack pointer
            //std::cout << STRING_LINE << END_LINE;
            //std::cout << StackSize << " : " << END_LINE;
            for (auto IP = ENV_0_I32; IP < ByteCodeSize; ++IP)   // IP - Instruction pointer
            {
                switch (ByteCode[IP])
                {
                case cImmed:
                    ++SP;
                    Stack[SP] = Immed[DP];
                    ++DP;
                    break;
                case cLogicalOr:
                    Stack[SP - 1] = (bool)Stack[SP - 1] || (bool)Stack[SP];
                    --SP;
                    break;
                case (cLogicalAnd):
                    Stack[SP - 1] = (bool)Stack[SP - 1] && (bool)Stack[SP];
                    --SP;
                    break;
                case (cEqual):
                    Stack[SP - 1] = (Stack[SP - 1] == Stack[SP]);
                    --SP;
                    break;
                case (cNotEqual):
                    Stack[SP - 1] = (Stack[SP - 1] != Stack[SP]);
                    --SP;
                    break;
                case (cLowerEqual):
                    Stack[SP - 1] = (Stack[SP - 1] <= Stack[SP]);
                    --SP;
                    break;
                case (cBiggerEqual):
                    Stack[SP - 1] = (Stack[SP - 1] >= Stack[SP]);
                    --SP;
                    break;
                case (cLower):
                    Stack[SP - 1] = (Stack[SP - 1] < Stack[SP]);
                    --SP;
                    break;
                case (cBigger):
                    Stack[SP - 1] = (Stack[SP - 1] > Stack[SP]);
                    --SP;
                    break;
                case (cNot):
                    Stack[SP] = !(bool)Stack[SP];
                    break;
                case (cNeg):
                    Stack[SP] = -Stack[SP];
                    break;
                case (cAdd):
                    Stack[SP - 1] += Stack[SP];
                    --SP;
                    break;
                case (cSub):
                    Stack[SP - 1] -= Stack[SP];
                    --SP;
                    break;
                case (cMul):
                    Stack[SP - 1] *= Stack[SP];
                    --SP;
                    break;
                case (cDiv):
                    if (Stack[SP] == 0.0)
                    {
                        iError = 1;
                        if (iErrorOut) *iErrorOut = iError;
                    }
                    Stack[SP - 1] /= Stack[SP];
                    --SP;
                    break;
                case (cPow):
                    Stack[SP - 1] = pow(Stack[SP - 1], Stack[SP]);
                    --SP;
                    break;
                case (cAbs):
                    Stack[SP] = abs(Stack[SP]);
                    break;
                case (cSign):
                    if (Stack[SP] != 0.0) Stack[SP] = ENV::signum(Stack[SP]);
                    break;
                case (cExp):
                    Stack[SP] = exp(Stack[SP]);
                    break;
                case (cLog10):
                    if (Stack[SP] <= 0.0)
                    {
                        iError = 2;
                        if (iErrorOut) *iErrorOut = iError;
                        return 0.0;
                    }
                    Stack[SP] = log10(Stack[SP]);
                    break;
                case (cLog):
                    if (Stack[SP] <= 0.0)
                    {
                        iError = 3;
                        if (iErrorOut) *iErrorOut = iError;
                        return 0.0;
                    }
                    Stack[SP] = log(Stack[SP]);
                    break;
                case (cPi):
                    if (Stack[SP] > 0.0)
                    {
                        Stack[SP] = ENV::PI_R64 / Stack[SP];
                        continue;
                    }
                    if (Stack[SP] < 0.0)
                    {
                        Stack[SP] = ENV::PI_R64 * Stack[SP];
                        continue;
                    }
                    Stack[SP] = ENV::PI_R64;
                    break;
                case (cSind):
                    Stack[SP] = sin(Stack[SP] * ENV::RADIAN_R64);
                    break;
                case (cCosd):
                    Stack[SP] = cos(Stack[SP] * ENV::RADIAN_R64);
                    break;
                case (cTand):
                    Stack[SP] = tan(Stack[SP] * ENV::RADIAN_R64);
                    break;
                case (cSqrt):
                    if (Stack[SP] < 0.0)
                    {
                        iError = 4;
                        if (iErrorOut) *iErrorOut = iError;
                        return 0.0;
                    }
                    Stack[SP] = sqrt(Stack[SP]);
                    break;
                case (cSinh):
                    Stack[SP] = sinh(Stack[SP]);
                    break;
                case (cCosh):
                    Stack[SP] = cosh(Stack[SP]);
                    break;
                case (cTanh):
                    Stack[SP] = tanh(Stack[SP]);
                    break;
                case (cSin):
                    Stack[SP] = sin(Stack[SP]);
                    break;
                case (cCos):
                    Stack[SP] = cos(Stack[SP]);
                    break;
                case (cTan):
                    Stack[SP] = tan(Stack[SP]);
                    break;
                case (cAsin):
                    if ((Stack[SP] < -1.0) || (Stack[SP] > 1.0))
                    {
                        iError = 5;
                        if (iErrorOut) *iErrorOut = iError;
                        return 0.0;
                    }
                    Stack[SP] = asin(Stack[SP]);
                    break;
                case (cAcos):
                    if ((Stack[SP] < -1.0) || (Stack[SP] > 1.0))
                    {
                        iError = 6;
                        if (iErrorOut) *iErrorOut = iError;
                        return 0.0;
                    }
                    Stack[SP] = acos(Stack[SP]);
                    break;
                case (cAtan):
                    Stack[SP] = atan(Stack[SP]);
                    break;
                default:
                    ++SP;
                    Stack[SP] = Val[ByteCode[IP] - VarBegin];
                    break;
                }
                //for (auto i = 0; i < StackSize; ++i) std::cout << Stack[i] << ", ";
                //std::cout << END_LINE;
            }
            //std::cout << STRING_LINE << END_LINE;
            iError = 0;
            res = Stack[0];
            if (tRange != nullptr)
            {
                if (res < tRange[0]) res = tRange[0];
                else if (res > tRange[1]) res = tRange[1];
            }
            if (iErrorOut) *iErrorOut = iError;
            return res;
        }

        // Compile single function string F into ByteCode
        void compilef(const STRING& F, const int b, const int e, V_STRING& Var);
        // Public Clear (Empty) function parser type for single function
        void clear();
        // Constructor of function parser type for single function
        // Add compiled byte to ByteCode
        void add(const INT08 b);
        FunctionParserType();
        // Destructor of function parser type for single function
        ~FunctionParserType();
    private:
        INT32 iError = 0;   // = 0: no error occured, > 0: evaluation error
        // Return math item index, if item is real number, enter it into Comp - structure
        INT08 itemIndex(const STRING& F, V_STRING& Var);
        //
        /*
        // freadText = fptTextRead;
        // fwriteHDF = fptHDFWrite;
        // freadHDF  = fptHDFRead;
        // evalf     = fptEvaluateFunction;
        // compilef  = fptCompileSubString;
        // itemIndex = fptMathItemIndex;
        // add       = fptAddCompiledByte;
        // clear     = fptEmpty;
        // init      = FunctionParserType;
        // final     = ~FunctionParserType;
        */
    };
}
#endif
