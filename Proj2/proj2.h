//---------------------------------------------------------------------
// Author: Clay Sprinkles
// For: CS 441 Compilers class at UKY.
// About: This program is an implementation of a string table designed
// to have a relatively low collision rate.
//---------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
const int STRTBL_NUM_BUCKETS = 1000;
const int PERCENTAGE_MULTIPLIER = 100;
const int RANDOMSEED = 42938;

// a node in a linked list
struct StringTableEntry {
	std::string data;
	StringTableEntry* next = NULL;
};
typedef StringTableEntry* StringTableRef;

class StringTable {
	public:
		StringTable();
		~StringTable();
		StringTableRef insert(string item);
		StringTableRef search(string searchName);
		string search(StringTableRef ref);
		void print();
		void destruct();
	private:
		StringTableRef bucket[STRTBL_NUM_BUCKETS];
		int hash(string item);
		int hashVal;
		int numCollisions = 0;
		int numEntries = 0;
};
