#include "pin.H"
#include <iostream>
#include <fstream>
#include<string.h>
//Author: Sukriti Bhattacharya
//pintool: print the indirect jump trace


ofstream OutFile;
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "jmpTr.out", "specify output filename");

UINT32 Usage()
{
	cout << "Print indirect jump trace" << endl;
	cout << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}
void printjmp(const string *s)
{
	OutFile.write(s->c_str(), s->size());
}
VOID Trace(TRACE trace, VOID *v)
{
    
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS tail = BBL_InsTail(bbl);
		string traceString = "";
		traceString += INS_Disassemble(tail) + "\n";
		string str1("ret");
		string str2("call");

		if((traceString.find(str1)== string::npos && traceString.find(str2)== string::npos)){
			if(INS_IsIndirectBranchOrCall(tail))
			{		
				INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(printjmp),
                           IARG_PTR, new string(traceString), IARG_END);
			}
		}
    }
}

VOID Fini(INT32 code, VOID *v)
{
	 OutFile.close();
}



int  main(int argc, char *argv[])
{

    PIN_InitSymbols();
	OutFile.open(KnobOutputFile.Value().c_str());
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();

    return 0;
}
