#include <iostream>
#include <string>
#include <unordered_set>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cmath>
#include <stack>
#include <iomanip>
#include <limits>

using namespace std;

class Pair {
private:
    unsigned int key;
    string value;

public:
    Pair(string fileName){
        this->value = fileName;
        key = this->calculateHash(fileName);
    }

    unsigned int calculateHash(string file){
        unsigned long long sum = 0;
        ifstream File(file);
        if(!File) {
            for(char c : file) sum += (unsigned char)c;
        }
        else {
            char c;
            while(File.get(c)) 
                sum += (unsigned char)c;
        }
        unsigned int hash = (unsigned int)(sum % (UINT32_MAX + 1ULL));
        return hash;
    }

    unsigned int getKey(){ return key; } 
    string getValue(){ return value; } 
};

class LinkedNode {
private:
    Pair* data;
    LinkedNode* next;

public:
    LinkedNode(){
        data = NULL;
        next = NULL;
    }

    LinkedNode* getNext(){ return next; }
    void setNext(LinkedNode* n){ next = n; }   
    void setPair(Pair* p){ data = p; }
    Pair* getPair(){ return data; }
};

class TreeNode {
private:
    TreeNode* left;
    LinkedNode* data;
    TreeNode* right;

public:
    TreeNode(){
        left = right = NULL;
        data = NULL; 
    }

    LinkedNode* getData(){ return data; }
    TreeNode* getLeft(){ return left; }
    TreeNode* getRight(){ return right; }

    void setData(LinkedNode* d){ data = d; }
    void setLeft(TreeNode* l){ left = l; }
    void setRight(TreeNode* r){ right = r; }
};

class Tree {
private:
    TreeNode* root;

public:
    Tree(){ root = NULL; }

    void insertinTree(Pair* data){
        if(root == NULL){
            root = new TreeNode();
            LinkedNode* ln = new LinkedNode();
            ln->setPair(data);
            root->setData(ln);
            return;
        }

        TreeNode* curr = root;
        TreeNode* parent = NULL;

        while(curr != NULL){
            parent = curr;

            unsigned int currKey = curr->getData()->getPair()->getKey();
            unsigned int newKey  = data->getKey();

            if(newKey == currKey){
                LinkedNode* ln = curr->getData();
                while(ln->getNext() != NULL)
                    ln = ln->getNext();

                LinkedNode* newNode = new LinkedNode();
                newNode->setPair(data);
                ln->setNext(newNode);
                return;
            }

            if(newKey < currKey)
                curr = curr->getLeft();
            else
                curr = curr->getRight();
        }

        TreeNode* newNode = new TreeNode();
        LinkedNode* ln = new LinkedNode();
        ln->setPair(data);
        newNode->setData(ln);

        if(data->getKey() < parent->getData()->getPair()->getKey())
            parent->setLeft(newNode);
        else
            parent->setRight(newNode);
    }

    void inorderCollect(TreeNode* node, vector<Pair*>& out) {
        if(!node) return;
        inorderCollect(node->getLeft(), out);
        LinkedNode* ln = node->getData();
        while(ln) {
            out.push_back(ln->getPair());
            ln = ln->getNext();
        }
        inorderCollect(node->getRight(), out);
    }

    void clearTree(TreeNode* node) {
        if(!node) return;
        clearTree(node->getLeft());
        clearTree(node->getRight());
        LinkedNode* ln = node->getData();
        while(ln) {
            LinkedNode* nxt = ln->getNext();
            delete ln;
            ln = nxt;
        }
        delete node;
        node = nullptr;
    }

    vector<Pair*> extractAll() {
        vector<Pair*> files;
        inorderCollect(root, files);
        clearTree(root);
        root = NULL;
        return files;
    }
    
    void printContent(TreeNode* node) {
        if(!node) return;
        printContent(node->getLeft());
        LinkedNode* ln = node->getData();
        while(ln){
            cout << "    File: " << ln->getPair()->getValue() 
                 << " [Key: " << ln->getPair()->getKey() << "]" << endl;
            ln = ln->getNext();
        }
        printContent(node->getRight());
    }
    
