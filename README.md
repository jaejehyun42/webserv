## 협업 과정

1. 요구사항정리
2. 환경정리 (리눅스vs맥)
리눅스 사용? seige테스트 가능(sudo 명령어 필요), 파일 수정 시 automic 보장, 다른곳에서도 코딩가능. 
3. 플로우차트
    - https://www.figma.com/design/vnvQFLBTB2uQ0g6SVmldne/Webserv?node-id=3-753&m=dev&t=GLccVe81iZBVoUX1-1
    - 참고 : https://blog.naver.com/taks47/223482427582
4. 클래스다이어그램
    - 참고 : https://www.nextree.co.kr/p6753/
5. 파트나누기
6. 코딩 컨벤션 정하기
7. 깃 커밋 메세지 규칙 정하기
8. Git Team project 시작

---

## 요구 사항 및 플로우

1. 시작
2. confgirue 파일 해석. parse 결과 적용. 
    - 참고: 멀티 포트. 타임아웃
3. 서버 시작. 소켓 준비, 비동기i/o 멀티플렉싱 kqueue으로 적용. 루프시작.
    - 참고: tcp 프로토콜 조절가능한지
4. 요청 해석. kqueue 사용. HTTP parse. 유효하면 처리. 
5. 응답. 정적, 동적 구분.  python으로 cgi구현 및 처리.
6. 에러처리. 시그널처리

- 참고: https://www.figma.com/design/vnvQFLBTB2uQ0g6SVmldne/Webserv?node-id=3-753&m=dev&t=GLccVe81iZBVoUX1-1

---

## 클래스

1. ServConf
    - parse, port, domain, timeout. 싱글톤으로 구성
2. Serv
    - 소켓, kqueue
3. Message
    - →Request:
    - →Responce:

---

## 파트

기능단위로 협업 및 {configure: 재빈 , request: 승현 , response: 정수 } 담당

---

## 코딩 컨벤션

1. 클래스 명은 UpperCanonicalForm사용
2. 함수는  lowerCanonicalForm사용
3. 파일명 소문자로 시작, 띄어쓰기는 _로 표현
4. private 멤버는 _로 시작

---

## Git commit message convention

- 참고 : https://gist.github.com/jeshin119/13b561ae0d830bc1cbb2a6e91b7f5a84
