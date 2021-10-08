// studying based on https://dens.website/tutorials/cpp-asio/multithreading
//					 https://dens.website/tutorials/cpp-asio/multithreading-2
// 이해할 수 있는 범위 내에서 쉽게 작성하려고 노력했다
// 번역이 매끄럽지 못한 부분이 몇몇 있을 수 있다

#define BOOST_ASIO_NO_DEPRECATED
#include <thread>
#include <vector>
#include <iostream>

#include <boost/asio.hpp>

int main()
{
	// 비동기 작업은 OS의 어딘가에서 수행된다
	// 모든 completion handler는 context.run()이 수행되고 있는 스레드 중 frist free한 스레드에서 호출된다
	// 다른 스레드에서 동작하지만 호출 자체는 직렬화(순서가 보장)되기 때문에 동기화는 필요하지 않다!
	{
		namespace io = boost::asio;
		using tcp = io::ip::tcp;
		using error_code = boost::system::error_code;

		// single-threaded approach
		io::io_context context;
		//context.run();

		std::vector<std::thread> threads;
		auto count = std::thread::hardware_concurrency() * 2;

		for (int n = 0; n < count; ++n)
		{
			threads.emplace_back([&]
				{
					context.run();
				});
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}


		class session
		{
			session(io::io_context& context)
				:
				socket(context), read(context), write(context)
			{}

			void async_read()
			{
				io::async_read(socket, io::buffer(read_buffer), 
					io::bind_executor(read, [&](error_code error, std::size_t bytes_transferred)
					{
						if (!error)
						{
							// ...
							async_read();
						}
					}));
			}

			void async_write()
			{
				io::async_write(socket, io::buffer(write_buffer), 
					io::bind_executor(write, [&](error_code error, std::size_t bytes_transferred)
				{
					if (!error)
					{
						// ...
						async_write();
					}
				}));
			}

			void do_some()
			{
				io::post(read, []
					{
						std::cout << "We're inside a read sequence, it's safe to access a read-related data here!\n";
					}
				);
			}

		private:
			tcp::socket socket;

			std::vector<char> read_buffer;
			std::vector<char> write_buffer;

			io::io_context::strand read;
			io::io_context::strand write;
		};
		return 0;

		// 비동기 읽기와 쓰기는 동일한 컨텍스트에서 실행되지만 실제 수행되는 스레드는 다르다
		// boost::asio::bind_executor는 completion handler를 strand에 묶어주는 함수라고 할 수 있다
		// 이는 OS에서 자동으로 묶어줌
	}

	// 위는 하나의 context에 N개의 스레드가 수행하는 것이었다
	// context 자체가 로드 밸런싱을 진행하며 다음 handler가 어디에 쓰이는지 신경쓰지 않아도 됐다

	// 다른 실행 모델이 있다. N쌍의 "1context + 1thread"이다
	// 이 경우에 모든 스레드는 각 context의 인스턴스를 소유하고 있다
	// context group wrapper 클래스는 context의 수만큼의 인스턴스, work_guard 수만큼의 인스턴스, 스레드 인스턴스를 포함하고 있다

	{
		namespace io = boost::asio;
		using tcp = io::ip::tcp;
		using work_guard_type = io::executor_work_guard<io::io_context::executor_type>;
		using error_code = boost::system::error_code;

		class context_group
		{
		public:
			context_group(std::size_t size)
			{
				for (std::size_t n = 0; n < size; ++n)
				{
					contexts.emplace_back(std::make_shared<io::io_context>());
					guards.emplace_back(std::make_shared<work_guard_type>(contexts.back()->get_executor()));
				}
			}

			void run()
			{
				for (auto& context : contexts)
				{
					threads.emplace_back([&]
					{
						context->run();
					});
				}
			}

			// 여기서 밸런싱 알고리즘이 적용될 부분이라고 할 수 있다
			// 지금은 단순하게 Round Robin
			io::io_context& query()
			{
				return *contexts[index++ % contexts.size()];
			}

		private:

			std::vector<std::shared_ptr<io::io_context>> contexts;
			std::vector<std::shared_ptr<work_guard_type>> guards;
			std::vector<std::thread> threads;

			std::atomic<std::size_t> index = 0;
		};

		context_group group(std::thread::hardware_concurrency() * 2);
		tcp::socket socekt(group.query());
		group.run();

		return 0;

		// 플러스 요소
		//	strands 같은 동기화 도구를 사용할 필요가 없다 
		//	io_context 별로 스레드가 있기 때문에 같은 io_context에만 있다면 데이터 동기화가 필요하지 않다
		// 마이너스 요소
		//	socket, acceptor 등의 자원들은 io_context에 바운드(묶인 상태)되기 때문에
		//	기존 스레드가 살아있는 동안(lifetime)에 자원을 다시 바인드(rebind)하는 것은 불가능한다
		//	이는 io_context에 바인드된 모든 물체들은 싱글 스레드로만 동작할 것을 의미한다
		//	OS는 CPU를 활용할 때 스레드가 들어갈 가장 적절한 코어에 배정시키는데(이 부분 번역이 매끄럽지 못함)
		//	결국 하나의 코어만 100%상태가 되고 다른 것은 유휴 상태가 되는 결과를 마주할 수 있다
		
		// 이 자체가 마이너스 요소는 아니지만 
		//	1. 선택한 밸런싱 알고리즘이 부적절한 경우 마이너스 되거나
		//	2. 특정 사용 사례에 대한 실행 모델의 부적절한 사용 결과를 낳을 수 있다
		// 이전의 모델은 몇몇 특정 상황마다 적절한 밸런싱 알고리즘을 개발자가 구현해야하는 부분이다
	}
}