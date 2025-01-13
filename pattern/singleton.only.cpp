// Allow only 1 instance:
#include <iostream>
#include <string>
using namespace std;

class Singleton {
   public:
    string name;
    static Singleton* GetInstance() {
        static Singleton* singleton = new Singleton();
        return singleton;
    }

   private:
    Singleton() = default;
};

int main() {
    Singleton* s1 = Singleton::GetInstance();
    Singleton* s2 = Singleton::GetInstance();
    s1->name = "aaa";
    s2->name = "bbb";
    cout << s1->name << endl;
}
