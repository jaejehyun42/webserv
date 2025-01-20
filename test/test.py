import cgi
import io

# 파일 경로
file_path = '/Users/jeshin/42Cursus/webserv/html/formtest'

# 파일 열기
with open(file_path, 'rb') as f:
    data = f.read()

# 가짜 CGI 환경 설정
class FakeCGIRequest:
    def __init__(self, data):
        self.body = data
        self.headers = {
            'Content-Type': 'multipart/form-data; boundary=——WebKitFormBoundaryO7rlE6UB4oLSUEfC'
        }

# 가짜 요청 객체 생성
request = FakeCGIRequest(data)

# cgi.FieldStorage로 데이터를 파싱
field_storage = cgi.FieldStorage(fp=io.BytesIO(request.body), environ={'REQUEST_METHOD': 'POST'})
#file_item = field_storage["file"]
print("\n")
print("\n")
print("\n")
print("\n")
print("\n")
print("\n")
#print(f"{file_item.filename}") 
# 파싱한 데이터를 출력
for key in field_storage.keys():
    item = field_storage[key]
    if item.filename:  # 파일인 경우
        print("####\n")
        print(f"File field: {key} -> Filename: {item.filename}")
        print("****\n")
        print(f"File content: {item.file.read().decode()}")
        print("!!!!\n")
    else:  # 일반 필드인 경우
        print(f"Field: {key} -> Value: {item.value}")
        print("asodif\n")
