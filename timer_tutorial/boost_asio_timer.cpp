/**
* Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
* 
* Distributed under the Boost Software License, Version 1.0. (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tuttimer1.html
/// </summary>
void timer_1()
{
	// 입출력 처리에선 필수적으로 io_context를 생성해야함
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

	t.wait();

	std::cout << "hello world! timer_1() func executed\n";
}

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tuttimer2.html
/// </summary>
/// <param name="error"></param>
void print_2(const boost::system::error_code& error)
{
	std::cout << "Hello world! timer_2() func executed\n";
}

void timer_2()
{
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

	// 비동기 핸들러 대기
	// 핸들러의 함수 시그니처
	// 반드시 boost::system::error_code& error 인자가 존재해야함
	t.async_wait(&print_2);

	io.run();
}

/// <summary>
/// 비동기 핸들러 with params
/// </summary>
/// <param name="">에러코드(반드시 있어야함)</param>
/// <param name="t">전달받은 타이머 포인터</param>
/// <param name="count">전달받은 카운트</param>
void print_3(const boost::system::error_code& /*e*/,
	boost::asio::steady_timer* t, int* count)
{
	if (*count < 5)
	{
		std::cout << *count << '\n';
		++(*count);

		t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));

		t->async_wait(boost::bind(print_3,
			boost::asio::placeholders::error, t, count));
	}
}

void timer_3()
{
	boost::asio::io_context io;
	int count = 0;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

	t.async_wait(boost::bind(print_3,
		boost::asio::placeholders::error, &t, &count));

	io.run();
	std::cout << "Final count is " << count << '\n';
}

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tuttimer4.html
/// </summary>
class printer
{
public:
	printer(boost::asio::io_context& io)
		: timer_(io, boost::asio::chrono::seconds(1)), count_(0)
	{
		timer_.async_wait(boost::bind(&printer::print, this));
	}
	~printer()
	{
		std::cout << "Final count is " << count_ << '\n';
	}

	void print()
	{
		if (count_ < 5)
		{
			std::cout << count_ << '\n';
			++count_;

			timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
			timer_.async_wait(boost::bind(&printer::print, this));
		}
	}

private:
	boost::asio::steady_timer timer_;
	int count_;
};

void timer_4()
{
	boost::asio::io_context io;
	printer p(io);
	io.run();
}

/// <summary>
/// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/tutorial/tuttimer5.html
/// </summary>
class printer_extended
{
public:
	printer_extended(boost::asio::io_context& io)
		: strand_(boost::asio::make_strand(io)),
		timer1_(io, boost::asio::chrono::seconds(1)),
		timer2_(io, boost::asio::chrono::seconds(1)),
		count_(0)
	{
		timer1_.async_wait(boost::asio::bind_executor(strand_,
			boost::bind(&printer_extended::print1, this)));

		timer2_.async_wait(boost::asio::bind_executor(strand_,
			boost::bind(&printer_extended::print2, this)));
	}
	~printer_extended()
	{
		std::cout << "Final count is " << count_ << '\n';
	}

	void print1()
	{
		if (count_ < 10)
		{
			std::cout << "Timer 1: " << count_ << '\n';
			++count_;

			timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
			timer1_.async_wait(boost::asio::bind_executor(strand_,
				boost::bind(&printer_extended::print1, this)));
		}
	}
	
	void print2()
	{
		if (count_ < 10)
		{
			std::cout << "Timer 2: " << count_ << '\n';
			++count_;

			timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
			timer2_.async_wait(boost::asio::bind_executor(strand_,
				boost::bind(&printer_extended::print2, this)));
		}
	}

private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	boost::asio::steady_timer timer1_;
	boost::asio::steady_timer timer2_;
	int count_;
};

void timer_5()
{
	boost::asio::io_context io;
	printer_extended p(io);
	boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
	io.run();
	t.join();
}

#if 0
int main()
{
	//timer_1();
	//timer_2();
	//timer_3();
	//timer_4();
	timer_5();
}
#endif