    void printTree(){
        if(!root) cout << "    (Empty)" << endl;
        else printContent(root);
    }
};

struct RoutingEntry {
    unsigned int jumpSize;
    unsigned int targetID;
    void* machinePtr;
};

class Machine {
private:
    unsigned int id;
    vector<RoutingEntry> routingTable; 

public:
    static unordered_set<unsigned int> usedIDs;
    Tree* hardDisk;
    Machine* next;

    Machine() {
        next = nullptr;
        unsigned int tmp;
        do{
            tmp = (unsigned int) rand();
        }while(usedIDs.count(tmp));
        this->id = tmp;
        usedIDs.insert(tmp);
        hardDisk = new Tree();
    }

    unsigned int getID(){ return id; }
    void setNext(Machine* n){ next = n; }
    Machine* getNext(){ return next; }

    void addAPair(Pair* a){ hardDisk->insertinTree(a); }

    void forceID(unsigned int newID){
        if(usedIDs.count(newID)) {
        }
        usedIDs.erase(id);
        id = newID;
        usedIDs.insert(id);
    }
    
    void clearRoutingTable() {
        routingTable.clear();
    }
    
    void addRoutingEntry(unsigned int powerOfTwo, unsigned int tID, Machine* mPtr) {
        RoutingEntry re;
        re.jumpSize = powerOfTwo;
        re.targetID = tID;
        re.machinePtr = (void*)mPtr;
        routingTable.push_back(re);
    }
    
    Machine* getRoutingMachine(int index) {
        if(index >= 0 && index < routingTable.size())
            return (Machine*)routingTable[index].machinePtr;
        return nullptr;
    }
    
    int getRoutingTableSize() {
        return routingTable.size();
    }
    
    void printRoutingTable() {
        cout << "  Routing Table for Machine " << id << ":" << endl;
        cout << "  --------------------------------------------------" << endl;
        cout << "  | i | Jump (2^i) | Target ID       | Machine ID  |" << endl;
        cout << "  --------------------------------------------------" << endl;
        for(int i = 0; i < routingTable.size(); i++) {
            Machine* m = (Machine*)routingTable[i].machinePtr;
            cout << "  | " << setw(2) << i << " | " 
                 << setw(10) << routingTable[i].jumpSize << " | "
                 << setw(15) << routingTable[i].targetID << " | "
                 << setw(11) << (m ? to_string(m->getID()) : "NULL") << " |" << endl;
        }
        cout << "  --------------------------------------------------" << endl;
    }
    
    void printStorage() {
        cout << "Storage of Machine " << id << ":" << endl;
        hardDisk->printTree();
    }
};

unordered_set<unsigned int> Machine::usedIDs;

class Ring_DHT {
private:
    Machine* head;
    Machine* tail;
    unsigned int totalBits;

public:
    Ring_DHT(unsigned int num) {
        totalBits = 32; 
        head = tail = NULL;

        if(num > 0) {
            head = new Machine();
            Machine* temp = head;

            for(unsigned int i=1; i<num; i++){
                temp->setNext(new Machine());
                temp = temp->getNext();
            }

            tail = temp;
            tail->setNext(head);
            sortRing();
            updateAllRoutingTables();
        }
    }

    Machine* findMid(Machine* h) {
        Machine* slow = h;
        Machine* fast = h;
        while (fast && fast->getNext() && fast->getNext()->getNext()) {
            slow = slow->getNext();
            fast = fast->getNext()->getNext();
        }
        return slow;
    }

    Machine* mergeList(Machine* a, Machine* b){
        Machine* head = NULL;
        Machine* tail = NULL;

        auto attach = [&](Machine* node){
            if(head == NULL){
                head = tail = node;
            }
            else{
                tail->setNext(node);
                tail = node;
            }
        };

        while(a && b){
            if(a->getID() <= b->getID()){
                Machine* nxt = a->getNext();
                attach(a);
                a = nxt;
            }
            else{
                Machine* nxt = b->getNext();
                attach(b);
                b = nxt;
            }
        }
        while(a){
            Machine* nxt = a->getNext();
            attach(a);
            a = nxt;
        }
        while(b){
            Machine* nxt = b->getNext();
            attach(b);
            b = nxt;
        }
        tail->setNext(NULL);
        return head;
    }

