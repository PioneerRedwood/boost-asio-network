# MySQL for C++



## MySQL 설치하며 배운 것들

MySQL에 연결하는 방법이 언어별로 상이하기 때문에 정리합니다.

MySQL installer에서 설치한 Connector C++은 XDevApi은 원하는 기능과 차이가 있어서 제쳐두고 Legacy C++ API인 mysql/jdbc.h를 사용했습니다.

이번엔 Visual Studio 프로젝트 설정에서 디렉토리에 대한 환경변수를 설정해두어서 boost, mysql, protobuf 같은 자주 사용되는 디렉토리에 접근이 쉽도록 했습니다. Win + Pause/Break 키를 눌러 고급 시스템 설정 - 환경 변수 설정 - 시스템 변수에 추가했습니다.

<img src="https://user-images.githubusercontent.com/45554623/141924443-64b6909a-97b0-4176-bb7e-805e70665cc2.png">

예시로 boost를 사용하는 프로젝트 설정에서 추가 포함 디렉토리에 %SDK_DIR%\boost_1_77_0 넣어주고, 추가 라이브러리 디렉토리에는 %SDK_DIR%\boost_1_77_0\stage\lib을 넣어주면 인식할 수 있습니다.



## 외부 라이브러리를 적용할 때마다 만나는 Visual studio 링킹 에러😒 때문에 C++ 빌드 과정에 대해 정리(?)

<img src="https://user-images.githubusercontent.com/45554623/141924363-4c01d11a-8138-4211-acaa-7640843661bf.png">

[이미지 출처](https://www.google.com/url?sa=i&url=https%3A%2F%2Fsubscription.packtpub.com%2Fbook%2Fprogramming%2F9781789801491%2F1%2Fch01lvl1sec03%2Fthe-c-compilation-model&psig=AOvVaw3k3jsExeVrpsV3MpVS1kRl&ust=1637125584019000&source=images&cd=vfe&ved=0CAsQjRxqFwoTCPiY1ceOnPQCFQAAAAAdAAAAABAD)

링킹 에러에서 시작한 지식의 확장으로 .lib파일과 .dll 파일이 어떻게 다른 지에 대해 명확하고 자세하게 살펴보기 위해 시작됐습니다. 하지만 정적, 동적 라이브러리 파일을 어떻게 해석하는지 살펴보기 전에 C/C++의 소스가 빌드되는 과정을 다시 짚고갑니다.

### C++ 빌드 과정

1. 헤더, 소스 파일을 지정한 **컴파일러**를 통해 컴파일 과정을 수행하면 오브젝트 파일이 생성됩니다. 여기서 지정한 컴파일러란 Microsoft Visual Studio 내부 컴파일러나 gcc, g++ 등을 말합니다. 각 컴파일러마다 소스파일을 해석하는 방식이 다르며 이들은 C++0X ~ C++2X 등의 C++ 국제 표준을 준수하며 작성됩니다. 
2. 컴파일 과정에서 정적 라이브러리(.lib) 파일들이 호출됩니다. 
3. **오브젝트 파일**은 런타임 시에 사용될 인스턴스에 대한 메타 정보가 담겨있고 인간이 쉽게 읽을 수 없는 기계 코드에 가깝습니다. (클래스, 변수 등) 템플릿이 사용된 소스들은 컴파일 과정이 상대적으로 시간이 더 걸리는데 이는 오브젝트 파일에 담을 클래스의 인스턴스를 구현-생성하는 과정이 포함돼있기 때문입니다. 
   1. C++ 템플릿 프로그래밍과 관련한 지식은 여전히 배우고 있는 중입니다. 정확하지 않은 정보를 제공하는 것은 좋지 않다고 생각하기 때문에 근거를 밝힙니다. 위 결론은 제 개인적인 경험(?)과 [스택오버플로우 토론](https://stackoverflow.com/questions/5979723/do-templates-actually-have-to-be-compile-time-constructs)에 근거했습니다. 👌
4. 링커 과정에선 컴파일하여 생성된 여러 오브젝트 파일들과 동적 라이브러리(.dll) 파일들이 함께 엮이게 되어 실행 파일이 생성됩니다. 
   1. 추가적으로 C의 헤더파일의 경로와 C++ 표준 라이브러리의 경로는 다릅니다. 
      1. C 표준 헤더파일 경로 *C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt*
      2. C++ 표준 라이브러리 경로 *C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include*



### 링킹 에러를 고치기 위해





## 참고 🙏

[Visual Studio 환경 변수 세팅](https://clucle.tistory.com/entry/visual-studio-2019-c-mysql-connector80)

[MySQL Connector C++ 8.0 개발 문서](https://dev.mysql.com/doc/dev/connector-cpp/8.0/)

[MySQL Connector C++ 8.0 jdbc 참고 문서](https://dev.mysql.com/doc/dev/connector-cpp/8.0/jdbc_ref.html)

[LIB vs DLL](http://www.differencebetween.net/technology/difference-between-lib-and-dll/), [스택오버플로우 lib vs dll](https://stackoverflow.com/questions/913691/dll-and-lib-files-what-and-why) 

