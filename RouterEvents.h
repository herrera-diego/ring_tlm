#ifndef ROUTER_EVENTS_H
#define ROUTER_EVENTS_H

#include <systemc.h>   

// User-defined extension class
class RouterEvents 
{
  public:
    
    static sc_event myEvent;

    static void myNotify()
    {
      myEvent.notify();
    }
};

#endif
