# boost-asio-network
boost::asio를 사용한 게임 네트워크 학습 겸 최종 프레임워크 제작이 목표 😎

server/client 양쪽 데이터 주고 받는 식의 프레임워크 설계/구현
boost::asio + Google protobuf 사용 예정

### 2021-08-25 boost::asio Timer tutorial
마지막 튜토리얼(timer_5) 하다 아래 문제가 발생
```
...
1>LINK : fatal error LNK1104: 'libboost_thread-vc142-mt-sgd-x32-1_77.lib' 파일을 열 수 없습니다.
...
```
boost include한 상태로 프로젝트를 빌드하는데 동적파일을 링크하려는 시도가 생겼다.
"C/C++ - 코드생성 - 런타임 라이브러리" 설정에서 DLL을 사용하지 않는 것으로 해도 안됨

알아보고 있는 상태
https://stackoverflow.com/questions/13042561/fatal-error-lnk1104-cannot-open-file-libboost-system-vc110-mt-gd-1-51-lib
https://blog.csdn.net/aoxuestudy/article/details/115427090

추신; boost를 따로 다운, CMAKE로 빌드하고 나서 include 파일을 해당 프로젝트에 외부 경로로 등록해보면 될 거 같다.
- 해결 뒤 다음은 https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tutdaytime1.html