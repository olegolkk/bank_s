//#include <stdint.h>
#include <cstdio>
#include <string>
#include "ENV_types.hpp"
#include "bank.hpp"


#define NIRP 48
#define LSTATE 25
#define BANK_INT32_ATTRIBUTES_COUNT 2
#define ENV_CHAR_STRING_LENGTH 256

//using namespace ENV;

// Normalize of the bank of fission neutrons to NTOT source neutrons with WEIGHT=1.
void BANK_Struct::Normalize(const INT64& NTOT, const INT32& NC, const INT32 NF, const REAL64 WT)
{
    //NC - number of queue of neutrons to store.
    //NF - number of queue of neutrons to fetch.
    bool REZ = true;
    INT32 MEV[NIRP], J;
    INT64 LENQ;
    REAL64 rng1 = 0, EC[LSTATE], S, CNM;

    LENQ = KQUES[4 * NF]; // Queue length
    if (LENQ < 1)
    {
        std::cout << " Bank normalisation error (1), total bank weight=" << QueueWeight[NF];
        StopByError(-2001);
    }
    CNM = NTOT / QueueWeight[NF];
    S = rng1;
    for (J = 1; J < NTOT; J++)
    {
        while ((LENQ > ENV_0_I64) && (J >= S))
        {
            fetch(NF, REZ, EC, MEV);
            S = S + EC[ENV_2_I32] * CNM;
            EC[ENV_2_I32] = WT;
            LENQ = LENQ - ENV_1_I64;
        }
        storeDeck(NC, REZ, EC, MEV, ENV_1_I64, 'E');
        if (!REZ)
        {
            // ERROR
            StopByError(-2001);
        }
    }
    while (REZ)
    {
        fetch(NF, REZ, EC, MEV);
    }
}

// Storing one element (EC,MEV) to queue number NQUE: to the endif BE = 'E', otherwise - to the head.
    // Results:
    // NFREE = number of free blocks(-1 if reject);
    // WQA and NQA are sum weight and number of elements in the queue used.
void BANK_Struct::Place(char BE, INT32 NQUE, REAL64* EC, INT32* MEV, REAL64 WQA, INT32 NQA, INT32 NFREE)
{
    bool R;
    INT64 I;
    REAL64* QUES = new REAL64[LHEADS];
    int locKQUES = *QUES;
    locKQUES = *KQUES;
    NFREE = ENV_1M_I32;
    storeDeck(NQUE, R, EC, MEV, ENV_1_I64, BE);
    if (!R)
    {
        NFREE = KQUES[6];
        I = NQUE * 4;
        WQA = QUES[I - 1];
        NQA = KQUES[I];
    }
}

// Fetch one element from queue number NQUE
void BANK_Struct::fetch(const INT32 NQUE, bool& REZ, REAL64* EC, INT32* MEV)
{
    STRING PPC;
    char C;
    INT64 I, N, KT;
    INT64 MR;
    REAL64* RM = (REAL64*)&MR;

    REAL64* QUES = (REAL64*) &KQUES;
    C = ' ';
    N = NQUE;
    if ((NMQUE < N) or (N < 1))
    {
        PPC = "STORE";
        if (C == ' ') PPC = "FETCH";
        error_message[1] = "error in call subroutine ' " + PPC + C + " ' bad NQUE = " + std::to_string(NQUE);
        StopByError(-2001, &error_message);
    }
    N = N * 4 - 3;
    REZ = false;
    KT = KQUES[N];
    if (KT == ENV_0_I64) return;
    REZ = true;
    // Correction the number of particles in the deck
    I = KT + KQUESE[4];
    BnkMemb[I - 1] = BnkMemb[I - 1] - 1;
    if (BnkMemb[I - 1] == 0)
    {
        // Move the Element to free place list
        KQUES[N] = BnkMemb[KT];
        BnkMemb[KT] = KQUESE[3];
        KQUESE[3] = KT;

        // Correcion the Number of Members
        KQUES[N + 3] = KQUES[N + 3] - 1;
        KQUESE[6] = KQUESE[6] + 1;
        KQUESE[7] = KQUESE[7] - 1;
    }
    // Correction the sum of Waiters
    if (KQUES[N + 3] > ENV_0_I64)
    {
        MR = BnkMemb[KT + 2];
        QUES[N + 2] = QUES[N + 2] - *RM;
        QueueWeight[NQUE] = QueueWeight[NQUE] - *RM;
    }
    else {
        QUES[N + 2] = ENV_0_I64;
        QueueWeight[NQUE] = ENV_0_R64;
    }
    // Write to formal parameters
    for (I = 1; I < LSTATE; I++)
    {
        MR = BnkMemb[KT + I];
        EC[I] = *RM;
    }
    KT = KT + LSTATE;
    for (int i = 0; i < NIRP; i++)
    {
        MEV[i] = BnkMemb[KT + i];
    }
}

