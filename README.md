# boost-asio-network

boost::asio를 사용한 ~~게임~~ 네트워크 학습 겸 최종 프레임워크 제작이 목표 😎

- server/client 양쪽 데이터 주고 받는 식의 프레임워크 설계/구현
- boost::asio 및 Google protobuf 사용 예정 --> 문제점 발생
- boost 1.77.0 사용



## boost::asio Timer tutorial

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



## 번외) Visual Studio 2019 boost 설치 및 디버깅

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



## boost::asio daytime server/client tutorial

### 2021-08-26

- 하나의 프로젝트에 서버와 클라이언트를 만들어두니 따로 실행하는게 어려움
- vscode에 C/C++을 빌드/컴파일 실행할 수 있는 환경을 만들어야겠다. (환경에 제약을 받음)

2021-08-27 따로 프로젝트 파일을 만들어서 개별 실행, 튜토리얼 완료



## echonet

### 2021-08-27 #1

- echonet 이라는 간단하게 서버와 클라이언트 간 에코 날리는 시스템 제작
  - 아직 미완성
- 필요 및 추가 개선 사항
  - asio 내에서 사용하는 비동기 함수들의 콜백 사용 익숙해질 필요가 있음
  - 서버와 클라이언트의 데이터 전송을 감추는 connection 데이터 전송 래퍼 클래스 설계 중
    - 기존에 많이 참고했던 [javidx9의 네트워크 프레임워크](https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/Networking/Parts1%262/net_connection.h) 살펴보는 중

### 2021-08-28 #2

- 서버와 클라이언트 간 에코 주고 받는 시스템을 만드려고 했는데 javidx9의 네트워크 프레임워크에서 살폈던 설계를 참고하며 구현하는 게 어려움
- 클라이언트에서 입력 받아 서버에 바로 보내면 되고 서버는 받으면 다시 날려주는 식으로 하면 됨

### 2021-08-29 #3

- **std::scoped_lock** C++17
  - 스레드 안전 자료구조를 구현하기 위해서 (스레드 간 자원 공유시 발생할 수 있는 데드락(deadlock), 상호배제(mutex)을 해결 하기 위한 C++17 안전 기법) 프로젝트 속성에서 C++ 언어 표준을 C++17로 명시해둬야 함
  - 기본 프로젝트 생성시 프로젝트의 C++ 언어 표준이 C++14이었으므로 네임스페이스 std 안에 있는 thread, mutex, optional 등과 같은 헤더파일의 경로를 찾지 못하는 경우가 발생할 수 있으니 확인할 것

### 2021-08-30 #4

