#!/usr/bin/env python3
import sys

# 파일 이름 설정
filename = 'image.png'

# 파일 열기
with open(filename, 'wb') as file:
    # 표준 입력에서 데이터 읽고 파일에 쓰기
    file.write(sys.stdin.buffer.read())

print(f"파일이 {filename}으로 저장되었습니다.")