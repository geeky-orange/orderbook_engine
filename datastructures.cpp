#include <iostream>
#include <map>
#include <vector>


using namespace std;



int main() {

    /* ------- STACK -------------
    // Stacks = LIFO Datastruture
    // push() add to top
    // pop() remove from last    
    ------------------------------
    */



    


       /* ------- Linked List -------------


    ------------------------------
    */
    struct Node
    {
        int data;
        Node* next;
    };

        for (int i = 0; i < 10; ++i)
    {
        cout << i << endl;
        
        if (i%2 == 0){
            cout << "Even Number" << endl;

        }

        else if (i == 3) {
            cout << "Three" << endl;
        }
        
        else{
            cout << "Odd" << endl;
        }
        
    }

    int arr[5] = {1,2,3,4};

    cout << sizeof(arr)/sizeof(arr[0]) << endl;

    int a = 4;
    int *b = &a;
    int *head = arr;

    for (int i = 0; i < 3; i++)
    {
        head = arr + i;
        cout << head << endl;
        cout << *head << endl;

    }
    

    cout << a << endl;
    cout << b << endl;
    cout << *b << endl; // Derefrecing the pointer

    /*
    Tuples
    */

    tuple <int,string> person(20,"Mubeen");
    cout << get<0>(person) << endl;
    get<0>(person) = 30;
    cout << get<0>(person) << endl;

    tuple <int,float,char> thing;
    thing = make_tuple(2,2.3,'a');

    cout << get<1>(thing) << endl;


    map<string,int> mp = {
        {"A",5},
        {"B",4},
        {"C",1}
    };

    mp["D"] = 89;

    cout << mp["A"] << endl;
    cout << mp["D"] << endl;
    
    pair<string,int> p1("E",34);
    mp.insert(p1);

    cout << mp["E"] << endl;
    cout << mp.size() << endl;

    for (auto iter = mp.begin(); iter!= mp.end(); ++iter)
    {
        cout << iter->first << " " << iter->second << endl;
    }
    
    for (const auto& [key,value] : mp)
    {
        cout << key << " " << value << endl;
    }

    for (map<string,int>::iterator  iter = mp.begin(); iter != mp.end(); ++iter)
    {
        cout << iter->first << endl;
    }
    
    vector<int> newArr = {1,4,6,7,8};
    newArr.push_back(5);
    cout << newArr.size() << endl;

    for (int newA : newArr){
        cout << newA << endl;
    }
    

    




    
    return 0;
}