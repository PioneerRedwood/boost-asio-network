# boost-asio-network

boost::asio를 사용한 게임 네트워크 학습 겸 최종 프레임워크 제작이 목표 😎

- server/client 양쪽 데이터 주고 받는 식의 프레임워크 설계/구현
- boost::asio 및 Google protobuf 사용 예정



### 2021-08-25 boost::asio Timer tutorial

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



### Visual Studio 2019 boost 설치 및 디버깅

- boost 재설치 후 bootstarp.bat 을 실행하면 b2.exe 실행 파일 생성
- b2.exe 실행 시 boost/stage/lib 생성 - 이는 프로젝트 링커(DLL) 경로
  - b2.exe 실행에 시간이 다소 소요
- 외부 라이브러리를 해당 프로젝트에 추가하기 위해 몇가지를 점검해야한다
  - 프로젝트 속성에서 VC++ 디렉터리 - 외부 include 디렉터리 경로 설정
  - C/C++ - 추가 포함 디렉터리(Additional include directory) 경로 설정
  - C/C++ - 코드 생성 - 런타임 라이브러리를 다중 스레드 디버그 DLL(/MDd)로 설정[(언제나 찾는 MS DOCS)](https://docs.microsoft.com/ko-kr/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-160)
  - C/C++ - 미리 컴파일된 헤더(pre-compiled header) 사용 안 함으로 설정
  - 링커 - 추가 라이브러리 디렉터리(Additional library directory) - 경로 설정

- 해결 뒤 다음은 [daytime 동기/비동기 서버-클라이언트 튜토리얼](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime1.html)

### 2021-08-26 boost::asio daytime server/client tutorial

- 하나의 프로젝트에 서버와 클라이언트를 만들어두니 따로 실행하는게 어려움
- vscode에 C/C++을 빌드/컴파일 실행할 수 있는 환경을 만들어야겠다. (환경에 제약을 받음)

2021-08-27 따로 프로젝트 파일을 만들어서 개별 실행, 튜토리얼 완료



## echonet

#### 2021-08-27 #1

- echonet 이라는 간단하게 서버와 클라이언트 간 에코 날리는 시스템 제작
  - 아직 미완성
- 필요 및 추가 개선 사항
  - asio 내에서 사용하는 비동기 함수들의 콜백 사용 익숙해질 필요가 있음
  - 서버와 클라이언트의 데이터 전송을 감추는 connection 데이터 전송 래퍼 클래스 설계 중
    - 기존에 많이 참고했던 [javidx9의 네트워크 프레임워크](https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/Networking/Parts1%262/net_connection.h) 살펴보는 중

#### 2021-08-28 #2

- 서버와 클라이언트 간 에코 주고 받는 시스템을 만드려고 했는데 javidx9의 네트워크 프레임워크에서 살폈던 설계를 참고하며 구현하는 게 어려움
- 클라이언트에서 입력 받아 서버에 바로 보내면 되고 서버는 받으면 다시 날려주는 식으로 하면 됨

#### 2021-08-29 #3

- **std::scoped_lock** C++17

  - 스레드 안전 자료구조를 구현하기 위해서 (스레드 간 자원 공유시 발생할 수 있는 데드락(deadlock), 상호배제(mutex)을 해결 하기 위한 C++17 안전 기법) 프로젝트 속성에서 C++ 언어 표준을 C++17로 명시해둬야 함
  - 기본 프로젝트 생성시 프로젝트의 C++ 언어 표준이 C++14이었으므로 네임스페이스 std 안에 있는 thread, mutex, optional 등과 같은 헤더파일의 경로를 찾지 못하는 경우가 발생할 수 있으니 확인할 것

  

#### 2021-08-30 #4

- [popcorntree님의 블로그](https://popcorntree.tistory.com/159)를 참고해서 다른 방식으로 구현 중
- 클라이언트에서 서버로 주기적으로 고정 데이터를 전송하는 것은 문제가 되지 않지만 문제는 일정 주기가 아닌 전송하는 타이밍과 전송할 데이터를 직접 컨트롤하는 과정을 구현하는 것이 어려운 부분
  - 내가 이해한 프로세스🤔
    - boost::asio::io_context가 스레드로 동작 중
    - 스레드 내부 클라이언트(혹은 서버)에서 보낼 데이터가 있으면 동기/비동기 쓰기/읽기 동작 수행
    - 보낼 데이터는 클라이언트 혹은 서버의 멤버 변수 자료구조에 담김
    - 동기 vs 비동기 (sync vs async)
      - 동기; 네트워크 동작하는 동안에 다른 동작을 수행하지 않음
      - 비동기; 네트워크 동작하는 동안에 다른 동작을 수행할 수 있으며 네트워크 동작이 끝나면 콜백 함수 실행

