#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Charger {
   public:
    Charger() = default;

    virtual int getById() = 0;

    virtual void executeInstance() = 0;
};

class Connector : public Charger {
   private:
    int connectorId;

   public:
    Connector() = default;
    Connector(int id) {
        this->connectorId = id;
    }
    ~Connector() = delete;

    int getById() override {
        return this->connectorId;
    }

    void executeInstance() override {
        cout << "Executed" << '\n';
    }
};

int32_t main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    vector<Charger *> charger = {new Connector(1), new Connector(2), new Connector(3)};
    for (auto item : charger) {
        item->executeInstance();
    }
}