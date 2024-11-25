#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int32_t main() {
    // Mở file "config.txt" để đọc và chuyển hướng luồng nhập từ file này
    freopen("file.txt", "r", stdin);

    // check
    vector<string> v;

    // Khai báo một chuỗi để lưu dòng văn bản đọc từ file
    string s, token;

    // Đọc một dòng từ file (stdin đã được chuyển hướng)
    getline(cin, s);

    // In dòng vừa đọc ra màn hình
    cout << "String config: " << s << '\n';

    stringstream ss(s);

    while (ss >> token) {
        v.emplace_back(token);
    }

    return 0;
}
