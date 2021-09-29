# Boost.AsioDebuggingDocument

버그란, 예상치 못한 결과 혹은 런타임 과정에서 발생하는 에러를 모두 포함.

머릿속의 생각만으로 눈에 보이는 화면만으로 모든 것을 추적할 수 없다. 생각을 정리하고 글로 남기며 논리적으로 추론하는 것이 내게 있어서 필수적임을 최근에서야 알게 됐다.



## 2021-09-16 #1

#### 문제점 #1

- 현재 연결은 원격 호스트에 의해 강제로 끊겼습니다
- 현재 연결은 사용자의 호스트 시스템의 소프트웨어의 의해 중단되었습니다

```C++
// boost::asio::execution::any_executor.hpp 일부분
#if !defined(BOOST_ASIO_NO_TYPEID)
  const std::type_info& target_type() const
#else // !defined(BOOST_ASIO_NO_TYPEID)
  const void* target_type() const
#endif // !defined(BOOST_ASIO_NO_TYPEID)
  {
    return target_fns_->target_type(); // Access 
  }
```

위는 shared_ptr로 선언된 것의 접근할 수 없는 부분을 접근하려고 시도했을때 발생하는 런타임에러인데 boost-asio-network-server/client에서 구현했을 때는 테크니컬 도서에서 참고했던 매크로를 떠올렸다. 

```c++
#define BIND(x)				boost::bind(&self_type::x, shared_from_this())
class connection
{ ...  public using self_type = connection; ... }
```

해당 매크로는 인자로 x를 받는데 이것은 함수 포인터이며 boost::bind() 함수를 호출한다. bind 함수에서 주목해야할 부분은 바로 두번째 인자인데 shared_ptr로 생성된 클래스의 함수를 다루기 위해선 this를 사용할 때 주의를 기울여야 한다. 위 엑세스 위반 에러가 생겨난 원인은 바로 아래 코드이다. 

```
void async_read(socket_, boost::asio::buffer(read_buffer_),
	// completion condition
	[this](const err& error, size_t bytes) -> bool {
		if (error) { return 0; }
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') 
			< read_buffer_ + bytes;
		return found ? 0 : 1;
	},
	// handler			
	boost::bind(&connection::on_read, this, _1, _2)); 				// Unexpected ERROR!
	boost::bind(&connection::on_read, shared_from_this(), _1, _2)); // OK!
```

추가로 boost::async 함수들에 람다 함수를 콜백함수로 등록할 수 있다. 람다 클로저 내에서 클래스 멤버에 접근할 수는 있어도 shared_ptr로 선언된 상태라면 this를 쓰는 것은 예상할 수 없는 결과를 초래한다. 람다를 보기에 깔끔하다는 이유로 사용했지만 디버깅도 힘들 뿐더러 나중엔 오히려 수정하는 데에도 어려움이 있었다. 

참고로 현재 해당 매크로는 쓰지 않을 예정이다.



**2021-09-29 람다에 대한 새로운 생각 추가**

