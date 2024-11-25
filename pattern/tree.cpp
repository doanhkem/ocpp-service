#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

const int LIM = 4;

template <class T = int>
class Tree {
   public:
    struct Node {
        T key;
        Node *pLeft;
        Node *pRight;
        Node *pParent;

        Node() = default;
        Node(T x) : key(x), pLeft(nullptr), pRight(nullptr), pParent(nullptr) {}
    };

    bool insertNodeToTree(Node *(&root), T k) {
        return insertNode(root, k, root);
    }

    bool insertNode(Node *(&root), T k, Node *parent) {
        if (root == nullptr || root->key == k) {
            return insert(root, k, parent);
        }
        if (root->key == k) {
            return false;
        } else if (root->key > k) {
            return this->insertNode(root->pLeft, k, root);
        } else {
            return this->insertNode(root->pRight, k, root);
        }
    }

    bool insert(Node *(&root), T k, Node *parent) {
        Node *p = new Node(k);
        if (p == nullptr) {
            return false;
        }
        p->pParent = parent;
        root = p;
        return true;
    }

   public:
    Node *root = nullptr;

    bool removeNode(Node *(&root), int k) {
        Node *tmp = searchTree(root, k);
        if (tmp == nullptr) {
            return false;
        }
        return remove(tmp);
    }

    Node *searchTree(Node *root, int k) {
        if (root == nullptr || root->key == k) {
            return root;
        } else if (root->key > k) {
            return searchTree(root->pLeft, k);
        } else {
            return searchTree(root->pRight, k);
        }
    }
    
    bool remove(Node *(&root)) {
        if (root->pLeft == nullptr && root->pRight == nullptr) {  // Leaf
            if (root->key < (root->pParent)->key) {
                (root->pParent)->pLeft = nullptr;
            } else {
                (root->pParent)->pRight = nullptr;
            }
            root = nullptr;
            delete root;
        } else if (root->pLeft == nullptr || root->pRight == nullptr) {  // 1 Sub tree
            if (root->key < (root->pParent)->key) {
                (root->pParent)->pLeft = root->pRight ?: root->pLeft;
                if (root->pLeft) {
                    (root->pLeft)->pParent = root->pParent;
                } else {
                    (root->pRight)->pRight = root->pParent;
                }
            } else {
                (root->pParent)->pRight = root->pLeft ?: root->pRight;
                if (root->pLeft) {
                    (root->pLeft)->pParent = root->pParent;
                } else {
                    (root->pRight)->pParent = root->pParent;
                }
            }
            root = nullptr;
            delete root;
        } else {  // 2 Sub + Root
            Node *successor = findSuccessor(root);
            root->key = successor->key;
            return removeNode(root->pRight, successor->key);
        }
        return true;
    }

    Node *findSuccessor(Node *tmp) {
        tmp = tmp->pRight;
        while (tmp->pLeft) {
            tmp = tmp->pLeft;
        }
        return tmp;
    }

    void convertToTree(Node *(&root), vector<int> a) {
        int len = (int)a.size(), mid = len >> 1, left = len & 1 ? mid : mid - 1, right = mid;
        while (left >= 0 || right < len) {
            insertNodeToTree(root, a[left]);
            insertNodeToTree(root, a[right]);
            --left;
            ++right;
        }
    }

    void traversal(Node *root) {
        if (root) {
            if (root->key <= LIM) {
                cout << root->key << ' ';
            }
            traversal(root->pLeft);
            traversal(root->pRight);
        }
    }
};

int32_t main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    Tree<> *t = new Tree<>();
    vector<int> a = {1, 2, 3, 4, 5, 6};
    t->convertToTree(t->root, a);
    t->traversal(t->root);
}