#include "FunctionParser.hpp"
#include "ENV_types.hpp"
#include <unordered_map>

namespace ENV_FP
{

void FunctionParser::clear()
{
    ipos.clear();
    if (Comp != nullptr)
    {
        for (auto ci : *Comp) if (ci != nullptr) ci->clear();
        for (auto ci : *Comp) PTR_DELETE(ci);
        //for (auto i = ENV_1_I32; i <= Length; ++i) delete (*Comp)[i];
        if (Comp->size()) Comp->clear();
        PTR_DELETE(Comp);
    }
    Length = 0;
    iError = 0;
}

// Initializer of function parser for nFunc functions
void FunctionParser::init(const int nFunc)
{
    clear();
    Comp = new VECTOR<FunctionParserType*>(1);
    for (auto i = 0; i < nFunc; ++i) add();
    for (auto ci : *Comp) if (ci != nullptr) ci->clear();
}

// Initialize function parser from reading of file FileName with key KeyName
FunctionParser::FunctionParser(const STRING& FileName, const STRING& KeyName)
{
    clear();
    if (FileName.empty()) return;
    if (KeyName.empty()) readText(FileName);
    else readText(FileName, KeyName);
}

// Destruct function parser for n functions
FunctionParser::~FunctionParser()
{
    clear();
}

// Print error message and terminate program
// FuncStr - Original function string
void FunctionParser::printError(const int j, const STRING& FuncStr, const STRING& Msg)
{
    if (Msg.length()) error_message[1] = "*** Error in syntax of function string: " + Msg;
    else error_message[1] = "*** Error in syntax of function string:";
    error_message[2] = FuncStr;
    for (int k = 0; k < j; ++k) error_message[3] += ' ';  // Advance to the j-th position
    error_message[3] += '?';
    ENV::StopByError(1005, &error_message);
}

// Return error message
// Evaluator of Error Message (Use only when iError>0)
const STRING FunctionParser::errorMessage()
{
    V_STRING m =
    {
    "Division by zero.",
    "Argument of sqrt is negative.",
    "Argument of log is negative.",
    "Argument of asin or acos is illegal."
    };
    return ((iError < 1) || (iError + 2 > m.size())) ? STRING_EMPTY : m[iError - 1];
}

// ***Read all formulas from Text File
void FunctionParser::readText(const STRING& FileName, const STRING& KeyName)
{
    // Open an existing text file.
    if (FileName.empty())
    {
        error_message[1] = " readText(), cannot read from input Text File,";
        error_message[2] = " File Open Error '" + FileName + "'.";
        ENV::StopByError(3005, &error_message);
    }
    FSTREAM FileOfInput;
    FileOfInput.open(FileName, std::ios::in);
    if (FileOfInput.fail())
    {
        iError = -1;
        error_message[1] = " readText(), cannot read from input Text File,";
        error_message[2] = " File Open Error '" + FileName + "'.";
        error_message[3] = " Error #" + std::to_string(iError) + ".";
        ENV::StopByError(3005, &error_message);
    }
    clear();
    STRING key = (KeyName.empty()) ? "FNC" : KeyName;
    STRING skey, stmp;
    for (;;)
    {
        STRING stmp;
        // read head
        std::getline(FileOfInput, stmp);
        if (FileOfInput.fail())
        {
            FileOfInput.close();
            return;
        }
        stmp = ENV::trim(stmp);
        auto ksf = stmp.find(' ');
        skey = (ksf != STRING::npos) ? stmp.substr(0, ksf) : STRING_EMPTY;
        if (skey.compare(key) == 0) break;
    }
    // Read size of Parser
    stmp = stmp.substr(skey.length());
    int id;
    if (ENV::StrToInt(stmp, id))
    {
        error_message[1] = " readText(), cannot read 'number parameter' from Text File:";
        error_message[2] = " '" + FileName + "'";
        error_message[3] = " (error in '" + key + "' or '" + FP_HDF_DATA_NAMES[1] + "')";
        ENV::StopByError(1005, &error_message);
    }
    init(id);
    V_STRING LineArray;
    while (std::getline(FileOfInput, stmp)) LineArray.push_back(stmp);
    if (LineArray.size() == 0)
    {
        iError = -2;
        error_message[1] = " Cannot read from input Text File '" + FileName + "'.";
        error_message[2] = " File Reading Error, String = '" + stmp + "'.";
        error_message[3] = ENV::add_AI(" Error #", iError) + " in readText().";
        ENV::StopByError(3005, &error_message);
    }
    FileOfInput.close();
    if (FileOfInput.fail())
    {
        error_message[1] = " File Close Error '" + FileName + "'.";
        error_message[2] = ENV::add_AI(" Error #", iError) + " in readText().";
        ENV::StopByError(3005, &error_message);
    }
    // Read Comp array from text file.
    STRING FunctionKey = key.substr(0, 1);
    for (int i = 1; i < Comp->size(); ++i)
    {
        STRING sf = ENV::add_AI(FunctionKey, i);
        stmp = (*Comp)[i]->readText(LineArray, sf);
        if (stmp.compare("FINISH") == 0) parse(i, "(0)", VAR_DATA_NAMES, sf);
        else parse(i, stmp, VAR_DATA_NAMES, sf);
    }
    LineArray.clear();
}

// ***Read all formulas from String Vector
void FunctionParser::readText(const V_STRING& StrVector, const STRING& KeyName)
{
    // Open an existing text file.
    if (StrVector.empty())
    {
        error_message[1] = " readText(), cannot get from empty String Vector,";
        ENV::StopByError(3005, &error_message);
    }
    clear();
    STRING key = (KeyName.empty()) ? "FNC" : KeyName;
    STRING skey, stmp;
    // Get Head
    int ihead = 0;
    for (auto vsi = StrVector.begin(); ; ++vsi, ++ihead)
    {
        STRING stmp = *vsi;
        stmp = ENV::trim(stmp);
        auto ksf = stmp.find(' ');
        skey = (ksf != STRING::npos) ? stmp.substr(0, ksf) : STRING_EMPTY;
        if (skey.compare(key) == 0) break;
    }
    // Get size of Parser
    stmp = stmp.substr(skey.length());
    int id = 0;
    if (ENV::StrToInt(stmp, id))
    {
        error_message[1] = " freadText(), cannot get 'number parameter' from String Vector.";
        error_message[2] = " (error in '" + key + "' or '" + FP_HDF_DATA_NAMES[1] + "')";
        ENV::StopByError(1005, &error_message);
    }
    init(id);
    V_STRING LineArray = ENV::v_slice(StrVector, ihead, StrVector.size());
    if (LineArray.size() == 0)
    {
        iError = -2;
        error_message[1] = " Cannot get from empty rest of String Vector.";
        error_message[2] = " String = '" + stmp + "'.";
        error_message[3] = ENV::add_AI(" Error #", iError) + " in readText().";
        ENV::StopByError(3005, &error_message);
    }
    // Get Comp array from String Vector.
    STRING FunctionKey = key.substr(0, 1);
    int i = 1;
    for (auto ci = Comp->begin() + 1; ci != Comp->end(); ++ci, ++i)
    {
        //rewind(iFile)
        STRING sf = ENV::add_AI(FunctionKey, i);
        stmp = (*ci)->readText(LineArray, sf);
        if (stmp.compare("FINISH") == 0) parse(i, "(0)", VAR_DATA_NAMES, sf);
        else parse(i, stmp, VAR_DATA_NAMES, sf);
    }
    LineArray.clear();
}

// ***Write all formulas from Text File
void FunctionParser::writeText(const STRING& FileName, const STRING& KeyName)
{
    if (Length == 0) return;
    // Open an existing text file.
    if (FileName.empty())
    {
        error_message[1] = " writeText(), cannot write to output Text File,";
        error_message[2] = " File Open Error '" + FileName + "'.";
        ENV::StopByError(3005, &error_message);
    }
    FSTREAM FileOfInput;
    FileOfInput.open(FileName, std::ios::app);
    if (FileOfInput.fail())
    {
        iError = -1;
        error_message[1] = " writeText(), cannot write to output Text File,";
        error_message[2] = " File Open Error '" + FileName + "'.";
        error_message[3] = " Error #" + std::to_string(iError) + ".";
        ENV::StopByError(3005, &error_message);
    }
    STRING key = (KeyName.empty()) ? "FNC" : KeyName;
    // Write head with size of Parser
    FileOfInput << key << " " << Length << END_LINE;
    // Read Comp array from text file.
    for (int i = 1; i < Comp->size(); ++i)
    {
        STRING s_tmp = vector_join((*Comp)[i]->Name, " ");
        FileOfInput << s_tmp << END_LINE;
    }
    FileOfInput << "FINISH" << END_LINE;
    FileOfInput.close();
    if (FileOfInput.fail())
    {
        error_message[1] = " File Close Error '" + FileName + "'.";
        error_message[2] = ENV::add_AI(" Error #", iError) + " in writeText().";
        ENV::StopByError(3005, &error_message);
    }
}

// Parse i-th function string FuncStr and compile it into bytecode
// i        - Function identifier
// FuncStr  - Function string
// Var      - Array with variable names
// FunctionKey - Key of Formula Function
void FunctionParser::parse(const int i, const STRING& FuncStr, V_STRING& Var, const STRING FunctionKey)
{
    STRING Func;        // Function string, local use
    int prange = 0, pcomment = 0;
    if ((i < 1) || (i > Length))
    {
        error_message[1] = ENV::add_AI("*** Function number ", i) + " is out of range.";
        ENV::StopByError(1005, &error_message);
    }
    pcomment = FuncStr.find_last_of('#');
    if (pcomment >= 0) --pcomment;
    else pcomment = FuncStr.length();
    prange = FuncStr.substr(0, pcomment).find_last_of('@');
    FunctionParserType* ci = (*Comp)[i];
    if (prange != STRING::npos)
    {
        STRING RangeStr = FuncStr.substr((prange--) + 1);    // Temperature Range string, local use
        auto vRS = ENV::string_split(RangeStr);
        bool isRS = (vRS.size() == 2);
        if (isRS)
        {
            int j = 0;
            ci->tRange = new REAL64[2]{};
            //std::fill_n(ci->tRange, 2, 0.0);
            for (auto t = vRS.begin(); t != vRS.end(); ++t, ++j)
            {
                isRS = ENV::StrToReal64(*t, ci->tRange[j]);
                if (!isRS) break;
            }
        }
        if (!isRS)
        {
            error_message[1] = " cannot read from input Text File the Temperature Range,";
            error_message[2] = " File Read Error.";
            ENV::StopByError(1005, &error_message);
        }
    }
    else prange = ENV::rtrim(FuncStr.substr(0, pcomment)).length();
    Func = FuncStr.substr(0, prange);       // Local copy of function string
    if (!fpRemoveSpaces(Func))              // Condense function string
    {
        error_message[1] = " wrong syntax in formula '" + FuncStr + "'.";
        error_message[2] = " Cannot remove spaces and change two-char commands.";
        ENV::StopByError(1005, &error_message);
    }
    CheckSyntax(ipos, Func, FuncStr, Var);
    ipos.clear();
    fpCompile(ci, Func, Var);          // Compile into ByteCode
    if (FunctionKey.empty()) ci->Name[0] = ENV::add_AI("F", i);
    else ci->Name[0] = FunctionKey;
    ci->Name[1] = FuncStr;
}

// Remove Spaces from string, remember positions of characters in old string
bool FunctionParser::fpRemoveSpaces(STRING& sstr)
{
    //ipos = new int[prange + 1];      // Char.positions in orig.string
    int lstr = sstr.length();
    ipos = VECTOR<int>(lstr);
    for (int k = 0; k < lstr; ++k) ipos[k] = k;
    for (int k = 0; k < lstr; ++k)
        if (sstr[k] == ' ') ipos[k] = -1;
    V_STRING vs1 = { "**", "<=", ">=", "==", "!=", "||", "&&" };
    V_STRING vs2 = { "^",  "[",  "]",  "=",  "~",  "|",  "&" };
    for (int k = 0; k < lstr - 1; ++k)
        if (ENV::isStringInVector(sstr.substr(k, 2), vs1))
            ipos[(k++) + 1] = -1;
    ipos.erase(std::remove(ipos.begin(), ipos.end(), -1), ipos.end());
    ipos.insert(ipos.begin(), -1);
    /*
    Replacing:
    ReplaceStringInPlace(Func, "**", "^");  // Exponent into One-Char.format
    ReplaceStringInPlace(Func, "<=", "[");  // Lower and equal into One-Char.format
    ReplaceStringInPlace(Func, ">=", "]");  // Bigger and equal  into One-Char.format
    ReplaceStringInPlace(Func, "==", "=");  // Equal into One-Char.format
    ReplaceStringInPlace(Func, "!=", "~");  // Not equal into One-Char.format
    ReplaceStringInPlace(Func, "||", "|");  // Logical or into One-Char.format
    ReplaceStringInPlace(Func, "&&", "&");  // Logical and into One-Char.format
    */
    std::unordered_map<STRING, STRING> dict; // or std::map
    int minSize = std::min(vs1.size(), vs2.size());
    for (int i = 0; i != minSize; ++i)
        dict.insert(std::make_pair(vs1[i], vs2[i]));
    for (auto s : dict)
        ENV::ReplaceStringInPlace(sstr, s.first, s.second);
    sstr.erase(remove(sstr.begin(), sstr.end(), ' '), sstr.end());
    sstr = " " + sstr;
    return (sstr.length() == ipos.size());
}

// Compile i-th function string F into ByteCode
// i - Function identifier
// F - Function string
// Var - Array with variable names
void FunctionParser::fpCompile(FunctionParserType* ci, const STRING& F, V_STRING& Var)
{
    if (ci->ByteCode) ci->clear();
    // Compile string to determine size
    ci->compilef(F, 1, F.length() - 1, Var);
    //allocate(Comp(i) % ByteCode(Comp(i) % ByteCodeSize), Comp(i) % Immed(Comp(i) % ImmedSize),
    //    Comp(i) % Stack(Comp(i) % StackSize), STAT = istat)
    if (ci->init())
    {
        ci->ByteCodeSize = 0;
        ci->ImmedSize = 0;
        ci->StackSize = 0;
        ci->StackPtr = 0;
        // Compile string into bytecode
        ci->compilef(F, 1, F.length() - 1, Var);
    }
    else
    {
        error_message[1] = "*** Memory allocation for byte code failed.";
        ENV::StopByError(3005, &error_message);
    }
}

// Adder of parser with new empty single formula
void FunctionParser::add()
{
    Comp->push_back(new FunctionParserType());
    ++Length;
}

// Add compiled byte to ByteCode of ith function parser
// i - Function identifier
// b - Value of byte to be added
void FunctionParser::fpAddCompiledByte(const int i, const INT08 b)
{
    (*Comp)[i]->add(b);
}

// Name of ith function
// i   - Function identifier
V_STRING FunctionParser::getName(const int i)
{
    return (*Comp)[i]->Name;
}

// Evaluate bytecode of ith function for the values passed in array Val
// i   - Function identifier
// Val - Variable values
//forceinline
//REAL64 FunctionParser::evaluate(const int i, const REAL64* Val)
//{
//    return (*Comp)[i]->evalf(Val, &iError);
//}

#ifdef ENV_HDF5_HPP

#ifdef H5Cpp_H

// ***Write parser(all formulas) to HDF-file
void FunctionParser::fwriteHDF(H5::Group* parent_id, const STRING* group)
{
    STRING GroupName = (group) ? ((group->empty()) ? FP_HDF_DATA_NAMES[0] : *group) : STRING_EMPTY;
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
            error_message[1] = " FunctionParser::fwriteHDF(), cannot write to output HDF-file,";
            error_message[2] = " Group Create Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    // write to HDF the Parser Size
    V_STRING vs_tmp = vslice(FP_HDF_DATA_NAMES, 1, 1);
    if (ENV_H5::HDF_HeaderWriteSimple(&group_id, STRING_EMPTY, vs_tmp, VECTOR<INT32>({ Length })))
    {
        //auto fn = parent_id->getFileName();
        //auto pn = parent_id->fromClass();
        error_message[1] = " cannot write to output HDF-file '" + STRING() + "'";
        error_message[2] = " GROUP='" + STRING() + GroupName + "',";
        error_message[3] = " NAMES=['" + ENV::trim(FP_HDF_DATA_NAMES[1]) + "']";
        ENV::StopByError(3005, &error_message);
    }
    // Write Comp array.
    if (Comp)
    {
        int i = 1;
        for (auto ci = Comp->begin() + 1; ci != Comp->end(); ++ci)
            (*ci)->fwriteHDF(&group_id, add_AI("FORMULA #", i++));
    }
    try
    {
        // close last group
        if (!GroupName.empty()) group_id.close();
    }
    catch (H5::Exception&)
    {
        error_message[1] = " FunctionParser::fwriteHDF(), Group Close Error '" + GroupName + "'.";
        ENV::StopByError(3005, &error_message);
    }
}

// ***Read parser(all formulas) from HDF - file
void FunctionParser::freadHDF(const H5::Group* parent_id, const STRING* group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = (group) ? ((group->empty()) ? FP_HDF_DATA_NAMES[0] : *group) : STRING_EMPTY;
    H5::Group group_id;            // Group identifier
    if (GroupName.empty()) group_id = *parent_id;
    else
    {
        try
        {
            group_id = H5::Group(parent_id->openGroup(GroupName));
        }
        catch (const H5::GroupIException&)
        {
            error_message[1] = " FunctionParser::freadHDF(), cannot read from input HDF-file.";
            error_message[2] = " Group Open Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    init();
    //Read size of Parser
    VECTOR<INT32> rdata({ ENV_0_I32 });
    V_STRING vs_tmp = vslice(FP_HDF_DATA_NAMES, 1, 1);
    if (ENV_H5::HDF_HeaderReadSimple(group_id, STRING_EMPTY, vs_tmp, rdata))
    {
        error_message[1] = " cannot read from input HDF-file:";
        error_message[2] = " GROUP='" + GroupName + "',";
        error_message[3] = " NAMES=['" + FP_HDF_DATA_NAMES[1] + "']";
        ENV::StopByError(3005, &error_message);
    }
    // Read Comp array.
    for (auto i = ENV_1_I32; i <= rdata[0]; ++i)
    {
        add();
        (*Comp)[i]->freadHDF(group_id, add_AI("FORMULA #", i));
    }
    try
    {
        // close last group
        if (!GroupName.empty()) group_id.close();
    }
    catch (H5::Exception&)
    {
        error_message[1] = " FunctionParser::freadHDF(), Group Close Error '" + GroupName + "'.";
        ENV::StopByError(3005, &error_message);
    }
}

#else

// ***Write parser(all formulas) to HDF-file
void FunctionParser::fwriteHDF(const hid_t parent_id, const STRING* group)
{
    STRING GroupName = (group) ? ((group->empty()) ? FP_HDF_DATA_NAMES[0] : *group) : STRING_EMPTY;
    // Open an existing group in the specified file group.
    hid_t group_id; // Group identifier
    if (GroupName.empty()) group_id = parent_id;
    else
    {
        group_id = ENV_H5::HDF_group_open(parent_id, GroupName, "create");
        if (group_id < 0)
        {
            error_message[1] = " FunctionParser::fwriteHDF(), cannot write to output HDF-file,";
            error_message[2] = " Group Create Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    // Write to HDF the Parser Size
    V_STRING vs_tmp = v_slice(FP_HDF_DATA_NAMES, 1, 1);
    if (ENV_H5::HDF_Write_Attributes(VECTOR<INT32>{Length}, vs_tmp, group_id) < 0)
    {
        //auto fn = parent_id->getFileName();
        //auto pn = parent_id->fromClass();
        error_message[1] = " cannot write to output HDF-file '" + STRING() + "'";
        error_message[2] = " GROUP='" + STRING() + GroupName + "',";
        error_message[3] = " NAMES=['" + FP_HDF_DATA_NAMES[1] + "']";
        ENV::StopByError(3005, &error_message);
    }
    // Write Comp array.
    if (Comp)
    {
        int i = 1;
        for (auto ci = Comp->begin() + 1; ci != Comp->end(); ++ci)
            (*ci)->fwriteHDF(group_id, add_AI("FORMULA #", i++));
    }
    // close last group
    if (!GroupName.empty())
    {
        error_message[2] = " FunctionParser::fwriteHDF(), Group Close Error '" + GroupName + "'.";
        ENV_H5::HDF_group_close(group_id, GroupName);
        error_message[2] = "";
    }
}

// ***Read parser(all formulas) from HDF - file
void FunctionParser::freadHDF(const hid_t parent_id, const STRING* group)
{
    // Open an existing group in the specified file group.
    STRING GroupName = (group) ? ((group->empty()) ? FP_HDF_DATA_NAMES[0] : *group) : STRING_EMPTY;
    hid_t group_id;            // Group identifier
    if (GroupName.empty()) group_id = parent_id;
    else
    {
        group_id = ENV_H5::HDF_group_open(parent_id, GroupName);
        if (group_id < 0)
        {
            error_message[1] = " FunctionParser::freadHDF(), cannot read from input HDF-file.";
            error_message[2] = " Group Open Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }
    init();
    // Read from HDF the Parser Size
    VECTOR<INT32> rdata{ ENV_0_I32 };
    V_STRING vs_tmp = v_slice(FP_HDF_DATA_NAMES, 1, 1);
    if (ENV_H5::HDF_Read_Attributes(rdata, vs_tmp, group_id) < 0)
    {
        error_message[1] = " cannot read from input HDF-file:";
        error_message[2] = " GROUP='" + GroupName + "',";
        error_message[3] = " NAMES=['" + FP_HDF_DATA_NAMES[1] + "']";
        ENV::StopByError(3005, &error_message);
    }
    // Read Comp array.
    for (auto i = ENV_1_I32; i <= rdata[0]; ++i)
    {
        add();
        (*Comp)[i]->freadHDF(group_id, add_AI("FORMULA #", i));
    }
    // close last group
    if (!GroupName.empty())
    {
        error_message[2] = " FunctionParser::freadHDF(), Group Close Error '" + GroupName + "'.";
        ENV_H5::HDF_group_close(group_id, GroupName);
        error_message[2] = "";
    }
}

#endif

#endif

} // namespace ENV_FP