//  Store one particles deck of length NPINDC to queue number NQUE.
void BANK_Struct::storeDeck(const INT32 NQUE, bool& REZ, REAL64* EC, INT32* MEV, const INT64 NPINDC, const char STDK)
{
    STRING PPC;
    char C;
    INT64 I, KE, KQ, KT, N, NPINPA;
    INT64* WTT;
    INT64 MR;
    void* vMR = &MR;
    REAL64* RM = (REAL64*)vMR;
    //int locKQUES;
    REAL64* QUES = new REAL64[LHEADS];
    //locKQUES = *QUES;
    // STDK = 'B' means setting to the head of the queue
    C = STDK;
    NPINPA = NPINDC;
    N = NQUE;
    if ((NMQUE < N) || (N < 1)) {
        PPC = "STORE";
        if (C == ' ') PPC = "FETCH";
        error_message[1] = " at call subroutine '" + PPC + C + "' bad NQUE : " + std::to_string(NQUE);
        StopByError(-2001, &error_message);
    }
    N = N * 4 - 3;
    REZ = true;
    // Searching for free place
    if (KQUESE[3] /= ENV_0_I64) {
        KT = KQUESE[3];
        KQUESE[3] = BnkMemb[KT];
    }
    else {
        // Is it necessary to increase the bank
        MR = KQUESE[2] + KQUESE[4];
        if (KQUESE[1] < MR) {
            if (KQUESE[1] >= 2 * 27)
            {
                // Increasing 
                REZ = false;
                return;
            }
            // Correction KQUESE[6]
            KQ = KQUESE[1] / KQUESE[4];
            KQUESE[1] = KQUESE[1] * 2;
            KQUESE[6] = KQUESE[1] / KQUESE[4] - (KQ - KQUESE[6]);
            INT64* WTT = new INT64[KQUESE[2]];
            for (int i = 0; i < KQUESE[2]; i++)
            {
                WTT[i] = BnkMemb[i];
            }
            delete [] BnkMemb;
            INT64* BnkMemb = new INT64[KQUESE[1]];
            for (int i = 0; i < KQUESE[2]; i++)
            {
                BnkMemb[i] = WTT[i];
            }
            for (int i = 0; i < KQUESE[1]; i++)
            {
                BnkMemb[i + KQUESE[2]] = ENV_0_I64;
            }
            delete WTT;
        }
        KT = KQUESE[2] + ENV_1_I64;
        KQUESE[2] = MR;
    }
    for (auto I = 0; I < LSTATE; ++I)
    { // Write to free place
        *RM = EC[I];
        BnkMemb[KT + I] = MR;
    }
    KQ = KT + LSTATE;
    for (int i = 0; i < KQ + NIRP; i++)
    {
        BnkMemb[KQ + 1 + i] = MEV[i];
    }
    BnkMemb[KQ + NIRP + 1] + NPINPA;
    // FORM LIST as ENTRY is
    KQ = KQUES[N];
    if (C == 'B')
    {
        KQUES[N] = KT;
        if (KQ == 0)
        {
            KQUES[N + 1] = KT;
            BnkMemb[KT] = KQ;
        }
        else
        {
            BnkMemb[KE] + KT;
        }
        BnkMemb[KT] = ENV_0_I64;
    }
    // Correction the sum of WAITERS and number of MEMBERS
    KQUES[N + 3] = KQUES[N + 3] + 1;
    QUES[N + 2] = QUES[N + 2] + EC[2] * NPINPA;
    QueueWeight[NQUE] = QueueWeight[NQUE] + EC[2] * NPINPA;
    KQUESE[6] = KQUESE[6] - ENV_1_I64;
    KQUESE[7] = KQUESE[7] + ENV_1_I64;
}

