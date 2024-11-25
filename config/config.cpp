#include <fstream>  // Thư viện để thao tác với file
#include <iostream>
#include <string>

using namespace std;

int32_t main() {
    int numConnector;
    string backendUrl, chargeBoxId;

    // Hiển thị thông tin ban đầu
    cout << "===ESP Config Tool===\n\n";

    // Nhập số connector
    cout << "Input Num connector: ";
    cin >> numConnector;

    // Nhập URL của Backend OCPP
    cout << "Backend OCPP Url: ";
    cin >> backendUrl;

    // Nhập ID của Charge box
    cout << "Charge box ID: ";
    cin >> chargeBoxId;

    // Mở file config.txt để ghi dữ liệu
    ofstream configFile("file.txt", ios::app);  // ios::app để ghi thêm vào cuối file

    // Kiểm tra nếu file mở thành công
    if (configFile.is_open()) {
        // Ghi dữ liệu vào file
        configFile << numConnector << " " << backendUrl << " " << chargeBoxId;

        // Đóng file sau khi ghi
        configFile.close();

        cout << "\nConfiguration has been saved to config.txt.\n";
    } else {
        // Nếu không mở được file, thông báo lỗi
        cerr << "Error: Unable to open config.txt for writing.\n";
    }

    return 0;
}