    Machine* mergeSort(Machine* h){
        if(!h || !h->getNext()) return h;
        Machine* mid = findMid(h);
        Machine* left = h;
        Machine* right = mid->getNext();
        mid->setNext(NULL);
        left = mergeSort(left);
        right = mergeSort(right);
        return mergeList(left, right);
    }

    void sortRing(){
        if(!head) return;
        tail->setNext(NULL);
        head = mergeSort(head);
        Machine* temp = head;
        while(temp->getNext())
            temp = temp->getNext();
        tail = temp;
        tail->setNext(head);
    }

    Machine* getPrev(Machine* m){
        if(!head) return nullptr;
        Machine* curr = head;
        while(curr->getNext() != m && curr->getNext() != head)
            curr = curr->getNext();
        return curr;
    }

    void migrateAfterJoin(Machine* newM){
        Machine* prev = getPrev(newM); 
        vector<Pair*> files = prev->hardDisk->extractAll();

        for(Pair* p : files){
            unsigned int key = p->getKey();
            bool belongsToNew = false;
            
            if(prev->getID() < newM->getID()) {
                if(key > prev->getID() && key <= newM->getID()) belongsToNew = true;
            } else {
                if(key > prev->getID() || key <= newM->getID()) belongsToNew = true;
            }

            if(belongsToNew) newM->addAPair(p);
            else prev->addAPair(p);
        }
        
        Machine* succ = newM->getNext();
        if(succ && succ != newM) {
             vector<Pair*> succFiles = succ->hardDisk->extractAll();
             for(Pair* p : succFiles) {
                 unsigned int key = p->getKey();
                 unsigned int prevID = getPrev(newM)->getID();
                 bool inNewRange = false;
                 if(prevID < newM->getID()) {
                     if(key > prevID && key <= newM->getID()) inNewRange = true;
                 } else { 
                     if(key > prevID || key <= newM->getID()) inNewRange = true;
                 }
                 
                 if(inNewRange) newM->addAPair(p);
                 else succ->addAPair(p);
             }
        }
    }

    void migrateAfterLeave(Machine* removed, Machine* successor){
        vector<Pair*> files = removed->hardDisk->extractAll();
        for(Pair* p : files)
            successor->addAPair(p);
    }

    void joinMachine(unsigned int newID){
        if(Machine::usedIDs.count(newID)) {
            cout << "Error: Machine ID " << newID << " already exists. Cannot add." << endl;
            return;
        }

        Machine* newM = new Machine();
        newM->forceID(newID);

        if(!head) {
            head = newM;
            tail = newM;
            head->setNext(tail);
            updateAllRoutingTables();
            return;
        }

        Machine* curr = head;
        Machine* prev = tail;

        if(newID < head->getID()){
            newM->setNext(head);
            head = newM;
            tail->setNext(head);
            migrateAfterJoin(newM);
            updateAllRoutingTables();
            return;
        }

        do{
            if(curr->getID() >= newID){
                newM->setNext(curr);
                prev->setNext(newM);
                migrateAfterJoin(newM);
                updateAllRoutingTables();
                return;
            }
            prev = curr;
            curr = curr->getNext();
        }while(curr != head);

        prev->setNext(newM);
        newM->setNext(head);
        tail = newM;
        migrateAfterJoin(newM);
        updateAllRoutingTables();
    }

    void leaveMachine(unsigned int idToRemove){
        if(!head) return;

        Machine* curr = head;
        Machine* prev = tail;

        if(head->getID() == idToRemove){
            Machine* succ = head->getNext();
            migrateAfterLeave(head, succ);
            if(head == tail){
                delete head;
                head = tail = NULL;
                return;
            }
            prev->setNext(succ);
            delete head;
            head = succ;
            updateAllRoutingTables();
            return;
        }

        do{
            if(curr->getID() == idToRemove){
                Machine* succ = curr->getNext();
                migrateAfterLeave(curr, succ);
                prev->setNext(curr->getNext());
                if(curr == tail) tail = prev;
                delete curr;
                updateAllRoutingTables();
                return;
            }
            prev = curr;
            curr = curr->getNext();
        }while(curr != head);
        
        cout << "Machine ID not found." << endl;
    }