- C++ 람다에 대해서 정확한 식견이 없어서 전에 이러한 어려움을 겪었다. 여러 글을 참고하고 Effective Modern C++ 도서([해당 도서의 pdf 웹 파일](https://moodle.ufsc.br/pluginfile.php/2377667/mod_resource/content/0/Effective_Modern_C__.pdf))를 참고한 결과. 람다를 잘 알고 사용하면 코드 가독성과 유지보수에 있어서 많은 이점을 가져다줄 수 있음을 후에서야 알게됐다.
- 하지만 위 맥락에 있어서 디버깅이 어려웠던 부분은 사실이었기 때문에 글을 지우지 않고 참고형식으로 추가하는게 마땅하다고 생각했다.

```c++
void accept()
{
    //boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
    //	context_, recv_deque_, curr_id_, conn_map_, rooms_);
    // 
    //acceptor_.async_accept(
    //	boost::bind(&server::on_accept, this, boost::asio::placeholders::error, conn_));

    acceptor_.async_accept(
        [this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket)
        {
            if (!error)
            {
                std::cout << "new connection: " << socket.remote_endpoint() << "\n";

                boost::shared_ptr<connection> conn_ = boost::make_shared<connection>(
                    context_, recv_deque_, curr_id_, std::move(socket), conn_map_, rooms_);

                conn_map_.insert(std::make_pair(curr_id_++, conn_));
                conn_->start();

                std::cout << "[SERVER] connected clients ";
                for (const auto iter : conn_map_)
                {
                    std::cout << iter.first << " ";
                }
                std::cout << "\n";
            }
            else
            {
                std::cerr << "[SERVER] ERROR " << error.message() << "\n";
                return;
            }
            accept();
        });
}
```



#### 문제점 #2

- boost::asio::io_context를 서버/클라이언트 클래스 멤버 변수로 선언하고 실행하면 비정상 종료되는 버그

- 서버에서 클라이언트로 메시지를 보내지만 클라이언트가 제대로 받지 못하는 버그
  - 연결이 안돼있으면 아예 통신을 할 수 없으며 다른 버그로 표시돼야함

```c++
size_t on_read_completion(const err& error, size_t bytes)
{
    if (error) { return 0; }
	// char* std::find(char* First, char* Last, const char& Val)
    // First ~ Last 내에서 Val 값의 위치(char형 포인터)를 반환
    // 읽어온 데이터 안에 Val이 포함돼있다면 true 아니면 false
    // true는 읽기를 마친 상태를 의미, false는 반대
    bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') 
        < read_buffer_ + bytes;
    return found ? 0 : 1;
}

void read()
{
	if (socket_.is_open())
	{
        // 읽기 전 버퍼 초기화
		std::fill_n(read_buffer_, MAX_MSG, '\0');
		async_read(socket_, boost::asio::buffer(read_buffer_),
			boost::bind(&connection::on_read_completion, shared_from_this(), _1, _2),
			boost::bind(&connection::on_read, shared_from_this(), _1, _2));

		//socket_.async_read_some(boost::asio::buffer(read_buffer_, MAX_MSG),
		//	boost::bind(&connection::on_read_completion, shared_from_this(), _1, _2),
		//	boost::bind(&connection::on_read, shared_from_this(), _1, _2));
	}
}
```

서버, 클라이언트 양측에서 공동으로 사용하고 있는 소켓으로부터 비동기 읽기 실행 부분이다. 서버의 시그니처를 설명하진 않겠다. async_read의 3번째 , async_read_some 2번째 매개변수는 지금까지 읽어온 내용 중 어느 조건에 부합하는지 처리하는 완료 함수(테크니컬 도서에선 completion function 으로 소개됨)다. 추가로 완료 함수는 선택사항이다.

- async_read 에서 EOF 
  - 찾지 못함
  - 2021-09-29 추가 예상치 못한 비동기 읽기 종료시 발생하는 에러



## 2021-09-29 #2

#### 문제점 #1

- 클라이언트에서 서버로 메시지를 전송 작업이 수행이 되고 서버에선 받은 메시지를 콘솔에 출력한다. 그 즉시 서버는 받은 메시지에 따라 클라이언트에 적절한 메시지를 전송한다. 
- 여기서 서버로부터 수신된 메시지의 형태가 매우 이상하다.

#### 디버깅

- 클라이언트에서 수신된 메시지를 읽는 부분에서는 on_read_completion()이라는 함수로 "\n" 문자가 올 때까지 읽는데, 무작위로 코드를 수정하다가 서버에서 비동기 쓰기 함수에서 보내는 내용 끝에 "\n" 문자를 붙이는 것을 삭제했다. 



#### ~~문제점 #2~~

- ~~설계상 클라이언트는 키보드 입력에 따라 서버에 전송하는 메시지가 다르다. 서버에서 수신은 빠른데 다시 돌아오는 응답에는 딜레이가 있다.~~



#### ~~문제점 #3~~

- ~~클라이언트 연결이 5-6개 됐을 때 알 수 없는 오류로 인해 종료될 수 있다.~~



#### 문제점 #4

- 서버와 클라이언트 양쪽 모두 boost::asio::io_context(이하 io_context) 인스턴스의 run() 함수를 실행할 스레드를 어디에 만드는게 적합할까?
  - 현재 방식에서 서버는 서버 인스턴스를 생성하는 main 함수에서 io_context 객체를 참조 전달해서 생성하여 서버 인스턴스 내부에 있는 스레드로 참조 전달된 io_context 객체의 run()을 호출한다. 클라이언트 역시 동일한 방식으로 수행하지만 클라이언트에서는 무한 루프로 연결이 종료되기까지 사용자의 키보드 입력에 따라 서버에 데이터를 전송한다. 
  - 곧바로 종료됨, io_context의 참조가 전달된 boost::asio::steady_timer가 있는데도 종료됨

#### 디버깅

- 서버는 종료되고 클라이언트는 종료되지 않는 이유는 그 둘의 차이점에 있다. 클라이언트는 main 함수 내에 일정 조건에 의해 끝나는 무한 루프 작업이 존재하기 때문에 스레드가 종료되지 않는다.
- io_context가 존재하는 main 내에서 더이상의 작업이 없는 경우 main 스레드가 종료되면서 프로그램 내 스레드는 모두 종료된다. 이로 인해 서버는 아무리 io_context에 비동기 호출이 존재한다해도 종료되는 것이다.



#### 문제점 #5

- 서버/클라이언트의 소켓 및 버퍼 래퍼 클래스인 connection에서 on_message() 내부에서 수신된 메시지를 1차적으로 처리해서 수신 메시지 덱에 저장하고 일정 주기마다 수신 덱을 점검하는 서버/클라이언트의 update()에서 2차적으로  처리한다.
- 과연 이 구조가 합리적일까? 전문가에게 물어보자



#### 문제점 #6

- 현재 서버 측에 세션 룸 생성 요청 시 처리하는 과정에서 엑세스 위반 예외를 마주했다. - std::unique_ptr 사용으로 해결했으나 설계가 여전히 마음에 안듬
- 세션 룸을 생성하고 나서 조회까지는 가능하지만 세션을 소유하고 있는 클라이언트 종료시 다른 클라이언트까지 종료하는 버그 발견

