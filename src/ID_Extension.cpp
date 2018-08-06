#include "ID_Extension.h"

ID_extension::ID_extension(void)
{
    transaction_id = 0;
}

tlm::tlm_extension_base* ID_extension::clone() const
{
      ID_extension* t = new ID_extension;
      t->transaction_id = this->transaction_id;
      return t;
}

void ID_extension::copy_from(tlm::tlm_extension_base const &ext)
{
    transaction_id = static_cast<ID_extension const &>(ext).transaction_id;
}
