#include <iostream>
#include <string>
#include <unordered_set>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <vector>
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
        if(!File) cout << "Unable to open file " << file << endl;
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
        return files;
    }
};

class Machine {
private:
    unsigned int id;
    static unordered_set<unsigned int> usedIDs;

public:
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
        usedIDs.erase(id);
        id = newID;
        usedIDs.insert(id);
    }
};

unordered_set<unsigned int> Machine::usedIDs;





class Ring_DHT {
private:
    Machine* head;
    Machine* tail;

public:
    Ring_DHT(unsigned int num) {
        if(num == 0){
            head = tail = NULL;
            return;
        }

        head = new Machine();
        Machine* temp = head;

        for(unsigned int i=1; i<num; i++){
            temp->setNext(new Machine());
            temp = temp->getNext();
        }

        tail = temp;
        tail->setNext(head);

        sortRing();
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
        tail->setNext(NULL);
        head = mergeSort(head);

        Machine* temp = head;
        while(temp->getNext())
            temp = temp->getNext();

        tail = temp;
        tail->setNext(head);
    }

    Machine* getPrev(Machine* m){
        Machine* curr = head;
        while(curr->getNext() != m)
            curr = curr->getNext();
        return curr;
    }

    void migrateAfterJoin(Machine* newM){
        Machine* prev = getPrev(newM);

        vector<Pair*> files = prev->hardDisk->extractAll();

        for(Pair* p : files){
            unsigned int key = p->getKey();

            if(key > prev->getID() && key <= newM->getID())
                newM->addAPair(p);
            else
                prev->addAPair(p);
        }
    }

    void joinMachine(unsigned int newID){
        Machine* newM = new Machine();
        newM->forceID(newID);

        Machine* curr = head;
        Machine* prev = tail;

        if(newID < head->getID()){
            newM->setNext(head);
            head = newM;
            tail->setNext(head);
            migrateAfterJoin(newM);
            return;
        }

        do{
            if(curr->getID() >= newID){
                newM->setNext(curr);
                prev->setNext(newM);
                migrateAfterJoin(newM);
                return;
            }
            prev = curr;
            curr = curr->getNext();
        }while(curr != head);

        prev->setNext(newM);
        newM->setNext(head);
        tail = newM;

        migrateAfterJoin(newM);
    }

    void migrateAfterLeave(Machine* removed, Machine* successor){
        vector<Pair*> files = removed->hardDisk->extractAll();
        for(Pair* p : files)
            successor->addAPair(p);
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
            return;
        }

        do{
            if(curr->getID() == idToRemove){
                Machine* succ = curr->getNext();
                migrateAfterLeave(curr, succ);

                prev->setNext(curr->getNext());
                if(curr == tail)
                    tail = prev;

                delete curr;
                return;
            }
            prev = curr;
            curr = curr->getNext();
        }while(curr != head);
    }

    void insertFileinDHT(string FileName){
        Pair* p = new Pair(FileName);
        unsigned int key = p->getKey();

        Machine* curr = head;

        while(curr != tail && curr->getID() < key)
            curr = curr->getNext();

        curr->addAPair(p);
    }

    void printAllMachines(){
        if(!head) return;

        Machine* temp = head;
        do{
            cout << temp->getID() << endl;
            temp = temp->getNext();
        }while(temp != head);
    }
};




int main(){
    srand(time(0));

    unsigned int noOfMachines;
    cout << "IDENTIFIER SPACE SIZE : 32 BITS\n";
    cout << "MAX NUMBER OF MACHINES : 2^32\n";
    cout << "Enter Number of Machines : ";
    cin >> noOfMachines;

    Ring_DHT DHT(noOfMachines);

    cout << "\nMachines in ring:" << endl ;
    DHT.printAllMachines();


    DHT.insertFileinDHT("testing.txt");



    return 0;
}
