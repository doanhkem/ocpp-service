// Limit the number of instances

#include <iostream>
#include <string>
using namespace std;

#define MAX 2

class Singleton {
   public:
    string name;
    static int instance_count;
    static Singleton *GetInstance() {
        if (instance_count >= MAX)
            return NULL;
        Singleton *singleton = new Singleton();
        instance_count++;
        return singleton;
    }

   private:
    Singleton() = default;
};

int Singleton::instance_count = 0;

int main() {
    Singleton *s1 = Singleton::GetInstance();
    Singleton *s2 = Singleton::GetInstance();
    Singleton *s3 = Singleton::GetInstance();
    if (s3 == NULL)
        cout << "s3 is NULL" << endl;
    s1->name = "aaa";
    s2->name = "bbb";
    cout << s1->name << endl;
}