    void insertFileinDHT(string FileName){
        Pair* p = new Pair(FileName);
        unsigned int key = p->getKey();
        cout << "[System] Inserting file '" << FileName << "' with Hash Key: " << key << endl;

        stack<Machine*> path;
        Machine* responsibleMachine = searchMachine(key, path);

        if(responsibleMachine) {
            responsibleMachine->addAPair(p);
            cout << "[System] File stored in Machine " << responsibleMachine->getID() << endl;
        } else {
            cout << "[Error] Search returned NULL." << endl;
        }
    }
    
    Machine* findSuccessorOfID(unsigned int targetID) {
        if(!head) return nullptr;
        Machine* curr = head;
        
        if(targetID > tail->getID()) return head;
        
        do {
            if(curr->getID() >= targetID) return curr;
            curr = curr->getNext();
        } while(curr != head);
        
        return head; 
    }

    void updateAllRoutingTables() {
        if(!head) return;
        int tableSize = 16; 
        
        Machine* curr = head;
        do {
            curr->clearRoutingTable();
            for(int i = 0; i < tableSize; i++) {
                unsigned long long jump = (1ULL << i);
                unsigned long long target = ((unsigned long long)curr->getID() + jump);
                unsigned int wrappedTarget = (unsigned int)(target % (UINT32_MAX + 1ULL));
                
                Machine* succ = findSuccessorOfID(wrappedTarget);
                curr->addRoutingEntry((unsigned int)jump, wrappedTarget, succ);
            }
            curr = curr->getNext();
        } while(curr != head);
    }
    
    Machine* searchMachine(unsigned int key, stack<Machine*>& path) {
        if(!head) return nullptr;
        
        Machine* curr = head;
        path.push(curr);
        
        int steps = 0;
        int limit = 500; 

        while(steps < limit) {
            unsigned int currID = curr->getID();
            unsigned int succID = curr->getNext()->getID();
            
            bool isResponsible = false;
            
            if (currID < succID) {
                if (key > currID && key <= succID) isResponsible = true;
            } else {
                if (key > currID || key <= succID) isResponsible = true;
            }
            
            if (curr == curr->getNext()) isResponsible = true;

            if (isResponsible) {
                path.push(curr->getNext());
                return curr->getNext();
            }
            
            Machine* bestFinger = curr->getNext();
            
            int tblSize = curr->getRoutingTableSize();
            for(int i = tblSize - 1; i >= 0; i--) {
                Machine* finger = curr->getRoutingMachine(i);
                if(!finger) continue;
                
                unsigned int fID = finger->getID();
                
                bool insideInterval = false;
                
                if (currID < key) {
                    if (fID > currID && fID < key) insideInterval = true;
                } else {
                    if (fID > currID || fID < key) insideInterval = true;
                }
                
                if(insideInterval) {
                    bestFinger = finger;
                    break;
                }
            }
            
            if(bestFinger == curr) {
                bestFinger = curr->getNext();
            }

            curr = bestFinger;
            path.push(curr);
            steps++;
        }
        
        cout << "[Warning] Search limit reached. Ring might be unstable." << endl;
        return curr;
    }
    
    void printAllMachines(){
        if(!head) {
            cout << "Ring is empty." << endl;
            return;
        }
        Machine* temp = head;
        cout << "Ring Structure: ";
        do{
            cout << "[ID: " << temp->getID() << "] -> ";
            temp = temp->getNext();
        }while(temp != head);
        cout << "[HEAD]" << endl;
    }
    
    void printSpecificRoutingTable(unsigned int id) {
        if(!head) return;
        Machine* temp = head;
        bool found = false;
        do {
            if(temp->getID() == id) {
                temp->printRoutingTable();
                found = true;
                break;
            }
            temp = temp->getNext();
        } while(temp != head);
        
        if(!found) cout << "Machine ID " << id << " not found." << endl;
    }
    
