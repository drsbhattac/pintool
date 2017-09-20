#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>


// Author: Sukriti Bhattacharya
// This pintool generates memory state traces.


ofstream OutFile;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "context.out", "specify output filename");
ADDRINT reg_s[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

UINT32 Usage()
{
	cout << "Print Program State" << endl;
	cout << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}


static VOID LogInstruction(const CONTEXT *ctxt)
{
	REG regList[] = { REG_EDI, REG_ESI, REG_EBP, REG_ESP, REG_EBX, 
  			  REG_EDX, REG_ECX, REG_EAX, REG_SEG_CS, REG_SEG_SS, 
  			  REG_SEG_DS, REG_SEG_ES, REG_SEG_FS, REG_SEG_GS, 
  			  REG_EFLAGS };

	

	for (unsigned i = 0; i < 15; i++) {
		REG reg = regList[i];
		int a = PIN_GetContextReg(ctxt, reg);
		//OutFile <<REG_StringShort(reg).c_str() <<"=" <<std::bitset<32>(a)<<" " /*- reg_s[i]*/;
		OutFile <</*REG_StringShort(reg).c_str() <<"=" <<*/std::bitset<32>(a)<<" ";
		//OutFile << reg_s[i];
		//reg_s[i]=  PIN_GetContextReg(ctxt, reg);
		 
	}
	OutFile << endl;
}

VOID TraceInstructions(INS ins, VOID *arg)
{
	
	INS_InsertCall(ins, IPOINT_BEFORE,
			(AFUNPTR) LogInstruction,
			IARG_CONTEXT, // Handle to access a context (architectural state).
			IARG_END);
}

VOID Fini(INT32 code, VOID *v)
{
	 OutFile.close();
}


int main(int argc, char **argv)
{
	PIN_SetSyntaxXED();
	PIN_InitSymbols();

	if (PIN_Init(argc, argv)) return Usage();
	
	 OutFile.open(KnobOutputFile.Value().c_str());

	INS_AddInstrumentFunction(TraceInstructions, 0);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}