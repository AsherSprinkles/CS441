//---------------------------------------------------------------------
// Author: Clay Sprinkles
// For: CS 441 Compilers class at UKY.
// About: This program is an implementation of a string table designed
// to have a relatively low collision rate.
//---------------------------------------------------------------------

#include "proj2.h"
//---------------------------------------------------------------------
//                      StringTable::StringTable
//---------------------------------------------------------------------
StringTable::StringTable() {
    for (size_t i = 0; i < STRTBL_NUM_BUCKETS; i++) {
        bucket[i] = NULL;
    }
}
//---------------------------------------------------------------------
//                      StringTable::~StringTable()
//---------------------------------------------------------------------
StringTable::~StringTable() {
    destruct();
}
//---------------------------------------------------------------------
//                      StringTable::insert()
//---------------------------------------------------------------------
StringTableRef StringTable::insert(string item) {
    StringTableRef insertedNode;
    insertedNode = search(item);
    if (insertedNode != NULL) { // string was found
        return insertedNode;
    }
    else {
        (void)hash(item);
        StringTableRef head = bucket[hashVal];
        if (head == NULL) { // bucket is empty
            head = new StringTableEntry;
            head->data = item;
            bucket[hashVal] = head;
            insertedNode = head;
        }
        else {
            StringTableRef tail = head;
            while (tail->next != NULL) { // traverses past the end of the list
                tail = tail->next;
            }
            tail->next = new StringTableEntry;
            tail = tail->next;
            tail->data = item;
            insertedNode = tail;
            numCollisions += 1;
        }
        numEntries += 1;
    }
    return insertedNode;
}
//---------------------------------------------------------------------
//                      StringTable::search()
//---------------------------------------------------------------------
// returns pointer to a StringTableEntry if found, otherwise returns NULL.
StringTableRef StringTable::search(string searchName) {
    StringTableRef current;
    (void)hash(searchName);
    current = bucket[hashVal];
    if (current == NULL) { // not found
        return NULL;
    }
    while (current != NULL) {
        if (current->data == searchName) {
            return current;
        }
        else
            current = current->next;
    }
    return NULL;
}
//---------------------------------------------------------------------
//                      StringTable::search()
//---------------------------------------------------------------------
string StringTable::search(StringTableRef ref) {
    if (ref) {
        return ref->data;
    }
    else return "";
}
//---------------------------------------------------------------------
//                      StringTable::print()
//---------------------------------------------------------------------
void StringTable::print() {
    cout << "STRING TABLE:" << endl;
    for (size_t i = 0; i < STRTBL_NUM_BUCKETS; i++) {
        StringTableRef current = bucket[i];
        if (current != NULL) {
            cout << "[" << setw(4) << i << "]:\t" << bucket[i]->data << endl;
            current = current->next;
            while (current != NULL) {
                cout << "       \t" << current->data << endl;
                current = current->next;
            }
        }
    }
    if (numEntries > 0) {
    cout << "Collision Percentage: " << setprecision(2) <<
        ((float)numCollisions/(float)numEntries)*PERCENTAGE_MULTIPLIER << "%" << endl;
    }
}
//---------------------------------------------------------------------
//                      StringTable::destruct()
//---------------------------------------------------------------------
void StringTable::destruct() {
    for (size_t i = 0; i < STRTBL_NUM_BUCKETS; i++) {
        StringTableRef head = bucket[i];
        StringTableRef current;
        while (head != NULL) {
            current = head->next;
            delete head;
            head = current;
        }
        bucket[i] = NULL;
    }
    numCollisions = 0;
    numEntries = 0;
}
//---------------------------------------------------------------------
//                      StringTable::hash()
//---------------------------------------------------------------------
int StringTable::hash(string item) {
	int seed = STRTBL_NUM_BUCKETS + RANDOMSEED; // helps make the table more random
	// makes hash likely to be different for strings with differing chars,
	// and guaranteed to be different for strings with 1 char.
	for (size_t i = 0; i < item.length(); i++) {
		seed += item[i];
		seed *= item[i]; // helps spread distribution
	}
	// allows strings that are permutations of one another to be different.
	// ex. abc, cba, bac
	for (size_t i = 0; i < item.length(); i++) {
		seed += (i+1) ^ item[i];
	}
	if (seed < 0) {
		seed = -seed;
	}
    hashVal = seed % STRTBL_NUM_BUCKETS;
	return hashVal;
}
//---------------------------------------------------------------------
//                              name
//---------------------------------------------------------------------
int main(int argc, char** argv) {
	{ // extra block to test the destructor
		StringTable t;
		string filename, aline, cmd;
		char ccmd;
		StringTableRef p;

		// open the input file
		if (argc < 2) return 0;
		filename = argv[1];
		ifstream f;
		f.open(filename);
		if (f.fail()) return 0;

		// read test data and insert into string table, one line per item
		while (!f.eof() && !f.fail()) {
			getline(f, aline);
			t.insert(aline);
		}
		f.close();
		t.print();

		ccmd = '?';
		while (ccmd != 'X' && ccmd != 'E') {
			cout << "Insert, Search, Print or Exit (I,S,P,X): ";
			getline(cin, cmd);
			ccmd = toupper(cmd[0]);
			switch (ccmd) {
			case 'I': cout << "Enter string: ";
				getline(cin, aline);
				t.insert(aline);
				break;
			case 'S': cout << "Enter string: ";
				getline(cin, aline);
				p = t.search(aline);
				if (p) {
					cout << "Found search 1: " << p->data << endl;
					aline = t.search(p);
					cout << "Found search 2: " << aline << endl;
				}
				else
					cout << "Not found\n";
				break;
			case 'P': t.print(); break;
			case 'X': cout << "Testing destruct...\n";
				      t.destruct();
					  t.print();
					  break;
			default:  cout << "Invalid input. Enter I,S,P or X\n";
			}
		}
	}
	return 0;
}
