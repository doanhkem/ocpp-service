## Triển khai mã nguồn vào thực tế

### Bước 1: Cần đảm bảo máy đã có python version 3 và đã cài đặt platformio từ trước
### Bước 2: Vào trong thư mục src xoá file **main.cpp**
### Bước 3: Mở terminal của vscode bằng tổ hợp **control + `** và sau đó di chuyển vào thư mục config
```bash
cd config
```
### Sau khi di chuyển vào trong **config** thì dán dòng lệnh ghi file mã nguồn vào như sau

```python
python write.py -wifi=SOLARZ-PKT -password=88888888 -num_connector=4 -backend_url=ws://dev-ocpp.rabbitevc.vn/websocket/central-system -charge_box_id=1 -file=../src/main.cpp
```
### Trong đó các thông số có ý nghĩa là
- -wifi: tên wifi cần kết nối
- -password: mật khẩu của wifi cần kết nối.
- -num_connector: số lượng vòi sạc output.
- -backend_url: đường dẫn websocket của ocpp.
- -charge_box_id: mã trụ sạc hoặc box sạc.
- -file: đường dẫn ghi ra file, mặc định là src/main.cpp nên không cần phải thay đổi cụm này.


### Bước 4: sau khi hoàn thành thì mở terminal của riêng platform io và chạy lệnh build cho MacOS + Linux
``` bash
./build_project.sh
```

hoặc lệnh build cho Window
``` bash
./build_project.bat
```

### Bước 5: đảm bảo kiểm tra kết nối wifi bằng log gửi lên server.