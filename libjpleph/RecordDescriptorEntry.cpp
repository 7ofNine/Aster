//
//
//
#include "RecordDescriptor.h"

RecordDescriptorEntry::RecordDescriptorEntry(int const index, int const order, int const entries)
{
    recordIndex     = index;
    numCoefficient  = order;
    numEntries      = entries;
}
