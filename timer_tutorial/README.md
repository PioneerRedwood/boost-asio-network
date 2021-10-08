# boost::asio Timer tutorial

### 2021-08-25

마지막 튜토리얼(timer_5) 하다 아래 문제가 발생

```
...
1>LINK : fatal error LNK1104: 'libboost_thread-vc142-mt-sgd-x32-1_77.lib' 파일을 열 수 없습니다.
...
```

boost include한 상태로 프로젝트를 빌드하는데 동적파일을 링크하려는 시도가 생겼다.
"C/C++ - 코드생성 - 런타임 라이브러리" 설정에서 DLL을 사용하지 않는 것으로 해도 안됨

해결 방법 탐색 중

- [스택오버플로우](https://stackoverflow.com/questions/13042561/fatal-error-lnk1104-cannot-open-file-libboost-system-vc110-mt-gd-1-51-lib)
- [중국 개발 커뮤니티](https://blog.csdn.net/aoxuestudy/article/details/115427090)

추신; boost를 따로 다운, CMAKE로 빌드하고 나서 include 파일을 해당 프로젝트에 외부 경로로 등록해보면 될 거 같다.
-> CMAKE가 아니라 boost 파일 내 bootstrap.bat 파일을 실행(2021-08-26 추가)



# boost::asio daytime server/client tutorial

### 2021-08-26

- 하나의 프로젝트에 서버와 클라이언트를 만들어두니 따로 실행하는게 어려움
- vscode에 C/C++을 빌드/컴파일 실행할 수 있는 환경을 만들어야겠다. (환경에 제약을 받음)

2021-08-27 따로 프로젝트 파일을 만들어서 개별 실행, 튜토리얼 완료