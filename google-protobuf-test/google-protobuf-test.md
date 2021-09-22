# google-protobuf-test

[protocolbuffers](https://github.com/protocolbuffers/protobuf) 사용한 간단 데이터 직렬화

#### #1일차 2021-09-21

- 프로젝트 파일에 분명히 include/lib 디렉터리 경로를 등록해놨음에도 불구하고 인식을 못 하는 듯하다.
  - 이유를 알 수가 없다.. 😐

#### #2일차 2021-09-22

- [protobuf/cmake/README.md](protobuf/cmake/README.md) 해당 문서를 따라 다시 빌드하고 컴파일을 시도했으나 여전히 링킹 오류가 뜬다. 
  - Debug, Release 별로 따로 빌드했는데도 안됐다.
  - 뭐가 문제인지 도저히 찾아낼 수가 없다. 포기하게 만들었던 예전의 악몽이 떠오른다.