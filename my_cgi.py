#!/usr/bin/env python3

import os
import sys
import urllib.parse
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import cgi
import mimetypes
import shutil

def chkError(e):
    if "Permission denied" in str(e):
        return "403 Forbidden"
    else:
        return "500 Internal Server Error"

# 환경 변수에서 각종 정보 읽기
path_info = os.environ.get("PATH_INFO", "")
query_string = os.environ.get("QUERY_STRING", "")
request_method = os.environ.get("REQUESTED_METHOD", "")
content_type   = os.environ.get("CONTENT_TYPE", "")
content_length = os.environ.get("CONTENT_LENGTH", 0)
root_dir = os.environ.get("DOCUMENT_ROOT", "")

#path_info = "/test.txt"
#query_string = "name=John&&age=30&hobby=reading&hobby=traveling"
#query_string = ""
#request_method = "GET"
#content_length = 0
#content_type = "text/plain"
#root_dir = "/Users/seunghan/42_projects/webserv/debug/d1/response/cgifiles"

content_length = int(content_length)

# HTML 제목
title = ""

# 상태 라인
status = ""

# 루트 디렉토리가 없으면 생성
if not os.path.exists(root_dir):
    os.makedirs(root_dir, exist_ok=True)

# PATH_INFO 의 경로를 추가
if path_info:
    path = root_dir + path_info
else:
    path = root_dir

# url에서 파일명, MIME 타입, 확장자 추출
filename = os.path.basename(path)
mime_type, encoding = mimetypes.guess_type(path)
# MIME 타입이 없다면 바이너리로 간주
if mime_type is None:
    mime_type = "application/x-mach-binary"
extension = mime_type.split("/")[-1]

# 콘텐츠 타입이 텍스트인지 바이너리인지 확인하기 위한 플래그
isbinary = False

# 바이너리 타입 리스트
binary_types = [
        "application/octet-stream",
        "application/x-mach-binary",
        "application/pdf",
        "application/zip",
        "application/x-tar",
        "application/x-gzip",
        "application/vnd.",
        "image/",
        "audio/",
        "video/",
    ]

# 딕셔너리에 쿼리문 파싱 후 담기
params = urllib.parse.parse_qs(query_string)

# 반환할 바디
content = ""

# GET 인데 PATH_INFO 가 존재할 경우
if request_method == "GET" and path_info:

    # MIME 타입 확인
    isbinary = any(mime_type.startswith(bt) for bt in binary_types)

    try:
        # 바이너리와 텍스트 구분하여 read
        if isbinary == False:
            with open(path, "r", encoding="utf-8") as file: # 텍스트 읽기, with은 파일 열고 닫기까지 수행
                if content_length > 0:
                    content = file.read(content_length)
                else:
                    content = file.read()
        else:
            with open(path, "rb") as file: # 바이너리 읽기, 옵션에 b 가 붙으면 바이너리로 수행
                if content_length > 0:
                    content = file.read(content_length)
                else:
                    content = file.read()
    except Exception as e:
        # 파일을 open 하는데 실패할 경우
        params["status"] = [f"Error reading file {filename} : {str(e)}"]
        status = chkError(e)

# POST 인데 multipart 인 경우
elif request_method == "POST" and content_type.startswith("multipart/form-data"):
    # 폼 가져오기
    form = cgi.FieldStorage()

    # HTML form 확인 (file)
    if "file" in form:
        file_item = form["file"]
    else:
        params["status"] = ["HTML form is wrong."]
        status = "400 Bad Request"

    # 파일 이름이 존재하면 읽어서 저장
    if file_item.filename:
        save_path = path + file_item.filename
        try:
            with open(save_path, "wb") as f:
                f.write(file_item.file.read())
            # 업로드가 완료된 경우
            params["status"] = [f"File '{filename}' uploaded successfully."]
            status = "201 Created"
        except Exception as e:
            # 업로드에 실패한 경우
            params["status"] = [f"Error uploading file {filename} : {str(e)}"]
            status = chkError(e)
    else:
        params["status"] = ["No file was uploaded."]
        status = "400 Bad Request"

# POST 인데 multipart 가 아닌 경우
elif request_method == "POST":

    # Content-Type 확인
    isbinary = any(content_type.startswith(bt) for bt in binary_types)

    # Content-Length 확인 후 Content-Type 에 따라 body 읽기
    if content_length > 0:
        if isbinary == True:
            body = sys.stdin.buffer.read(content_length) # 바이너리 읽기
        else:
            body = sys.stdin.read(content_length) # 텍스트 읽기
    else:
        if isbinary == True:
            body = sys.stdin.buffer.read()
        else:
            body = sys.stdin.read()

    # 파일 이름, 확장자 정하기
    filename = "test.txt"
    # 이미 있는 파일명이면 바꿔서 지정
    counter = 1
    while os.path.exists(os.path.join(path, filename)):
        filename = f"test_{counter}.txt"
        counter += 1
    save_path = os.path.join(path, filename)

    # 파일 쓰기
    try:
        if isbinary == False:
            with open(save_path, "w") as file:
                file.write(body)
        else:
            with open(save_path, "wb") as file:
                file.write(body)
        params["status"] = [f"File '{filename}' uploaded successfully."]
        status = "201 Created"
    except Exception as e:
        params["status"] = [f"Error uploading file {filename} : {str(e)}"]
        status = chkError(e)

# DELETE는 PATH_INFO 가 존재해야만 작동
elif request_method == "DELETE" and path_info:
    try:
        if os.path.isdir(path):
            shutil.rmtree(path)  # 디렉토리 삭제 (비어있지 않아도 삭제)
            params["status"] = [f"'{filename}' deleted successfully."]
        elif os.path.isfile(path):
            os.remove(path)  # 파일 삭제
            params["status"] = [f"'{filename}' deleted successfully."]
        else:
            params["status"] = [f"Error: '{filename}' doesnt exist"] # 파일이 존재하지 않음
            status = "404 Not Found"
    except Exception as e:
        params["status"] = [f"Error deleting '{filename}' : {str(e)}"]
        status = chkError(e)
elif request_method == "DELETE":
    params["status"] = ["Error: Need path info"]
    status = "400 Bad Request"

# status 가 비어있으면 정상
if status == "":
    status = "200 OK"

# status 가 정상이라면 제목을 CGI Test 로 설정
elif status == "201 Crated" or status == "200 OK":
    title = "CGI Test"
else:
    title = status # 아니라면 status를 제목으로 설정

# content 가 비어있으면 바디 직접 작성
if content == "":
    mime_type = "text/html"
    content = f"""<!DOCTYPE html>
<html>
<head>
    <title>{title}</title>
</head>
<body>
    <h1>{title}</h1>
    <p>Request Method: {request_method}</p>
    <p>PATH_INFO: {path_info}</p>
    <p>params: {params}</p>
</body>
</html>
"""

# HTTP 응답 길이 재기
if isbinary == True:
    content_length = len(content)
else:
    content_length = len(content.encode("utf-8"))

#HTTP 응답 출력
print(f"{status}")  # 상태 라인
print(f"Content-Type: {mime_type}") # 타입
print(f"Content-Length: {content_length}") # 길이
print() # 헤더 끝

print(content, end='') # 바디 출력
