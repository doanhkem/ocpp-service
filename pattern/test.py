import argparse

def main():
    # Tạo đối tượng ArgumentParser
    parser = argparse.ArgumentParser(description='Process width and height arguments.')

    # Thêm đối số w với kiểu int
    parser.add_argument('-w', '--width', type=int, required=False, help='Width value')

    # Thêm đối số h với kiểu int
    parser.add_argument('-ht', '--height', type=int, required=False, help='Height value')

    # Parse các đối số từ dòng lệnh
    args = parser.parse_args()

    # Lấy giá trị của đối số w và h và in ra
    width = args.width if args.width is not None else 640
    height = args.height if args.height is not None else 480
    print(f'Width: {width}, Height: {height}')

if __name__ == '__main__':
    main()