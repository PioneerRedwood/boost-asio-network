# boost-asio-network

boost::asio를 사용한 ~~게임~~ 네트워크 학습 겸 최종 프레임워크 제작이 목표 😎

- server/client 양쪽 데이터 주고 받는 식의 프레임워크 설계/구현
- boost::asio 및 Google protobuf 사용 예정 --> 문제점 발생
- boost 1.77.0 사용

## [boost-asio timer tutorial](https://github.com/PioneerRedwood/boost-asio-network/tree/main/timer_tutorial)

## [boost-asio based echonet](https://github.com/PioneerRedwood/boost-asio-network/tree/main/echonet)

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

## [boost-asio making generally](https://github.com/PioneerRedwood/boost-asio-network/tree/main/red_net)

## [boost-asio study with e-book pdf](https://github.com/PioneerRedwood/boost-asio-network/tree/main/Boost_Asio_Study)

## [boost-asio debugging](https://github.com/PioneerRedwood/boost-asio-network/tree/main/boost-asio-debugging)

## End of works?

한달 남짓한 시간동안 boost-asio 라이브러리를 통해 TCP 서버와 클라이언트 동작을 직접 구현하기도 해보면서 학습을 진행했다. 구현하고 싶었던 기능들을 구현하고 도중에 예상치 못한 버그들을 볼 때마다 ~~화내면서~~ 디버깅을 해나갔다. google-protobuf와 같이 며칠간 붙잡고도 해결하지 못했던 기능도 있다. 

효율적이고 직관적으로 코딩하고 네트워크 로직 설계를 고민하는게 즐거웠다. 전에 했던 [이모부의 과제]()가 몇년에 걸쳐 이렇게 다른 방식으로 흐를 것이라곤 상상도 못했다. 그때와 다르게 지금은 과제를 넘어서 구조 설계의 재미와 디버깅 과정 중 학습의 기쁨으로 하고 있다.

학습만이 아니라 프레임워크를 제작이 최종 목표였으나 문득 어떤 것을 구현하고 있었는지 구체적인 목표가 없음을 깨달았다. 갈망하던 그 목표는 곧 개발의 방향과 사용할 네트워크 아키텍처였다. 이제는 단순히 서버와 클라이언트끼리 핑만 보내는 추상적인 것이 아니라 로그인, 매치메이킹, 전투 시뮬레이션, 경매장 시스템, 랭킹 시스템 등의 실용적이며 차원 높은 기술을 사용하며 구현하고 싶어졌다.

많은 시간과 노력이 요구되는걸 알면서 동시에 설렌다. 

## [simple unity-network](https://github.com/PioneerRedwood/boost-asio-network/tree/main/unity-network)

## Connection into Unity client

시작하지 않았다

- Unity client simple login/lobby module
  - Simple login transaction(Handling encoded/decoded data)
  - Lobby(Matchmaking, Room system)
- boost-asio login/lobby server
  - MySQL, Redis

## redis installed WSL2 Ubuntu 20.04

## Architecture simple logic

<img src="https://user-images.githubusercontent.com/45554623/136512119-ea6fc152-69eb-47d0-ad4a-69bd1712e8e6.png">

1. 클라이언트는 처음에 로그인 서버에 접속을 한다. (TCP)
2. 로그인이 성공한다면 로비 서버에 입장한다. (TCP)
3. 매치를 시작하게 되면 로비 서버에서는 매칭 큐에 넣어둔다. (TCP)
4. 매칭이 성공하면 1:1 배틀 서버 (UDP)에 입장한다.
5. 배틀이 끝나면 로비로 다시 돌아온다.
6. [3]에서 다시 시작