// For Queue NQUE the TOTAL Weight set to WT. Number of Members set to NN.
   // Result of Function is the Number of free Members in the Bank.
void BANK_Struct::free(const INT64 NQUE, REAL64 WT, INT64 NN)
{
    INT32 bnkFree;
    INT64 N;
    //INT64* locKQUES;
    REAL64* QUES = (REAL64*)&KQUES;
    //locKQUES = &KQUES;
    N = NQUE * 4;
    WT = QUES[N - ENV_1_I64];
    NN = KQUES[N];
    bnkFree = KQUESE[6];
}

// Pack elements into the bank (move free place to the end of array)
void BANK_Struct::pack(INT64 KFT, INT64 HQ0, INT64 KQB, INT64 KQT, INT64 LBLKD, INT64 MEM, INT64 NI)
{
    KFT = KQUESE[3];
    KQUESE[3] = ENV_0_I32;
    // Obtain the number of members
    MEM = ENV_0_I32;
    for (NI = 1; NI < NMQUE; NI++)
    {
        MEM = MEM + KQUES[4 * NI];
    }
    KQUESE[2] = KQUESE[4] * MEM;
    LBLKD = KQUESE[4] - ENV_1_I64;
    for (NI = 1; NI < NMQUE; NI++)
    { // Main loop of queues
        // HQ0 means the queue head index; KQB - index of previous member (zero for 1-st position); KQT - index of current member
        HQ0 = 4 * NI - 3;
        KQB = ENV_0_I64;
        KQT = KQUES[HQ0];
        while (KQT > ENV_0_I64)
        {
            if (KQUESE[2] < KQT)
            {
                // Member must be moved. Looking for the new place.
                while (KQUESE[2] < KFT)
                {
                    KFT = BnkMemb[KFT];
                }
                if (KFT == ENV_0_I32)
                {
                    error_message[1] = " the structure of bank is wrong.";
                    StopByError(-2001, &error_message);
                }
                // Moving the member
                MEM = BnkMemb[KFT];
                for (int i = KFT; i < KFT + LBLKD; i++)
                {
                    BnkMemb[i] = BnkMemb[KQT + i];
                }
                KQT = KFT;
                KFT = MEM;
                // Change reference in previous member
                if (KQB > ENV_0_I64)
                {
                    BnkMemb[KQB] = KQT;
                }
                else
                {
                    KQUES[HQ0] = KQT;
                }
            }
            // Next member
            KQB = KQT;
            KQT = BnkMemb[KQB];
        }
        KQUES[HQ0 + ENV_1_I64] = KQB;
    }
}

void BANK_Struct::destroy()
{
    bnkFinal();
}

