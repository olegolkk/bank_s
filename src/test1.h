#include <stdint.h>
#include <cstdio>
#include <string>
#include "enviroment/ENV_types.h"

// LSTATE = 88
// NIRP = 28

class BANK_Struct
{
private:
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
    VECTOR<STRING> error_message;

public:
    // Normalize of the bank of fission neutrons to NTOT source neutrons with WEIGHT=1.
    void Normalize(const INT64 NTOT, const INT32 NC, const INT32 NF, const REAL64 WT)
    {
        //NC - number of queue of neutrons to store.
        //NF - number of queue of neutrons to fetch.
        bool REZ = true;
        INT32 MEV[NIRP], J;
        INT64 LENQ;
        REAL64 rng1, EC[LSTATE], S, CNM;

        LENQ = KQUES[4 * NF]; // Queue length
        if (LENQ < 1)
        {
            error_message[1] << " Bank normalisation error (1), total bank weight=" << QueueWeight[NF];
            ENV_StopByError(-2001);
        }
        CNM = NTOT / QueueWeight[NF];
        S = rng1;
        for (J = 1; J < NTOT; J++)
        {
            while ((LENQ > ENV_0_I64) && (J >= S))
            {
                fetch(NF, REZ, EC, MEV);
                S = S + EC(ENV_2_I32) * CNM;
                EC(ENV_2_I32) = WT;
                LENQ = LENQ - ENV_1_I64;
            }
            storeDeck(NC, REZ, EC, MEV, ENV_1_I64, 'E');
            if (!REZ)
            {
                // ERROR
                ENV_StopByError(-2001);
            }
        }
        while (REZ) {
            fetch(NF, REZ, EC, MEV);
        }
    }
    
    // Storing one element (EC,MEV) to queue number NQUE: to the endif BE = 'E', otherwise - to the head.
    // Results:
    // NFREE = number of free blocks(-1 if reject);
    // WQA and NQA are sum weight and number of elements in the queue used.
    void Place(char BE, INT32 NQUE, REAL64* EC, INT32* MEV, REAL64 WQA, INT32 NQA, INT32 NFREE)
    {
        bool R;
        INT64 I;
        REAL64 *QUES = new REAL64[LHEADS];
        int locKQUES = *QUES;
        locKQUES = *KQUES;
        NFREE = ENV_1M_I32;
        storeDeck(NQUE, R, EC, MEV, ENV_1_I64, BE);
        if (!R) {
            NFREE = KQUES[6];
            I = NQUE * 4;
            WQA = QUES[I - 1];
            NQA = KQUES[I];
        }
    }
    
