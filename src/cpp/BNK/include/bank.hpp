#pragma once
#include <cstdio>
#include <string>
#include "ENV_types.hpp"

#define NIRP 48
#define LSTATE 25

using namespace ENV;

// ќбъ€вление класса
class BANK_Struct
{
    STRING name;
    INT32 iError = ENV_0_I32;
    INT32 NMQUE = ENV_0_I32;
    INT32 nKQUESE = ENV_0_I32;
    INT32 LHEADS = ENV_0_I32;
    INT32 LMQEXT = ENV_0_I32;
    INT32 MPI_MODE = ENV_0_I32;
    INT64* KQUESE;
    INT64* KQUES;
    INT64* BnkMemb;
    REAL64* QueueWeight;
    INT32 MPI_ID;
    INT32 MPI_SIZE;
    REAL64* MPI_BNK_WEIGHTS;
    INT64* MPI_LENQ;
    INT64* MPI_BNK_SEND;
    INT64* MPI_BNK_RECV;
    INT32* MPI_BNK_SEND_QUEUE;
    INT32* MPI_BNK_RECV_QUEUE;
    VECTOR<STRING> error_message = { "BANK error:", "", "" };
private:
    void Normalize(const INT64& NTOT, const INT32& NC, const INT32 NF, const REAL64 WT);
    void Place(char BE, INT32 NQUE, REAL64* EC, INT32* MEV, REAL64 WQA, INT32 NQA, INT32 NFREE);
    void fetch(const INT32 NQUE, bool& REZ, REAL64* EC, INT32* MEV);
    void storeDeck(const INT32 NQUE, bool& REZ, REAL64* EC, INT32* MEV, const INT64 NPINDC, const char STDK);
    void free(const INT64 NQUE, REAL64 WT, INT64 NN);
    void pack(INT64 KFT, INT64 HQ0, INT64 KQB, INT64 KQT, INT64 LBLKD, INT64 MEM, INT64 NI);
    void create(const STRING name, const INT32 MPI_MODE, const INT32 NMQUE);
    void destroy();
    void bnkFinal();
};