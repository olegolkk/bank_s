#include "FunctionParserType.hpp"
#include "ENV_types.hpp"

namespace ENV_FP
{

//#pragma warning(1 : 4714)

// Return index of math function beginnig at 1st position of string str
INT08 MathFunctionIndex(const STRING s)
{
    size_t lstr = s.length();
    if (lstr < 2) return ENV_1M_I08;
    if (lstr == 2)
    {
        if (s == "pi") return cPi;
        else return ENV_1M_I08;
    }
    // Check all math functions
    for (INT08 j = 0; j < Num_Abs_Atan; ++j)
    {
        // Compare lower case letters
        if (STRING(s, 0, std::min(Funcs[j].length(), lstr)).compare(Funcs[j]) == 0)
            return (j + cAbs);   // Found a matching function
    }
    return ENV_1M_I08;
}

// Get real number from string - Format: [blanks][+|-][nnn][.nnn][e|E|d|D[+|-]nnn]
REAL64 RealNum(const STRING& s, int* ibegin = nullptr, int* inext = nullptr, bool* error = nullptr)
{
    //real(real64)::r                         ! Result : Real number
    //character(len = *), intent(in)::s       ! String
    //integer, intent(out), optional::ibegin  ! Start position of real number
    //integer, intent(out), optional::inext   ! 1st character after real number
    //logical, intent(out), optional::error   ! Error flag
    //logical::Bflag, & ! .T.at begin of number in str
    //InMan,  & ! .T.in mantissa of number
    //Pflag,  & ! .T.after 1st '.' encountered
    //Eflag,  & ! .T.at exponent identifier 'eEdD'
    //InExp,  & ! .T.in exponent of number
    //DInMan, & ! .T. if at least 1 digit in mant.
    //DInExp, & ! .T. if at least 1 digit in exp.
    //err       ! Local error flag
    REAL64 r = 0.0;
    bool
        Bflag = true,
        InMan = false, Pflag = false, Eflag = false, InExp = false,
        DInMan = false, DInExp = false;
    int ib = 0, iin = 0, ls = s.length();
    while (iin < ls)
    {
        char c = s[iin];
        //case ('0'..'9'):                              // Mark
        if (std::isdigit(c))
        {
            if (Bflag)
            {
                InMan = true; Bflag = false;            // - beginning of mantissa
            }
            else if (Eflag)
            {
                InExp = true; Eflag = false;            // - beginning of exponent
            }
            if (InMan) DInMan = true;                   // Mantissa contains digit
            if (InExp) DInExp = true;                   // Exponent contains digit
        }
        else if (c == ' ')                              // Only leading blanks permitted
        {
            ++ib;
            if (InMan || Eflag || InExp) break;
        }
        else if ((c == '+') || (c == '-'))              // Permitted only
        {
            if (Bflag)
            {
                InMan = true; Bflag = false;            // - at beginning of mantissa
            }
            else if (Eflag)
            {
                InExp = true; Eflag = false;            // - at beginning of exponent
            }
            else break;                                 // - otherwise STOP
        }
        else if (c == '.')
        {
            if (Bflag)
            {
                Pflag = true;                           // - mark 1st appearance of '.'
                InMan = true; Bflag = false;            // mark beginning of mantissa
            }
            else if (InMan && !Pflag) Pflag = true;     // - mark 1st appearance of '.'
            else break;                                 //- otherwise STOP
        }
        else if (ENV::isCharInText(c, ExpBaseChar))     // Permitted only
        {
            if (InMan)
            {
                Eflag = true; InMan = false;            // - following mantissa
            }
            else break;                                 //- otherwise STOP
        }
        else break;                                     // STOP at all other characters
        ++iin;
    }
    bool err = (ib > iin - 1) || !DInMan || ((Eflag || InExp) && !DInExp);
    if (err) r = 0.0;
    else
    {
        STRING s_tmp = ENV::s_slice(s, ib, iin - 1);
        err = ENV::StrToReal64(s_tmp, r);
    }
    if (ibegin) *ibegin = ib;
    if (inext) *inext = iin;
    if (error) *error = err;
    return r;
}

// Return index of variable at begin of string str(returns 0 if no variable found)
// n      - Result : Index of variable
// s      - String
// Var    - Array with variable names
// ibegin - Start position of variable name
// inext  - Position of character after name
INT08 VariableIndex(const STRING& s, V_STRING& Var, int* ibegin = nullptr, int* inext = nullptr)
{
    INT08 n = ENV_1M_I08;
    int lstr = s.length();
    if (lstr < 1) return n;
    int ib = 0;
    for (; ib < lstr; ++ib)                     // Search for first character in s
        if (s[ib] != ' ') break;                // When lstr > 0 at least 1 char in s
    int iin = ib;
    for (; iin < lstr; ++iin)                   // Search for name terminators
        if (ENV::isCharInText(s[iin], OpsAllWideP9)) break;
    int j = 0;
    for (auto Vj = Var.begin(); Vj != Var.end(); ++Vj, ++j)
    {
        STRING s_tmp = ENV::s_slice(s, ib, iin - 1);
        if (s_tmp == *Vj)
        {
            n = j;                              // Variable name found
            break;
        }
    }
    if (ibegin) *ibegin = ib;
    if (inext)  *inext = iin;
    return n;
}

// Return operator index
INT08 OperatorIndex(const char c)
{
    INT08 i = OpsAll.find(c);
    return ((i == STRING::npos) ? ENV_1M_I08 : (i + cLogicalOr));
}

// Check if operator F(j:j) in string F is binary operator
//  Special cases already covered elsewhere : (that is corrected in v1.1)
// - operator character F[j] is first character of string (j = 0)
// j - Position of Operator
// F - Function String
bool IsBinaryOp(const size_t j, const STRING& F)
{
    if (!ENV::isCharInText(F[j], OpsUnary)) return true;
    // Plus or minus sign or logical not:
        // - leading unary operator ?
        // or
        // - other unary operator ?
    if (j == 1) return false;
    if (ENV::isCharInText(F[j - 1], OpsAllWideP6)) return false;
    if (std::isdigit(F[j + 1]) && (ExpBaseChar.find(F[j - 1]) != STRING::npos))
    {
        // - in exponent of real number ?
        bool Dflag = false, Pflag = false;
        int k = j - 1;
        while (k > 1)    // step to the left in mantissa
        {
            if (std::isdigit(F[--k])) Dflag = true;
            else
                if (F[k] == '.')
                    if (Pflag) break;   // *EXIT: 2nd appearance of '.'
                    else Pflag = true;  // *mark 1st appearance of '.'
                else break;             // *all other characters
        }
        if (Dflag && ((k == 1) || ENV::isCharInText(F[k], OpsAllWideP6))) return false;
    }
    return true;
}

// Check if function substring F(b:e) is completely enclosed by a pair of parenthesis
// F    - Function substring
// b, e - First and last pos.of substring
bool CompletelyEnclosed(const STRING& F, const size_t b, const size_t e)
{
    if (((F[b] != '(') || (F[e] != ')'))) return false;
    int k = 0;
    for (auto j = b + 1; j < e; ++j)
    {
        if (F[j] == '(') ++k;
        else if (F[j] == ')') --k;
        if (k < 0) return false;
    }
    return (k == 0); // All opened parenthesis closed
}

// Evaluate single function
// Val - Variable values
//forceinline
//inline REAL64 FunctionParserType::evalf(const REAL64* Val, int* iErrorOut)
//{
//    REAL64 res = 0.0;
//    int iError = 0;
//    int DP = 0;     // Data pointer
//    int SP = -1;    // Stack pointer
//    //std::cout << STRING_LINE << END_LINE;
//    //std::cout << StackSize << " : " << END_LINE;
//    for (auto IP = ENV_0_I32; IP < ByteCodeSize; ++IP)   // IP - Instruction pointer
//    {
//        switch (ByteCode[IP])
//        {
//        case cImmed:
//            ++SP;
//            Stack[SP] = Immed[DP];
//            ++DP;
//            break;
//        case cLogicalOr:
//            Stack[SP - 1] = (bool)Stack[SP - 1] || (bool)Stack[SP];
//            --SP;
//            break;
//        case (cLogicalAnd):
//            Stack[SP - 1] = (bool)Stack[SP - 1] && (bool)Stack[SP];
//            --SP;
//            break;
//        case (cEqual):
//            Stack[SP - 1] = (Stack[SP - 1] == Stack[SP]);
//            --SP;
//            break;
//        case (cNotEqual):
//            Stack[SP - 1] = (Stack[SP - 1] != Stack[SP]);
//            --SP;
//            break;
//        case (cLowerEqual):
//            Stack[SP - 1] = (Stack[SP - 1] <= Stack[SP]);
//            --SP;
//            break;
//        case (cBiggerEqual):
//            Stack[SP - 1] = (Stack[SP - 1] >= Stack[SP]);
//            --SP;
//            break;
//        case (cLower):
//            Stack[SP - 1] = (Stack[SP - 1] < Stack[SP]);
//            --SP;
//            break;
//        case (cBigger):
//            Stack[SP - 1] = (Stack[SP - 1] > Stack[SP]);
//            --SP;
//            break;
//        case (cNot):
//            Stack[SP] = !(bool)Stack[SP];
//            break;
//        case (cNeg):
//            Stack[SP] = -Stack[SP];
//            break;
//        case (cAdd):
//            Stack[SP - 1] += Stack[SP];
//            --SP;
//            break;
//        case (cSub):
//            Stack[SP - 1] -= Stack[SP];
//            --SP;
//            break;
//        case (cMul):
//            Stack[SP - 1] *= Stack[SP];
//            --SP;
//            break;
//        case (cDiv):
//            if (Stack[SP] == 0.0)
//            {
//                iError = 1;
//                if (iErrorOut) *iErrorOut = iError;
//            }
//            Stack[SP - 1] /= Stack[SP];
//            --SP;
//            break;
//        case (cPow):
//            Stack[SP - 1] = pow(Stack[SP - 1], Stack[SP]);
//            --SP;
//            break;
//        case (cAbs):
//            Stack[SP] = abs(Stack[SP]);
//            break;
//        case (cSign):
//            if (Stack[SP] != 0.0) Stack[SP] = ENV::signum(Stack[SP]);
//            break;
//        case (cExp):
//            Stack[SP] = exp(Stack[SP]);
//            break;
//        case (cLog10):
//            if (Stack[SP] <= 0.0)
//            {
//                iError = 2;
//                if (iErrorOut) *iErrorOut = iError;
//                return 0.0;
//            }
//            Stack[SP] = log10(Stack[SP]);
//            break;
//        case (cLog):
//            if (Stack[SP] <= 0.0)
//            {
//                iError = 3;
//                if (iErrorOut) *iErrorOut = iError;
//                return 0.0;
//            }
//            Stack[SP] = log(Stack[SP]);
//            break;
//        case (cPi):
//            if (Stack[SP] > 0.0)
//            {
//                Stack[SP] = ENV::PI_R64 / Stack[SP];
//                continue;
//            }
//            if (Stack[SP] < 0.0)
//            {
//                Stack[SP] = ENV::PI_R64 * Stack[SP];
//                continue;
//            }
//            Stack[SP] = ENV::PI_R64;
//            break;
//        case (cSind):
//            Stack[SP] = sin(Stack[SP] * ENV::RADIAN_R64);
//            break;
//        case (cCosd):
//            Stack[SP] = cos(Stack[SP] * ENV::RADIAN_R64);
//            break;
//        case (cTand):
//            Stack[SP] = tan(Stack[SP] * ENV::RADIAN_R64);
//            break;
//        case (cSqrt):
//            if (Stack[SP] < 0.0)
//            {
//                iError = 4;
//                if (iErrorOut) *iErrorOut = iError;
//                return 0.0;
//            }
//            Stack[SP] = sqrt(Stack[SP]);
//            break;
//        case (cSinh):
//            Stack[SP] = sinh(Stack[SP]);
//            break;
//        case (cCosh):
//            Stack[SP] = cosh(Stack[SP]);
//            break;
//        case (cTanh):
//            Stack[SP] = tanh(Stack[SP]);
//            break;
//        case (cSin):
//            Stack[SP] = sin(Stack[SP]);
//            break;
//        case (cCos):
//            Stack[SP] = cos(Stack[SP]);
//            break;
//        case (cTan):
//            Stack[SP] = tan(Stack[SP]);
//            break;
//        case (cAsin):
//            if ((Stack[SP] < -1.0) || (Stack[SP] > 1.0))
//            {
//                iError = 5;
//                if (iErrorOut) *iErrorOut = iError;
//                return 0.0;
//            }
//            Stack[SP] = asin(Stack[SP]);
//            break;
//        case (cAcos):
//            if ((Stack[SP] < -1.0) || (Stack[SP] > 1.0))
//            {
//                iError = 6;
//                if (iErrorOut) *iErrorOut = iError;
//                return 0.0;
//            }
//            Stack[SP] = acos(Stack[SP]);
//            break;
//        case (cAtan):
//            Stack[SP] = atan(Stack[SP]);
//            break;
//        default:
//            ++SP;
//            Stack[SP] = Val[ByteCode[IP] - VarBegin];
//            break;
//        }
//        //for (auto i = 0; i < StackSize; ++i) std::cout << Stack[i] << ", ";
//        //std::cout << END_LINE;
//    }
//    //std::cout << STRING_LINE << END_LINE;
//    iError = 0;
//    res = Stack[0];
//    if (tRange != nullptr)
//    {
//        if (res < tRange[0]) res = tRange[0];
//        else if (res > tRange[1]) res = tRange[1];
//    }
//    if (iErrorOut) *iErrorOut = iError;
//    return res;
//}

// Compile single function string F into ByteCode
// F    - Function substring
// b, e - Begin and End position substring
// Var  - Array with variable names
void FunctionParserType::compilef(const STRING& F, const int b, const int e, V_STRING& Var)
{
    // Check for special cases of substring
    if (F[b] == '+')
    {
        // Case 1: F(b : e) = '+...'
        // std::cout << "1. F(b:e) = '+...'" << END_LINE;
        compilef(F, b + 1, e, Var);
        return;
    }
    else if (CompletelyEnclosed(F, b, e))
    {
        // Case 2 : F(b : e) = '(...)'
        // std::cout << "2. F(b:e) = '(...)'" << END_LINE;
        compilef(F, b + 1, e - 1, Var);
        return;
    }
    else if (strAlpha.find(F[b]) != STRING::npos)
    {
        STRING s_tmp = ENV::s_slice(F, b, e);
        INT08 n = MathFunctionIndex(s_tmp);
        if (n > ENV_0_I08)
        {
            STRING s_temp = ENV::s_slice(F, b, e);
            auto bp = s_temp.find('(');
            size_t b2 = b + ((bp == STRING::npos) ? 0 : bp);
            if (CompletelyEnclosed(F, b2, e))
            {
                // Case 3: F(b : e) = 'fcn(...)'
                // std::cout << "3. F(b:e) = 'fcn(...)'" << END_LINE;
                compilef(F, b2 + 1, e - 1, Var);
                add(n);
                return;
            }
        }
    }
    else if (F[b] == '-')
    {
        if (CompletelyEnclosed(F, b + 1, e))
        {
            // Case 4: F(b : e) = '-(...)'
            // std::cout << "4. F(b:e) = '-(...)'" << END_LINE;
            compilef(F, b + 2, e - 1, Var);
            add(cNeg);
            return;
        }
        else if (strAlpha.find(F[b + 1]) != STRING::npos)
        {
            STRING s_tmp = ENV::s_slice(F, b + 1, e);
            INT08 n = MathFunctionIndex(s_tmp);
            if (n > ENV_0_I08)
            {
                STRING s_temp = ENV::s_slice(F, b + 1, e);
                size_t b2 = b + s_temp.find('(');
                if (CompletelyEnclosed(F, b2, e))
                {
                    // Case 5: F(b : e) = '-fcn(...)'
                    // std::cout << "5. F(b:e) = '-fcn(...)'" << END_LINE;
                    compilef(F, b2 + 1, e - 1, Var);
                    add(n);
                    add(cNeg);
                    return;
                }
            }
        }
    }
    // Check for operator in substring : check only base level (k = 0), exclude expr.in ()
    // Increasing priority OpsAll
    for (INT08 io = ENV_0_I08; io < Num_LogicalOr_Pow; ++io)
    {
        int k = 0;
        for (auto j = e; j >= b; --j)
        {
            if (F[j] == ')') ++k;
            else if (F[j] == '(') --k;
            if ((k == 0) && (F[j] == OpsAll[io]) && IsBinaryOp(j, F))
            {
                if ((OpsArithmetic.find(F[j]) != STRING::npos) && (F[b] == '-'))
                {
                    // Case 6: F(b : e) = '-...Op...' with Op > -
                    // std::cout << "6. F(b:e) = '-...Op...' with Op > -" << END_LINE;
                    compilef(F, b + 1, e, Var);
                    add(cNeg);
                    return;
                }
                else
                {
                    if ((OpsLogic.find(F[j]) != STRING::npos) && (F[b] == '!'))
                    {
                        // Case 6: F(b : e) = '-...Op...' with Op > -
                        // std::cout << "6. F(b:e) = '-...Op...' with Op > -" << END_LINE;
                        compilef(F, b + 1, e, Var);
                        add(cNot);
                        return;
                    }
                else
                {
                    // Case 7: F(b : e) = "...BinOp..."
                    // std::cout << "7. Binary operator " << F[j] << END_LINE;
                    compilef(F, b, j - 1, Var);
                    compilef(F, j + 1, e, Var);
                    add(OperatorIndex(OpsAll[io]));
                    --StackPtr;
                    return;
                }
                }
            }
        }
    }
    // Check for remaining items, i.e.variables or explicit numbers
    auto b2 = b;
    if ((F[b] == '-') || (F[b] == '!')) ++b2;
    STRING s_tmp = ENV::s_slice(F, b2, e);
    INT08 n = itemIndex(s_tmp, Var);
    // std::cout << "8. AddCompiledByte " << n << END_LINE;
    add(n);
    if (++StackPtr > StackSize) ++StackSize;
    if (b2 > b)
    {
        if (F[b] == '-') add(cNeg);
        else add(cNot);
    }
}

// Return math item index, if item is real number, enter it into Comp - structure
// Byte value of math item
// F   - Function substring
// Var - Array with variable names
INT08 FunctionParserType::itemIndex(const STRING& F, V_STRING& Var)
{
    INT08 n = ENV_1M_I08;
    if (FloatDigits.find(F[0]) != STRING::npos)
    {
        // Check for begin of a number
        if (Immed) Immed[ImmedSize] = RealNum(F);
        ++ImmedSize;
        n = cImmed;
    }
    else
    {
        // Check for a variable
        n = VariableIndex(F, Var);
        if (n >= 0) n += VarBegin;
    }
    return n;
}

// Add compiled byte to ByteCode
void FunctionParserType::add(const INT08 b)
{
    if (ByteCode) ByteCode[ByteCodeSize] = b;
    ++ByteCodeSize;
}

// Empty function parser type for single function
void FunctionParserType::clear()
{
    PTR_DELETE(ByteCode, []);
    PTR_DELETE(Immed, []);
    PTR_DELETE(Stack, []);
    PTR_DELETE(tRange, []);
    ByteCodeSize = ImmedSize = StackSize = StackPtr = 0;
}

// Initialize arrays of function parser type for single function
bool FunctionParserType::init()
{
    try
    {
        if (ByteCodeSize) ByteCode = new INT08[ByteCodeSize]{};
        if (ImmedSize) Immed = new REAL64[ImmedSize]{};
        if (StackSize) Stack = new REAL64[StackSize]{};
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// Constructor: Initialize function parser type for single function
FunctionParserType::FunctionParserType()
{
    clear();
}

// Destructor
FunctionParserType::~FunctionParserType()
{
    clear();
}

// ***Read single formula from TEXT - file
STRING FunctionParserType::readText(FSTREAM& FileOfInput, const STRING& Group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = (Group.empty()) ? FPT_HDF_DATA_NAMES[0] : Group;
    STRING skey, stmp;
    if (GroupName.empty())
    {
        ENV_FP::error_message[1] = " readText(), cannot read from input Text File,";
        ENV_FP::error_message[2] = " Group Open Error '" + GroupName + "'.";
        ENV_FP::error_message[2] = " Group name cannot be empty.";
        ENV::StopByError(1005, &ENV_FP::error_message);
    }
    for (;;)
    {
        //FileOfInput >> stmp;
        std::getline(FileOfInput, stmp);
        if (FileOfInput.fail())
        {
            ENV_FP::error_message[1] = " readText(), cannot read from input Text File, ";
            ENV_FP::error_message[3] = " Group Open Error '" + GroupName + "'.";
            ENV::StopByError(1005, &ENV_FP::error_message);
        }
        stmp = ENV::trim(stmp);
        auto ksf = stmp.find(' ');
        skey = (ksf != STRING::npos) ? stmp.substr(0, ksf) : STRING_EMPTY;
        if (stmp.compare("FINISH") == 0) return stmp;
        if (skey.compare(GroupName) == 0) break;
    }
    clear();
    // read from Text File the formula data
    STRING sFormula = stmp.substr(skey.length());
    if (sFormula.empty())
    {
        ENV_FP::error_message[1] = " readText(), cannot read the Formula from input Text File";
        ENV_FP::error_message[2] = " GROUP='" + GroupName + "'.";
        ENV::StopByError(1005, &ENV_FP::error_message);
    }
    Name = { GroupName, sFormula };
    return sFormula;
}

// ***Read single formula from TEXT - String Vector
STRING FunctionParserType::readText(const V_STRING& StrVector, const STRING& Group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = (Group.empty()) ? FPT_HDF_DATA_NAMES[0] : Group;
    STRING skey, stmp;
    if (GroupName.empty())
    {
        error_message[1] = " readText(), cannot read from input String Vector,";
        error_message[2] = " Group Open Error '" + GroupName + "'.";
        error_message[2] = " Group name cannot be empty.";
        ENV::StopByError(1005, &error_message);
    }
    for (auto vsi = StrVector.begin(); vsi != StrVector.end(); ++vsi)
    {
        stmp = *vsi;
        stmp = ENV::trim(stmp);
        auto ksf = stmp.find(' ');
        skey = (ksf != STRING::npos) ? stmp.substr(0, ksf) : STRING_EMPTY;
        if (stmp.compare("FINISH") == 0) return stmp;
        if (skey.compare(GroupName) == 0) break;
    }
    clear();
    // read from Text File the formula data
    STRING sFormula = stmp.substr(skey.length());
    if (sFormula.empty())
    {
        error_message[1] = " readText(), cannot read the Formula from input String Vector";
        error_message[2] = " GROUP='" + GroupName + "'.";
        ENV::StopByError(1005, &error_message);
    }
    Name = { GroupName, sFormula };
    return sFormula;
}

#ifdef ENV_HDF5_HPP

#ifdef H5Cpp_H

// ***Write single formula to HDF - file
// parent_id - Parent Group identifier by H5::Group
void FunctionParserType::fwriteHDF(H5::Group* parent_id, const STRING& group)
{
    STRING GroupName = group.empty() ? FPT_HDF_DATA_NAMES[0] : group;
    // Open an existing group in the specified file group.
    H5::Group group_id; // Group identifier
    if (GroupName.empty()) group_id = *parent_id;
    else
    {
        try
        {
            group_id = H5::Group(parent_id->createGroup(GroupName));
        }
        catch (const H5::GroupIException&)
        {
            //auto fn = parent_id->getFileName();
            //auto pn = parent_id->fromClass();
            error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file";
            error_message[2] = " '" + STRING() + "'.";
            error_message[3] = " Group Create Error '" + STRING() + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    // write to the HDF-group the header data
    V_STRING vs_tmp = vslice(FPT_HDF_DATA_NAMES, 1, 4);
    if (ENV_H5::HDF_HeaderWriteSimple(&group_id, STRING_EMPTY, vs_tmp, VECTOR<INT32>({ ByteCodeSize, ImmedSize, StackSize, StackPtr })))
    {
        //auto fn = parent_id->getFileName();
        //auto pn = parent_id->fromClass();
        error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file '" + STRING() + "'.";
        error_message[2] = " GROUP='" + STRING() + GroupName + "',";
        error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
        for (size_t i = 2; i < 5; ++i)
            error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
        error_message[3] += "]";
        ENV::StopByError(3005, &error_message);
    }
    // Write ByteCode array dataset.
    if (ENV_H5::HDF_Write(ByteCode, ByteCodeSize, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[5] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Immed array dataset.
    if (ENV_H5::HDF_Write(Immed, ImmedSize, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[6] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Stack array dataset.
    if (ENV_H5::HDF_Write(Stack, StackSize, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[7] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Formula Name dataset.
    if (Name[0].empty()) Name[0] = GroupName;
    if (ENV_H5::HDF_Write_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[8] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Temperature Range array dataset.
    if (ENV_H5::HDF_Write(tRange, 2, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[9] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    try
    {
        // close last group
        if (!GroupName.empty()) group_id.close();
    }
    catch (H5::Exception&)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), Group Close Error '" + GroupName + "'.";
        ENV::StopByError(3005, &error_message);
    }
}

// ***Read single formula from HDF-file
// parent_id - Parent Group identifier by H5::Group
void FunctionParserType::freadHDF(const H5::Group& parent_id, const STRING& group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = group.empty() ? FPT_HDF_DATA_NAMES[0] : group;
    H5::Group group_id;            // Group identifier
    if (GroupName.empty()) group_id = parent_id;
    else
    {
        try
        {
            group_id = H5::Group(parent_id.openGroup(GroupName));
        }
        catch (const H5::GroupIException&)
        {
            error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file.";
            error_message[2] = " Group Open Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    clear();
    // read from the HDF-group the header data
    VECTOR<INT32> rdata(4, ENV_0_I32);
    V_STRING vs_tmp = vslice(FPT_HDF_DATA_NAMES, 1, 4);
    if (ENV_H5::HDF_HeaderReadSimple(group_id, STRING_EMPTY, vs_tmp, rdata))
    {
        error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file.";
        error_message[2] = " GROUP='" + GroupName + "',";
        error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
        for (auto i = 2; i < 5; ++i)
            error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
        error_message[3] += "]";
        ENV::StopByError(3005, &error_message);
    }
    ByteCodeSize = rdata[0];
    ImmedSize = rdata[1];
    StackSize = rdata[2];
    StackPtr = rdata[3];
    // Read ByteCode array dataset.
    if (ENV_H5::HDF_Read(ByteCode, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[5]) + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Immed array dataset.
    if (ENV_H5::HDF_Read(Immed, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[6]) + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Stack array dataset.
    if (ENV_H5::HDF_Read(Stack, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[7]) + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Formula Name dataset.
    if (ENV_H5::HDF_Read_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[8]) + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Temperature Range array dataset.
    if (ENV_H5::HDF_Read(tRange, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[9]) + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    try
    {
        // close last group
        if (!GroupName.empty()) group_id.close();
    }
    catch (H5::Exception&)
    {
        error_message[1] = " FunctionParserType::freadHDF(), Group Close Error '" + GroupName + "'.";
        ENV::StopByError(3005, &error_message);
    }
}

#else

// ***Write single formula to HDF - file
// parent_id - Parent Group identifier by hid_t
void FunctionParserType::fwriteHDF(const hid_t parent_id, const STRING& Group)
{
    STRING GroupName = (Group.empty()) ? FPT_HDF_DATA_NAMES[0] : Group;
    // Open an existing group in the specified file group.
    hid_t group_id; // Group identifier
    if (GroupName.empty()) group_id = parent_id;
    else
    {
        group_id = ENV_H5::HDF_group_open(parent_id, GroupName, "create");
        if (group_id < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file,";
            error_message[2] = " Group Create Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    // write to the HDF-group the header data
    V_STRING vs_tmp = v_slice(FPT_HDF_DATA_NAMES, 1, 4);
    if (ENV_H5::HDF_Write_Attributes(VECTOR<INT64>{ ByteCodeSize, ImmedSize, StackSize, StackPtr }, vs_tmp, group_id) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file,";
        error_message[2] = " GROUP='" + ENV::trim(GroupName) + "',";
        error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
        for (auto i = 2; i < 5; ++i)
            error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
        error_message[3] += "]";
        ENV::StopByError(3005, &error_message);
    }
    // Write ByteCode array dataset.
    if (ENV_H5::HDF_Write(ByteCode, ByteCodeSize, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[5] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Immed array dataset.
    if (ENV_H5::HDF_Write(Immed, ImmedSize, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[6] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Stack array dataset.
    std::cerr << "Writing "  << FPT_HDF_DATA_NAMES[7] << END_LINE;
    for (auto i = ENV_0_I64; (i < StackSize); ++i)
    {
        std::cerr << Stack[i] << END_LINE;
    }
    if (ENV_H5::HDF_Write(Stack, StackSize, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[7] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    // Write Formula Name dataset.
    if (Name[1].empty()) Name[1] = GroupName;
    if (ENV_H5::HDF_Write_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[8] + "' cannot be written.";
        ENV::StopByError(-3005, &error_message);
    }
    // Write Temperature Range array dataset.
    if (ENV_H5::HDF_Write(tRange, ENV_2_I64, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
    {
        error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[9] + "' cannot be written.";
        ENV::StopByError(3005, &error_message);
    }
    if (GroupName.length())
    {
        error_message[2] = " FunctionParserType::fwriteHDF(), Group Close Error '" + GroupName + "'.";
        ENV_H5::HDF_group_close(group_id, GroupName);
        error_message[2] = "";
    }
}

// ***Read single formula from HDF-file
// parent_id - Parent Group identifier by hid_t
void FunctionParserType::freadHDF(const hid_t parent_id, const STRING& Group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = (Group.empty()) ? FPT_HDF_DATA_NAMES[0] : Group;
    hid_t group_id;            // Group identifier
    if (GroupName.empty()) group_id = parent_id;
    else
    {
        group_id = ENV_H5::HDF_group_open(parent_id, GroupName);
        if (group_id < 0)
        {
            error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file,";
            error_message[2] = " Group Open Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    clear();
    // read from the HDF-group the header data
    VECTOR<INT64> rdata(4, ENV_0_I64);
    V_STRING vs_tmp = v_slice(FPT_HDF_DATA_NAMES, 1, 4);
    if (ENV_H5::HDF_Read_Attributes(rdata, vs_tmp, group_id) < 0)
    {
        error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file,";
        error_message[2] = " GROUP='" + GroupName + "',";
        error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
        for (auto i = 2; i < 5; ++i)
            error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
        error_message[3] += "]";
        ENV::StopByError(3005, &error_message);
    }
    ByteCodeSize = rdata[0];
    ImmedSize = rdata[1];
    StackSize = rdata[2];
    StackPtr = rdata[3];
    // Read ByteCode array dataset.
    //ByteCode = new INT08[ByteCodeSize];
    if (ENV_H5::HDF_Read(ByteCode, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + FPT_HDF_DATA_NAMES[5] + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Immed array dataset.
    //Immed = new REAL64[ImmedSize];
    if (ENV_H5::HDF_Read(Immed, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + FPT_HDF_DATA_NAMES[6] + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Stack array dataset.
    if (ENV_H5::HDF_Read(Stack, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + FPT_HDF_DATA_NAMES[7] + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Formula Name dataset.
    if (ENV_H5::HDF_Read_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + FPT_HDF_DATA_NAMES[8] + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    // Read Temperature Range array dataset.
    if (ENV_H5::HDF_Read(tRange, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
    {
        error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + FPT_HDF_DATA_NAMES[9] + "' cannot be read.";
        ENV::StopByError(3005, &error_message);
    }
    if (GroupName.length())
    {
        error_message[2] = " FunctionParserType::freadHDF(), Group Close Error '" + GroupName + "'.";
        ENV_H5::HDF_group_close(group_id, GroupName);
        error_message[2] = "";
    }
    //auto s_tmp = ENV::v_join(Name, " : ");
    //std::cout << END_LINE << s_tmp << END_LINE;
    //std::cout << ByteCodeSize << " : ";
    //for (auto i = 0; i < ByteCodeSize; ++i) std::cout << (int)ByteCode[i] << ", ";
    //std::cout << END_LINE;
    //std::cout << ImmedSize << " : ";
    //for (auto i = 0; i < ImmedSize; ++i) std::cout << Immed[i] << ", ";
    //std::cout << END_LINE;
    //std::cout << StackSize << " : ";
    //for (auto i = 0; i < StackSize; ++i) std::cout << Stack[i] << ", ";
    //std::cout << END_LINE << END_LINE;
}

#endif
#endif

// Print error message and terminate program
// FuncStr - Original function string
void PrintErrorMessage(const size_t j, const STRING& FuncStr, const STRING& Msg)
{
    if (Msg.length()) error_message[1] = "**** Error in syntax of function string: " + Msg;
    else error_message[1] = "**** Error in syntax of function string:";
    error_message[2] = FuncStr;
    // Advance to the j-th position
    error_message[3] = STRING(j, ' ') + '?';
    ENV::StopByError(1005, &ENV_FP::error_message);
}

// Check syntax of function string, returns 0 if syntax is OK
// Func    - Function string without spaces
// FuncStr - Original function string
// Var(:)  - Array with variable names
void CheckSyntax(VECTOR<int>& ipos, const STRING& Func, const STRING& FuncStr, V_STRING& Var)
{
    // integer(int32)  ::ParCnt  !Parenthesis counter
    int ParCnt = 0;
    int j = 1, lFunc = Func.length() - 1;
    // step begin
    for (;;)
    {
        if (j > lFunc) ENV_FP::PrintErrorMessage(ipos[lFunc], FuncStr);
        char c = Func[j];
        // -- -------- -------- - -------- - -------- - -------- - -------- - -------- - ------ -
        // Check for valid operand(must appear)
        // -- -------- -------- - -------- - -------- - -------- - -------- - -------- - ------ -
        // Check for leading '-' or '+' or '!'
        if (ENV::isCharInText(c, OpsUnary))
        {
            if (++j > lFunc) ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Missing operand");
            c = Func[j];
            if (OpsAll.find(c) != STRING::npos) ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Multiple operators");
        }
        INT08 n = MathFunctionIndex(Func.substr(j));
        // Check for math function
        if (n > 0)
        {
            j += Funcs[n - cAbs].length();
            if (j > lFunc) ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Missing function argument");
            c = Func[j];
            if (c != '(') ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Missing opening parenthesis");
        }
        // Check for opening parenthesis
        if (c == '(')
        {
            ++ParCnt;
            ++j;
            continue;
        }
        if (ENV::isCharInText(c, FloatDigits))
        {
            // Check for number
            bool err;
            int iib, iin;
            double r = RealNum(Func.substr(j), &iib, &iin, &err);
            if (!err)
            {
                STRING s_tmp = ENV::s_slice(Func, j + iib - 1, j + iin - 2);
                ENV_FP::PrintErrorMessage(ipos[j], FuncStr,
                    "Invalid number format:  " + s_tmp);
            }
            j += iin;
            if (j > lFunc) break;
            c = Func[j];
        }
        else
        {
            // Check for variable
            int iib, iin;
            n = VariableIndex(Func.substr(j), Var, &iib, &iin);
            if (n < 0)
            {
                STRING s_tmp = ENV::s_slice(Func, j + iib - 1, j + iin - 2);
                ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Invalid element: " + s_tmp);
            }
            j += iin;
            if (j > lFunc) break;
            c = Func[j];
        }
        // Check for closing parenthesis
        while (c == ')')
        {
            if (--ParCnt < 0) ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Mismatched parenthesis");
            if (Func[j - 1] == '(') ENV_FP::PrintErrorMessage(ipos[j - 1], FuncStr, "Empty parentheses");
            if (++j > lFunc) break;
            c = Func[j];
        }
        // Now, we have a legal operand : A legal operator or end of string must follow
        if (j > lFunc) break;
        if (ENV::isCharInText(c, OpsAll))   // Check for multiple operators
        {
            if (j + 1 > lFunc) PrintErrorMessage(ipos[j], FuncStr);
            if (ENV::isCharInText(Func[j + 1], OpsAll))
                ENV_FP::PrintErrorMessage(ipos[j + 1], FuncStr, "Multiple operators");
        }
        else                                // Check for next operand
            ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Missing operator");
        // Now, we have an operand and an operator: the next loop will check for another operand (must appear)
        ++j;
    }   // step end
    if (ParCnt > 0) ENV_FP::PrintErrorMessage(ipos[j], FuncStr, "Missing parenthesis ')'");
}

} // namespace ENV_FP