void BANK_Struct::bnkFinal() {
    if (BnkMemb) {
        delete[] BnkMemb;
        BnkMemb = nullptr;
    }
    if (KQUESE) {
        delete[] KQUESE;
        KQUESE = nullptr;
    }
    if (KQUES) {
        delete[] KQUES;
        KQUES = nullptr;
    }
    if (QueueWeight) {
        delete[] QueueWeight;
        QueueWeight = nullptr;
    }

    if (MPI_MODE > 0) {
        /*
        if (MPI_BNK_WEIGTHS) {
            delete[] MPI_BNK_WEIGTHS;
            MPI_BNK_WEIGTHS = nullptr;
        }
        */
        if (MPI_LENQ) {
            delete[] MPI_LENQ;
            MPI_LENQ = nullptr;
        }
        if (MPI_BNK_SEND) {
            delete[] MPI_BNK_SEND;
            MPI_BNK_SEND = nullptr;
        }
        if (MPI_BNK_RECV) {
            delete[] MPI_BNK_RECV;
            MPI_BNK_RECV = nullptr;
        }
        if (MPI_BNK_SEND_QUEUE) {
            delete[] MPI_BNK_SEND_QUEUE;
            MPI_BNK_SEND_QUEUE = nullptr;
        }
        if (MPI_BNK_RECV_QUEUE) {
            delete[] MPI_BNK_RECV_QUEUE;
            MPI_BNK_RECV_QUEUE = nullptr;
        }
        // MPI_BNK_SR_DIFF.closeAll();
    }

    name = "BANK";
    NMQUE = ENV_0_I32;
    nKQUESE = ENV_0_I32;
    LHEADS = ENV_0_I32;
    LMQEXT = ENV_0_I32;
    MPI_MODE = ENV_0_I32;
};

void BANK_Struct::create(const STRING name, const INT32 MPI_MODE, const INT32 NMQUE)
{
    INT32 ierr;
    this->NMQUE = NMQUE;
    this->MPI_MODE = MPI_MODE;
    nKQUESE = 7;
    LHEADS = this->NMQUE * 4;
    LMQEXT = LHEADS + nKQUESE;

    INT64* KQUESE = new INT64[nKQUESE]{};
    INT64* KQUES = new INT64[LHEADS]{};
    REAL64* QueueWeight = new REAL64[NMQUE]{};

    KQUESE[0] = pow(2, 18);
    KQUESE[4] = NIRP;
    KQUESE[3] = KQUESE[4] + LSTATE + 2;
    KQUESE[2] = ENV_0_I64;
    KQUESE[5] = KQUESE[0] / KQUESE[3];
    KQUESE[6] = ENV_0_I64;

    INT64* BnkMemb = new INT64[KQUESE[1]]{};

    this->name = name;

    if (this->MPI_MODE == 0) return;

    // ENV_MPI_Comm_size(MPI_SIZE);
    // ENV_MPI_Comm_rank(MPI_ID);
    try
    {
        MPI_BNK_WEIGHTS = new REAL64[MPI_SIZE];
    }
    catch (const std::bad_alloc& e)
    {
        error_message[1] = " Error in bnkCreate(while allocating MPI_BNK_WEIGTHS) :";
        error_message[2] = stringify(ierr);
        StopByError(-3001, &error_message);
    }
}

// Read the bank and other from HDF-file
void BANK_Struct::HDFRead(hid_t parent_id, const std::string& Group)
{
    hid_t group_id;
    std::string GroupName;
    LOGICAL lError;
    int32_t NQUEUE;
    int32_t MPI_MODE;

    this->destroy();

    // Open an existing group in the specified file group.
    if (!Group.empty())
    {
        GroupName = Group;
        name = Group;
    }
    else
    {
        GroupName = this->name;
    }

    iError = ENV_0_I32;
    if (GroupName.length() > 0)
    {
        group_id = ENV_H5::HDF_Group_Open(parent_id, GroupName, iError);
    }
    else
    {
        group_id = parent_id;
    }

    if (group_id < 0)
    {
        error_message[1] = "Error in bnkHDFRead. Group Open Error '" + GroupName + "'.";
        StopByError(-3001, &error_message);
    }
    VECTOR<INT32> result_int32;
    lError = ENV_H5::HDF_Read_Attributes(result_int32, V_STRING{"", ""}, group_id);
    if (!lError) {
        error_message[1] = "Error in bnkHDFRead: cannot read int32 attributes.";
        StopByError(-3001, &error_message);
    }

    NQUEUE = result_int32[0];
    MPI_MODE = result_int32[1];
    this->create(GroupName, MPI_MODE, NQUEUE);

    INT64 kquese_length;
    if (!(kquese_length = ENV_H5::HDF_Read(KQUESE, group_id, "KQUESE")))
    {
        error_message[1] = "Error in bnkHDFRead. KQUESE array cannot be read.";
        StopByError(-3001, &error_message);
    }

    INT64 kques_length;
    if (!(kques_length = ENV_H5::HDF_Read(KQUES, group_id, "KQUES")))
    {
        error_message[1] = "Error in bnkHDFRead. KQUES array cannot be read.";
        StopByError(-3001, &error_message);
    }

    PTR_DELETE(BnkMemb, []);
    BnkMemb = new int64_t[this->KQUESE[0]];

    if (!ENV_H5::HDF_Read(BnkMemb, group_id, "BnkMemb"))
    {
        error_message[1] = "Error in bnkHDFRead. BnkMemb array cannot be read.";
        StopByError(-3001, &error_message);
    }

    for (int i = this->KQUESE[1] + 1; i < this->KQUESE[0]; i++)
    {
        BnkMemb[i] = ENV_0_I64;
    }

    if (!ENV_H5::HDF_Read(this->QueueWeight, group_id, "QueueWeight"))
    {
        error_message[1] = "Error in bnkHDFRead. QueueWeight array cannot be read.";
        StopByError(-3001, &error_message);
    }

    if (GroupName.length() > 0)
    {
        H5Gclose(group_id);
        if (this->iError != ENV_0_I32) {
            error_message[1] = "Group Close Error '" + GroupName + "'.";
            StopByError(-3001, &error_message);
        }
    }
}

