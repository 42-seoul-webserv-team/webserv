#!/bin/python3

# import cgi

# form = cgi.FieldStorage()
import sys
import os


def	main():
	query_string = os.environ.get('QUERY_STRING', '')
	request_method = os.environ.get('REQUEST_METHOD', '')
	script_name = os.environ.get('SCRIPT_NAME', '')
	content_length = os.environ.get('CONTENT_LENGTH', '')
	content_type = os.environ.get('CONTENT_TYPE', '')

	post_data = ""
	if request_method == "POST" and content_length:
		post_data = sys.stdin.read(int(content_length))

	print("Content-Type: text/html")
	print("<!DOCTYPE html><html><head><title>CGI</title></head><body>")
	print("<h1>WebServ</h1>")

	if query_string:
		print(f"<p>Query String: {query_string}</p>")
	if request_method:
		print(f"<p>Request Method: {request_method}</p>")
	if script_name:
		print(f"<p>Script Name: {script_name}</p>")
	if post_data:
		print(f"<p>Post Data: {post_data}</p>")

	print("</body></html>")

if __name__ == '__main__':
	main()