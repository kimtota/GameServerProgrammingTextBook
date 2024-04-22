#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <mutex>

using namespace std;

const int MaxCount = 150000;
const int ThreadCount = 4;

bool IsPrimeNumber(int number)
{
	if (number == 1)
		return false;
	if (number == 2 || number == 3)
		return true;
	for (int i = 2; i < number - 1; i++)
	{
		if ((number % i) == 0)
			return false;
	}
	return true;
}

void PrintNumbers(const vector<int>& primes)
{
	for (int v : primes)
	{
		cout << v << endl;
	}
}

int main()
{
	int num = 1;
	recursive_mutex num_mutex; // num의 뮤텍스  
	
	vector<int> primes;
	recursive_mutex primes_mutex; // primes의 뮤텍스

	auto t0 = chrono::system_clock::now();

	vector<shared_ptr<thread>> threads; // 작동할 워커 스레드

	for (int i = 0; i < ThreadCount; i++)
	{
		shared_ptr<thread> thread(new thread([&]() {
			// 각 스레드의 메인 함수
			// 값을 가져올 수 있으면 루프를 돈다.
			while (true)
			{
				int n;
				{
					lock_guard<recursive_mutex> num_lock(num_mutex); // 잠금을 위한 지역변수 추가
					n = num;
					num++;
				}
				
				if (n >= MaxCount)
					break;

				if (IsPrimeNumber(n))
				{
					lock_guard<recursive_mutex> primes_lock(primes_mutex); // 잠금을 위한 지역변수 추가
					primes.push_back(n);
				}
			}
			}));
		threads.push_back(thread); // 스레드 객체를 일단 갖고 있는다.
	}

	// 모든 스레드가 일을 마칠 때까지 기다린다.
	for (auto thread : threads)
	{
		thread->join();
	}

	auto t1 = chrono::system_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();
	cout << "Took " << duration << " milliseconds." << endl;

	//PrintNumbers(primes);

	return 0;

	// 하지만 이 소스는 에러가 난다. 
	// 그 이유는 여러 스레드가 동시에 num을 읽고 증가시키기 때문이다. 
}