    // Fetch one element from queue number NQUE
    void fetch(const INT32 NQUE, bool REZ, REAL64 EC, INT32 MEV) {
        STRING PPC;
        char C;
        INT64 I, N, KT;
        INT64 MR;
        REAL64 RM;

        int locKQUES;
        REAL64* QUES = new REAL64[LHEADS];
        locKQUES = *QUES;
        C = ' ';
        N = NQUE;
        if ((NMQUE < N) or (N < 1)) {
            PPC = "STORE";
            if (C == ' ') PPC = "FETCH";
            error_message[0] = { "error in call subroutine ' " + PPC + C + " ' bad NQUE = " + std::to_string(NQUE)};
            ENV_StopByError(-2001, &error_message);
        }
        N = N * 4 - 3;
        REZ = false;
        KT = KQUES[N];
        if (KT == ENV_0_I64) return;
        REZ = true;
        // Correction the number of particles in the deck
        I = KT + KQUESE[4];
        BnkMemb[I - 1] = BnkMemb[I - 1] - 1;
        if (BnkMemb[I - 1] == 0) {
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
        if (KQUES[N + 3] > ENV_0_I64) {
            MR = BnkMemb[KT + 2];
            QUES[N + 2] = QUES[N + 2] - RM;
            QueueWeight[NQUE] = QueueWeight[NQUE] - RM;
        }
        else {
            QUES[N + 2] = ENV_0_I64;
            QueueWeight[NQUE] = ENV_0_R64;
        }
        // Write to formal parameters
        for (I = 1; I < LSTATE; I++) {
            MR = BnkMemb[KT + I];
            EC[I] = RM;
        }
        KT = KT + LSTATE;
        for (int i = 0; i < NIRP; i++) {
            MEV[i] = BnkMemb[KT + i];
        }
    }
    
    //  Store one particles deck of length NPINDC to queue number NQUE.
    void storeDeck(const INT32 NQUE, bool REZ, REAL64 *EC, INT32 *MEV, const INT64 NPINDC, const char STDK)
    {
        STRING PPC;
        char C;
        INT64 I, KE, KQ, KT, N, NPINPA;
        INT64 *WTT;
        INT64 MR;
        REAL64 RM;
        int *locKQUES;
        REAL64 *QUES = new REAL64[LHEADS];
        locKQUES = &QUES;
        // STDK = 'B' means setting to the head of the queue
        C = STDK;
        NPINPA = NPINDC;
        N = NQUE;
        if ((NMQUE < N) || (N < 1)) {
            PPC = "STORE";
            if (C == ' ') PPC = "FETCH";
            error_message[0] = { "ERROR: at call subroutine '" + PPC + C + "' bad NQUE : " + std::to_string(NQUE) };
            ENV_StopByError(-2001, error_message);
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
                if (KQUESE[1] >= 2 * 27) {
                    // Increasing 
                    REZ = false;
                    return;
                }
            }
            // Correction KQUESE[6]
            KQ = KQUESE[1] / KQUESE[4];
            KQUESE[1] = KQUESE[1] * 2;
            KQUESE[6] = KQUESE[1] / KQUESE[4] - (KQ - KQUESE[6]);
            INT64* WTT = new INT64[KQUESE[2]];
            for (int i = 0; i < KQUESE[2]; i++) {
                WTT[i] = BnkMemb[i];
            }
            delete BnkMemb;
            INT64* BnkMemb = new INT64[BnkMemb[KQUESE[1]]];
            for (int i = 0; i < KQUESE[2]; i++) {
                BnkMemb[i] = WTT[i];
            }
            for (int i = 0; i < KQUESE[1]; i++) {
                BnkMemb[i + KQUESE[2]] = ENV_0_I64;
            }
            delete WTT;
        }
        KT = KQUESE[2] + ENV_1_I64;
        KQUESE[2] = MR;
    }
	
	
    void HDFWrite();
	
	
    void HDFRead();
	
	
    void pack();
	
	
    void MPI_Normalize();
	
	
    void MPI_Equalizer();
	
	
    void destroy();
    
    //
    void bnkFinal();
    
    //
    void create(const STRING& name, const INT32 MPI_MODE, const INT32 NMQUE)
    {
        VECTOR<STRING> error_message = { "BANK ERROR:", "", "" };
        INT32 ierr;
        this->NMQUE = NMQUE;
        this->MPI_MODE = MPI_MODE;
        nKQUESE = 7;
        LHEADS = this->NMQUE * 4;
        LMQEXT = LHEADS + nKQUESE;

        INT64 *KQUESE = new INT64[nKQUESE];
        *KQUESE = ENV_0_I64;
        INT64 *KQUES = new INT64[LHEADS];
        *KQUES = ENV_0_I64;
        REAL64 *QueueWeight = new REAL64[NMQUE];
        *QueueWeight = ENV_0_R64;

        KQUESE[0] = pow(2, 18);
        KQUESE[4] = NIRP; 
        KQUESE[3] = KQUESE[4] + LSTATE + 2;
        KQUESE[2] = ENV_0_I64;
        KQUESE[5] = KQUESE[0] / KQUESE[3];
        KQUESE[6] = ENV_0_I64;
        
        INT64 *BnkMemb = new INT64[KQUESE[1]];
        *BnkMemb = ENV_0_I64;
        
        this->name = name;

        if (this->MPI_MODE == 0) return;

        // ENV_MPI_Comm_size(MPI_SIZE);
        // ENV_MPI_Comm_rank(MPI_ID);
        try
        {
            REAL64* MPI_BNK_WEIGHTS = new REAL64[MPI_SIZE];
        }
        catch (const std::bad_alloc& e)
        {
            error_message[1] = " Error in bnkCreate (while allocating MPI_BNK_WEIGTHS).";
            error_message[2] = " error #" + std::to_string(ierr);
            ENV_StopByError(-3001, &error_message);
        }
    }
};