void BANK_Struct::HDFWrite(hid_t parent_id, const std::string& Group)
{
    hid_t group_id;
    std::string GroupName;
    bool lError;

    this->pack();

    // Open an existing group in the specified file group.
    if (!Group.empty())
    {
        GroupName = Group;
        this->name = Group;
    }
    else {
        GroupName = this->name;
    }

    if (GroupName.length() > 0)
    {
        group_id = H5Gcreate(parent_id, GroupName.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        if (group_id < 0)
        {
            error_message[1] = "Group Create Error '" + GroupName + "'.";
            StopByError(-3001, &error_message);
        }
    }
    else
    {
        group_id = parent_id;
    }

    VECTOR<INT32> head_int32{NMQUE, MPI_MODE};
    lError = ENV_H5::HDF_Write_Attributes(head_int32, V_STRING{"", ""}, group_id);
    if (!lError) {
        error_message[1] = "ERROR in bnkHDFWrite: cannot write int32 attributes.";
        StopByError(-3001, &error_message);
    }

    // Write KQUESE array dataset.
    if (!ENV_H5::HDF_Write(KQUESE, nKQUESE, group_id, "KQUESE"))
    {
        error_message[1] = "Error in bnkHDFWrite. KQUESE array cannot be written.";
        StopByError(-3001, &error_message);
    }

    // Write KQUES array dataset.
    if (!ENV_H5::HDF_Write(KQUES, LHEADS, group_id, "KQUES"))
    {
        error_message[1] = "Error in bnkHDFWrite. KQUES array cannot be written.";
        StopByError(-3001, &error_message);
    }

    // Write BnkMemb array dataset.
    INT64 bnkMembLength = sizeof(BnkMemb) / sizeof(BnkMemb[0]);
    if (!ENV_H5::HDF_Write(BnkMemb, bnkMembLength, group_id, "BnkMemb"))
    {
        error_message[1] = "Error in bnkHDFWrite. BnkMemb array cannot be written.";
        StopByError(-3001, &error_message);
    }

    // Write QueueWeight array dataset.
    INT64 QueueWeightLength = sizeof(QueueWeight) / sizeof(QueueWeight[0]);
    if (!ENV_H5::HDF_Write(QueueWeight, QueueWeightLength, group_id, "QueueWeight"))
    {
        error_message[1] = "Error in bnkHDFWrite. QueueWeight array cannot be written.";
        StopByError(-3001, &error_message);
    }

    if (GroupName.length() > 0)
    {
        H5Gclose(group_id);
        if (this->iError != ENV_0_I32) {
            error_message[1] = "Error in bnkHDFWrite. Group Close Error '" + GroupName + "'.";
            StopByError(-3001, &error_message);
        }
    }
}



