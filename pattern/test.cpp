//
// Created by Kim Long on 13/8/24.
//
#include <vector>
#include <iostream>

using namespace std;

template<class T>
struct Node {
    T key;
    Node *next;
};

class NhanVien {
private:
    int age;
    string name;
    double salary;
    
public:
    void output() { }
};

int32_t main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    // Tạo danh sách liên kết các nhân viên
    Node<NhanVien> *head;
    // Tạo danh sách liên kết các số nguyên
    Node<int> *head;
    // Tạo danh sách liên kết các số thực
    Node<float> *head;
    // Tạo danh sách liên kết các ký tự
    Node<char> *head;
    // Tạo danh sách liên kết các cặp dữ liệu
    Node<pair<int, int>> *head;
    // Tạo danh sách liên kết chứa các vector
    Node<vector<NhanVien>> *head;
    // Tạo danh sách liên kết chứa tensor không gian
    Node<vector<pair<int, int>>> *head;
}