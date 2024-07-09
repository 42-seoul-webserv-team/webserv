import os
import cgi
import cgitb
import sys
from html import escape
from urllib.parse import urlencode

cgitb.enable()

script_dir = os.path.dirname(os.path.abspath(__file__)) + "/page/cgi/upload"
upload_dir = script_dir

def print_debug(message):
    sys.stderr.write(f"{message}\n")

def handle_upload():
    form = cgi.FieldStorage()
    if "file" in form:
        fileitem = form["file"]
        if fileitem.filename:
            fn = os.path.basename(fileitem.filename)
            file_path = os.path.join(upload_dir, fn)
            try:
                with open(file_path, 'wb') as f:
                    f.write(fileitem.file.read())
                return f"파일 '{escape(fn)}'이(가) 성공적으로 업로드되었습니다."
            except Exception as e:
                return f"파일 업로드 중 오류 발생: {str(e)}"
    return ""

def handle_delete(filename):
    file_path = os.path.join(upload_dir, filename)
    print_debug(f"삭제 시도: {file_path}")
    if os.path.exists(file_path):
        try:
            os.remove(file_path)
            return f"파일 '{escape(filename)}'이(가) 삭제되었습니다."
        except Exception as e:
            error_message = f"파일 삭제 중 오류 발생: {str(e)}"
            print_debug(error_message)
            return error_message
    else:
        return f"파일 '{escape(filename)}'을(를) 찾을 수 없습니다."

def generate_html(message=""):
    try:
        file_list = [f for f in os.listdir(upload_dir) if f != os.path.basename(__file__)]
    except Exception as e:
        file_list = []
        message += f"디렉토리 읽기 오류: {str(e)}"
    html = f"""<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>파일 관리 시스템</title>
    <style>
        body {{ font-family: Arial, sans-serif; line-height: 1.6; margin: 0 auto; padding: 20px; max-width: 800px; }}
        h1, h2 {{ color: #333; }}
        form {{ margin-bottom: 20px; }}
        table {{ width: 100%; border-collapse: collapse; }}
        th, td {{ padding: 10px; border: 1px solid #ddd; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .message {{ background-color: #e6f3ff; border: 1px solid #b8d4f2; padding: 10px; margin-bottom: 20px; }}
    </style>
</head>
<body>
    <h1>파일 관리 시스템</h1>
    {f'<div class="message">{message}</div>' if message else ''}
    <h2>파일 업로드</h2>
    <form enctype="multipart/form-data" method="post">
        <input type="file" name="file">
        <input type="submit" value="업로드">
    </form>
    <h2>파일 목록</h2>
    <table>
        <tr>
            <th>파일명</th>
            <th>작업</th>
        </tr>
"""
    html += """
    <script>
    function deleteFile(filename) {
        if (confirm('정말로 이 파일을 삭제하시겠습니까?')) {
            fetch(`${window.location.href}/upload/${filename}`, {
                method: 'DELETE',
            })
            .then(response => response.text())
            .then(result => {
                if (result.includes('삭제되었습니다')) {
                    alert('삭제에 성공했습니다');
                    window.location.reload();
                } else {
                    alert(result);
                }
            })
            .catch(error => {
                console.error('Error:', error);
                alert('파일 삭제 중 오류가 발생했습니다.');
            });
        }
    }
    </script>
    """

    for filename in file_list:
        if filename == "cgi-bin.py":
            continue
        html += f"""
        <tr>
            <td>{escape(filename)}</td>
            <td>
                <button onclick="deleteFile('{escape(filename)}')">삭제</button>
            </td>
        </tr>"""

    html += """
    </table>
</body>
</html>
"""
    return html

def main():
    if os.environ['REQUEST_METHOD'] == 'DELETE':
        path_info = os.environ.get('PATH_INFO', '')
        filename = os.path.basename(path_info)
        if filename:
            result = handle_delete(filename)
            print(result)
            return
    elif os.environ['REQUEST_METHOD'] == 'POST':
        upload_message = handle_upload()
        message = upload_message
    else:
        message = ''

    print(generate_html(message))

if __name__ == "__main__":
    main()