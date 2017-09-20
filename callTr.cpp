#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
//author: Sukriti Bhattacharya
//pintool: print the system call trace
ofstream OutFile;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "callTr.out", "specify output filename");

void callCallback (ADDRINT instruction_address, ADDRINT target_address)
{
    PIN_LockClient();
    RTN rtn = RTN_FindByAddress(target_address);
    if (RTN_Valid(rtn)) {
        std::string symbolName = RTN_Name(rtn);
        symbolName = PIN_UndecorateSymbolName(symbolName, UNDECORATION_COMPLETE);
         //OutFile << instruction_address << " => " << symbolName << endl;
		OutFile <<symbolName << endl;
    }
    else {
        // OutFile << std::hex << instruction_address << " => " << target_address << endl;
		OutFile << std::hex << target_address << endl;
	}
    PIN_UnlockClient();
}


void InstructionInstrumentation (INS ins, void * v)
{
    if (INS_IsCall(ins)) {
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR) callCallback,
                       IARG_INST_PTR,
                       IARG_BRANCH_TARGET_ADDR,
                       IARG_END);
    }
}

UINT32 Usage()
{
	cout << "Print call trace" << endl;
	cout << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}

VOID Fini(INT32 code, VOID *v)
{
	 OutFile.close();
}


int main(int argc, char *argv[])
{
   
    if (PIN_Init(argc, argv)) return Usage();
	
	OutFile.open(KnobOutputFile.Value().c_str());

    PIN_InitSymbols();

    INS_AddInstrumentFunction(InstructionInstrumentation, NULL);
	PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}
