#include <iostream>
using namespace std;

struct Node {
    int key;
    Node *next;
};

void init(Node *&head) {
    head = nullptr;
}

bool isEmpty(Node *head) {
    return head == nullptr;
}

bool addHead(Node *&head, int k) {
    Node *p = new Node;
    if (p == nullptr) {
        return false;
    }
    p->key = k;
    p->next = head;
    head = p;
    return true;
}

bool addLast(Node *&head, int k) {
    Node *p = new Node;
    if (p == nullptr) {
        return false;
    }
    if (isEmpty(head)) {
        return addHead(head, k);
    }
    Node *last = head;
    p->key = k;
    p->next = nullptr;
    while (last->next) {
        last = last->next;
    }
    last->next = p;
    return true;
}

bool deleteHead(Node *&head) {
    if (!isEmpty(head)) {
        Node *p = head;
        head = p->next;
        p->next = nullptr;
        delete p;
        return true;
    }
    return false;
}

bool deleteLast(Node *&head) {
    if (!isEmpty(head)) {
        Node *last = head;
        Node *prev = nullptr;
        while (last->next != nullptr) {
            prev = last;
            last = last->next;
        }
        prev->next = nullptr;
        last->next = nullptr;
        delete last;
        return true;
    }
    return false;
}

void output(Node *head) {
    while (!isEmpty(head)) {
        cout << head->key << ' ';
        head = head->next;
    }
    cout << '\n';
}