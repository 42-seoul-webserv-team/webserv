#!/usr/bin/env python3
import cgi
import os
import sys

def main():
    # 요청 본문을 표준 입력에서 읽습니다.
    form = cgi.FieldStorage(fp=sys.stdin, environ=os.environ, keep_blank_values=True)

    # 'uploaded_file'이라는 필드로 전달된 파일을 가져옵니다.
    if 'uploaded_file' in form:
        file_item = form['uploaded_file']

        # 파일이 업로드된 경우
        if file_item.file:
            # 파일 이름을 가져옵니다.
            file_name = os.path.basename(file_item.filename)
            # 파일 내용을 읽어 서버의 디렉토리에 저장합니다.
            with open(f'/path/to/save/{file_name}', 'wb') as fout:
                while True:
                    chunk = file_item.file.read(100000)
                    if not chunk:
                        break
                    fout.write(chunk)

            print("Content-Type: text/html\n")
            print(f"<html><body><h1>File {file_name} uploaded successfully.</h1></body></html>")
        else:
            print("Content-Type: text/html\n")
            print("<html><body><h1>No file was uploaded.</h1></body></html>")
    else:
        print("Content-Type: text/html\n")
        print("<html><body><h1>No file field found in the form.</h1></body></html>")

if __name__ == '__main__':
    main()
