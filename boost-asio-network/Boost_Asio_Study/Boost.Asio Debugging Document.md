# Boost.Asio Debugging Document

버그란, 예상치 못한 결과 혹은 런타임 과정에서 발생하는 에러를 모두 포함.



## 2021-09-16 #1

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

서버, 클라이언트 양측에서 공동으로 사용하고 있는 소켓으로부터 비동기 읽기 실행 부분이다. 서버의 시그니처를 설명하진 않겠다. async_read의 3번째 , async_read_some 2번째 매개변수는 지금까지 읽어온 내용 중 어느 조건에 부합하는지 처리하는 완료 함수(테크니컬 도서에선 completion function 으로 소개)다. 완료 함수는 선택사항이다.

- async_read 에서 EOF 
  - 찾지 못함