- [popcorntree님의 블로그](https://popcorntree.tistory.com/159)를 참고해서 다른 방식으로 구현 중
- 클라이언트에서 서버로 주기적으로 고정 데이터를 전송하는 것은 문제가 되지 않지만 문제는 일정 주기가 아닌 전송하는 타이밍과 전송할 데이터를 직접 컨트롤하는 과정을 구현하는 것이 어려운 부분
  - 내가 이해한 프로세스🤔
    - boost::asio::io_context가 스레드로 동작 중
    - 스레드 내부 클라이언트(혹은 서버)에서 보낼 데이터가 있으면 동기/비동기 쓰기/읽기 동작 수행
    - 보낼 데이터는 클라이언트 혹은 서버의 멤버 변수 자료구조에 담김
    - 동기 vs 비동기 (sync vs async)
      - 동기; 네트워크 동작하는 동안에 다른 동작을 수행하지 않음
      - 비동기; 네트워크 동작하는 동안에 다른 동작을 수행할 수 있으며 네트워크 동작이 끝나면 콜백 함수 실행

### 2021-08-31 #5

- 여전히 서버와 클라이언트 프로세스 진행 중 C++ 기초가 부족함을 느끼고 있지만 계속 해서 구현 중
  - 샘플 코드를 따라 쓰면서 구현한 것들을 자신만의 스타일대로 묶는 것 뿐인데 대체 왜 오류 투성이에 깔끔하지 않은 코드가 돼가는 걸까? 🤢 
  - When will I start "Google protobuf"??
  - 아무래도 책이 필요하다 무려 [테크니컬 참고 도서](https://github.com/devharsh/Technical-eBooks/blob/master/Boost.Asio%20C%2B%2B%20Network%20Programming.pdf)가 pdf로 😎
- **이때 이후로 도메인을 변경한다. echonet -> BoostAsioStudy(BAS) - 2021-09-16 추가**

## Boost Asio Study & Making General Network Library

### 2021-09-01 #1

- 기초부터 다시 잡는다는 생각으로 테크니컬 참고 도서를 정독 중

  - C++ 기초 지식만 부족한게 아니라 프레임워크 개발 경험이 없다보니 아키텍처 설계를 기획하는 과정부터 길을 해메는 중
  - 처음보단 나아지고 있음을 느끼지만 개발은 역시 겸손함을 놓으면 안된다 🙏
- **한글로 작성한 파일이 모두 깨져버렸다 -> 복구해야 함** 
  - 2021-09-02 복구

### 2021-09-12 #2

- Boost.Asio는 확실히 매력 있는 라이브러리임에도 제대로 쓸 수 없는 것은 사용자 수준에 머물러있는 논리력 때문인것 같다. (착잡하다 😥)
- 테크니컬 도서를 계속 읽고 있으며 4장까지 읽은 뒤 나만의 핑을 날리고 클라이언트 접속 리스트를 요청하고 응답하는 서버/클라이언트를 만들고 있는데 오류를 남발하고 있다.

### 2021-09-14 #3

- Boost.Asio server/client 접속과 핑 보내는 것을 완료했다! 😊
- 테크니컬 도서의 마지막 5장을 남겨두었다. 마무리한 뒤에 나만의 프레임워크 설계를 시작한다.
  - 2021-09-15 추가; 테크니컬 도서는 5장이 마지막이 아니다.

### 2021-09-15 #4

- 클라이언트에서 서버로 원하는 시간에 데이터를 전송하는 것을 구현해보고 있다.
- 추가로 서버와 클라이언트 그리고 각 연결(소켓, 버퍼 등)의 래퍼 클래스 구조도 손 보고 있는데 완벽하다고 할 만한 구조(설계)를 구현하는 것이 상당히 머리를 복잡하게 만든다. 🤦‍♂️

### 2021-09-16 #5

- [디버깅 관련 문서](https://github.com/PioneerRedwood/boost-asio-network/tree/main/boost-asio-network/Boost_Asio_Study/Boost.AsioDebuggingDocument.md)를 만들었다.

### 2021-09-24 #6

- [테스트 관련 문서](https://github.com/PioneerRedwood/boost-asio-network/blob/main/framework-test/boost-asio-network%20framework%20test.md)를 만들었다.
- 서버/클라이언트 계속 진행 중, 조금 더 매끄러운 통신이 가능한 방법을 모색 중
  - 서버 측에서 접속한 클라이언트 정보를 전송하는 기능 추가 중

### 2021-09-25 #7

- 서버/클라이언트 통신에서 수신 메시지를 처리하는 데 일관성이 없는 듯하다.
  - 서버에선 connection의 on_message()에서 새로운 비동기 호출을 생성
  - 클라이언트에선 connection의 on_message()에서도 처리, 받은 것을 수신 메시지 덱에 넣어두어서 클라이언트의 개체가 있는 main()의 루프에서 처리 -> 일관성을 깨는 부분

### 2021-09-27 #8

- 클라이언트 세션 룸 프레임워크 제작 중
- 클라이언트 측에서 making room, searching room, join room, exit room 등 기본 세션 룸 명령 호출하는 방식
- 2021-09-29 추가
  - [디버깅 문서](https://github.com/PioneerRedwood/boost-asio-network/tree/main/boost-asio-network/Boost_Asio_Study/Boost.AsioDebuggingDocument.md) 중 2021-09-29 문제점 #6 참고
  
  

## End of works?

한달 남짓한 시간동안 boost-asio 라이브러리를 통해 TCP 서버와 클라이언트 동작을 직접 구현하기도 해보면서 학습을 진행했다. 구현하고 싶었던 기능들을 구현하고 도중에 예상치 못한 버그들을 볼 때마다 ~~화내면서~~ 디버깅을 해나갔다. google-protobuf와 같이 며칠간 붙잡고도 해결하지 못했던 기능도 있다. 

효율적이고 직관적으로 코딩하고 네트워크 로직 설계를 고민하는게 즐거웠다. 전에 했던 [이모부의 과제]()가 몇년에 걸쳐 이렇게 다른 방식으로 흐를 것이라곤 상상도 못했다. 그때와 다르게 지금은 과제를 넘어서 구조 설계의 재미와 디버깅 과정 중 학습의 기쁨으로 하고 있다.

학습만이 아니라 프레임워크를 제작이 최종 목표였으나 문득 어떤 것을 구현하고 있었는지 구체적인 목표가 없음을 깨달았다. 갈망하던 그 목표는 곧 개발의 방향과 사용할 네트워크 아키텍처였다. 이제는 단순히 서버와 클라이언트끼리 핑만 보내는 추상적인 것이 아니라 로그인, 매치메이킹, 전투 시뮬레이션, 경매장 시스템, 랭킹 시스템 등의 실용적이며 차원 높은 기술을 사용하며 구현하고 싶어졌다.

많은 시간과 노력이 요구되는걸 알면서 동시에 설렌다. 



## ~~Packet serialize/deserialize~~

### ~~#1 tsdeque~~

- ~~designed by OneLoneCoder javidx9's tsdeque(thread-safe deque)~~
- ~~template class for message~~

### ~~#2 ban_packet~~



## Connection into Unity client

- Unity client simple login/lobby module
  - Simple login transaction(Handling encoded/decoded data)
  - Lobby(Matchmaking, Room system)
- boost-asio login/lobby server
  - MySQL, Redis

