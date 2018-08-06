#ifndef ID_EXTENSION_H
#define ID_EXTENSION_H

#include <systemc.h>    
#include "tlm.h"

// User-defined extension class
class ID_extension : public tlm::tlm_extension<ID_extension> 
{
    public:
        ID_extension();

        // Must override pure virtual methods
        virtual tlm::tlm_extension_base* clone() const;
        virtual void copy_from(tlm::tlm_extension_base const &ext);

        unsigned int transaction_id;
};

#endif