    void printSpecificStorage(unsigned int id) {
        if(!head) return;
        Machine* temp = head;
        bool found = false;
        do {
            if(temp->getID() == id) {
                temp->printStorage();
                found = true;
                break;
            }
            temp = temp->getNext();
        } while(temp != head);
        
        if(!found) cout << "Machine ID " << id << " not found." << endl;
    }
};

void printMenu() {
    cout << "\n========================================" << endl;
    cout << "     DISTRIBUTED HASH TABLE SIMULATION" << endl;
    cout << "========================================" << endl;
    cout << "1. Print Ring Structure" << endl;
    cout << "2. Add Machine" << endl;
    cout << "3. Remove Machine" << endl;
    cout << "4. Upload File" << endl;
    cout << "5. Search Machine (by Key/File) & Show Path" << endl;
    cout << "6. View Routing Table of a Machine" << endl;
    cout << "7. View Storage of a Machine" << endl;
    cout << "0. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Select Option: ";
}

int main(){
    srand(time(0));

    unsigned int noOfMachines;
    cout << "IDENTIFIER SPACE SIZE : 32 BITS\n";
    cout << "MAX NUMBER OF MACHINES : 2^32\n";
    cout << "Enter Number of Initial Machines : ";
    if(!(cin >> noOfMachines)){
        cout << "Invalid input. Defaulting to 5." << endl;
        noOfMachines = 5;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Ring_DHT DHT(noOfMachines);
    
    int choice;
    while(true) {
        printMenu();
        if(!(cin >> choice)){
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if(choice == 0) {
            cout << "Exiting..." << endl;
            break;
        }

        switch(choice) {
            case 1: {
                DHT.printAllMachines();
                break;
            }
            case 2: {
                unsigned int id;
                cout << "Enter new Machine ID: ";
                cin >> id;
                DHT.joinMachine(id);
                cout << "Machine " << id << " added." << endl;
                break;
            }
            case 3: {
                unsigned int id;
                cout << "Enter Machine ID to remove: ";
                cin >> id;
                DHT.leaveMachine(id);
                cout << "Machine removed (if it existed)." << endl;
                break;
            }
            case 4: {
                string fname;
                cout << "Enter Filename to upload: ";
                cin >> fname;
                DHT.insertFileinDHT(fname);
                break;
            }
            case 5: {
                int searchType;
                cout << "Search by: 1. File Name  2. Raw Key ID: ";
                cin >> searchType;
                
                unsigned int key;
                if(searchType == 1) {
                    string f;
                    cout << "Enter filename: ";
                    cin >> f;
                    Pair temp(f);
                    key = temp.getKey();
                    cout << "Calculated Key: " << key << endl;
                } else {
                    cout << "Enter Key: ";
                    cin >> key;
                }
                
                stack<Machine*> pathStack;
                Machine* res = DHT.searchMachine(key, pathStack);
                
                if(res) {
                    cout << "\n[Success] Key " << key << " is managed by Machine " << res->getID() << endl;
                    
                    cout << "Path/Succession taken (Hops):" << endl;
                    vector<unsigned int> pathVec;
                    while(!pathStack.empty()){
                        pathVec.push_back(pathStack.top()->getID());
                        pathStack.pop();
                    }
                    
                    cout << "Start ";
                    for(int i = pathVec.size() - 1; i >= 0; i--) {
                        cout << "-> [ID: " << pathVec[i] << "] ";
                    }
                    cout << "(Target)" << endl;
                } else {
                    cout << "Search failed." << endl;
                }
                break;
            }
            case 6: {
                unsigned int id;
                cout << "Enter Machine ID: ";
                cin >> id;
                DHT.printSpecificRoutingTable(id);
                break;
            }
            case 7: {
                unsigned int id;
                cout << "Enter Machine ID: ";
                cin >> id;
                DHT.printSpecificStorage(id);
                break;
            }
            default:
                cout << "Invalid Option." << endl;
        }
    }

    return 0;
}
