   //
   //
   //
   #pragma once
   #include<vector>

   struct RecordDescriptorEntry
   {
        RecordDescriptorEntry(int const index, int const order, int const entries);
        int recordIndex;
        int numCoefficient;
        int numEntries;
   };

   typedef std::vector<RecordDescriptorEntry> RecordDescriptor;
