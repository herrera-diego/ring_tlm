#ifndef CPU_H
#define CPU_H

#include <systemc.h>

#define SIZE 3

SC_MODULE (cpu)
{
    // -----------------------------------------> Module Definition
    // Inputs
    sc_in_clk       clk;
    

    // -----------------------------------------> Variables
    

    // -----------------------------------------> Methods
    void tracing(sc_trace_file *tf);
    void readMem();
    

    // -----------------------------------------> Constructor
    SC_HAS_PROCESS(cpu);


    cpu(sc_module_name name_):
    sc_module(name_)
    {        
        SC_THREAD(readMem);
        sensitive << clk;
}

};  // End of Module

#endif