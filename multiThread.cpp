#include<iostream>
#include<thread>
#include<future>
#include<cmath>
#include<vector>
#include<cstdlib>
#include<chrono>
#include<ctime>
#include<atomic>
#include<mutex>
#include<string>
#include<list>
#include<cstring>
#include<condition_variable>

//为什么用多线程来做服务器
//1.当前的机器的单核红利已经结束
//2.多线程拥有自身的优势
//3.API日益成熟，操作系统和标准库都已经支持了多线程
//要在数据IO和计算找到平衡点
//process 进程
//thread 线程

//可能导致的问题
//1.死锁
//2.乱序
//3.并发访问数据造成的问题
//4.低效率

//c++给我们带来的新概念
//1.高阶端口：（async，future）
//2.低阶端口：（thread，mutex）
//新线程中做的事情

/*111111
void helloWorld() {
	std::cout << "hello world\n";
}

int main() {
	//开启一个线程
	std::thread t(helloWorld);
	std::cout << "hello main thread\n";

	//线程的终结
	t.join();

	system("pause");
	return 0;
}
*/

/*222222
double caculate(double v) {
	if (v <= 0)
		return v;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return sqrt((v*v + sqrt((v - 5)*(v + 2.5)) / 2.0) / v);
}

template<typename Iter,typename Fun>
double visitRange(std::thread::id id,Iter iterBegin, Iter iterEnd, Fun func) {
	if (id == std::this_thread::get_id())
		std::cout << "hello main thread\n";
	else
		std::cout << "hello work thread\n";
	double v = 0;
	for (auto iter = iterBegin; iter != iterEnd; ++iter) {
		v += func(*iter);
	}
	return v;
}

int main() {
	auto mainThreadId = std::this_thread::get_id();//得到所在线程的id
	std::vector<double> v;
	for (int i = 0; i < 1000; ++i) {
		v.push_back(rand());
	}
	std::cout << v.size() << std::endl;
	double value = 0.0;

	auto nowc = clock();
	for (auto& info : v) {
		value += caculate(info);
	}
	auto finishc = clock();
	std::cout << "single thread:"<<value <<"\tused:"<<(finishc-nowc)<< std::endl;

	auto nowc_multi = clock();
	auto iter = v.begin() + (v.size() / 2);
	double anotherv = 0.0;
	auto iterEnd = v.end();
	//std::function
	std::thread s([&anotherv, mainThreadId, iter, iterEnd]() {
		anotherv = visitRange(mainThreadId, iter, iterEnd, caculate);
	});
	auto id = s.get_id();
	auto halfv = visitRange(mainThreadId, v.begin(), iter, caculate);
	s.join();
	auto finishc_multi = clock();

	std::cout <<"multi thread:"<< (halfv + anotherv)<<"\tused:"<<(finishc_multi-nowc_multi) << std::endl;


	system("pause");
	return 0;
}
*/

/*333333
class Counter {
public:
	void addCount() { m_count++;}
	int count() const { return m_count; }
	Counter():m_count{0} {}
private:
	int m_count;
};

int work(int a) {
	//do some thing
	return a + a;
}

template<class Iter>
void realWork(Counter& c,double& totalValue,Iter b,Iter e) {
	for (; b != e; ++b) {
		totalValue += work(*b);
		c.addCount();
	}
}

int main() {
	unsigned int n = std::thread::hardware_concurrency();//查看当前电脑支持几个线程
	std::cout << n << " concurrent threads are supported.\n";
	std::vector<int> vec;
	double totalValue = 0;
	for (int i = 0; i < 100000; ++i) {
		vec.push_back(rand() % 100);
	}
	Counter counter;
	for (auto v : vec) {
		totalValue += work(v);
		counter.addCount();
	}
	//do work

	std::cout << "total times:" << counter.count() << "\t" << totalValue << std::endl;

	totalValue = 0;
	Counter counter2;
	auto iter = vec.begin() + (vec.size() / 3);
	auto iter2 = vec.begin() + (vec.size() / 3 * 2);
	auto end = vec.end();
	std::thread b([&counter2, &totalValue, iter, iter2] {
			realWork(counter2, totalValue, iter, iter2);
		}
	);
	Counter c3;
	double totalC = 0;
	std::thread c([&c3, &totalC, iter2, end] {
			realWork(c3, totalC, iter2, end);
		}
	);

	Counter c4;
	double totalM = 0;
	realWork(c4, totalM, vec.begin(), iter);

	b.join();
	c.join();
	auto realTotalCount = counter2.count() + c3.count() + c4.count();
	auto realTotalValue = totalValue + totalM + totalC;
	std::cout << "total times use multithread: " << realTotalCount << "\t" << realTotalValue << std::endl;

	system("pause");
	return 0;
}
*/

//1.如果没有必要的话，线程间不要共享资源
/*444444
void printAll(int a, int b, int c) {
	std::cout << a << " " << b << " " << c << std::endl;
}

void testThreadInit() {
	int a = 3;
	int b = 4;
	int c = 5;
	std::thread t([=] {printAll(a, b, c); });
	t.join();
	std::thread t2(printAll, a, b, c);
	t2.join();
}

int main() {
	testThreadInit();

	system("pause");
	return 0;
}
*/

/*555555
template<typename T>
class Lock {
public:
	Lock(T& mutex) :m_mutex(mutex) { m_mutex.lock(); }
	~Lock() { m_mutex.unlock(); }

private:
	T& m_mutex;
};

class Counter {
public:
	void addCountAndResource(int r) {
		//m_mutex.lock();
		Lock<std::mutex> lock(m_mutex);
		addCount();
		addResource(r);
		//m_mutex.unlock();
	}
	int count() const { return m_count; }
	int aveResource() {
		Lock<std::mutex> lock(m_mutex);
		//m_mutex.lock();
		if (m_count == 0) {
			//m_mutex.unlock();
			return 1;
		}
		//auto r = m_totalResource / m_count;
		//m_mutex.unlock();
		//return r;
		return m_totalResource / m_count;
	}
	Counter():m_count{0},m_totalResource{0} {}
	//在此之间的代码在一个线程下跑
	//void lockMutex() { m_mutex.lock(); }
	//void unlockMutex() { m_mutex.unlock(); }

private:
	void addResource(int r) { m_totalResource++; }
	void addCount() { m_count++; }
	std::atomic<int> m_count;
	std::atomic<int> m_totalResource;
	std::mutex m_mutex;
};

int work(int a) {
	return a + a;
}
template<class Iter>
void realWork(Counter &c, double &totalValue, Iter b, Iter e) {
	for (; b != e; ++b) {
		totalValue += work(*b);
		c.addCountAndResource(1);
	}
}
void printAll(int a, int b, int c) {
	std::cout << a << " " << b << " " << c << std::endl;
}

void add(int a, int b, int& c) {
	c = a + b;
}

void printString(const std::string& info, const std::string& info2) {
	std::cout << "hello " << info << " " << &info2 << std::endl;
}

void testThreadInit() {
	int a = 3;
	int b = 4;
	int c = 5;
	std::thread t([=] {printAll(a, b, c); });
	t.join();
	std::thread t2(printAll, a, b, c);
	t2.join();

	std::thread t3([=, &c] {add(a, b, c); });
	t3.join();
	std::cout << "after add " << c << std::endl;
	std::thread t4(add, a, b, std::ref(c));
	t4.join();

	std::string abc("abc");
	std::string def("def");

	std::thread t5([&] {printString(abc, def); });
	t5.join();
	std::thread t6(printString, abc, def);
	t6.join();
	std::thread t7(printString, std::cref(abc), std::cref(def));
	t7.join();
}

bool printStep(Counter &c, int maxCount) {
	auto count = c.count();
	//c.lockMutex();
	auto ave = c.aveResource();
	if (ave != 1) std::cout << "has bad thing happened\n";
	//c.unlockMutex();
	if (count == maxCount) {
		std::cout << "ok finished \n";
		return true;
	}
	return false;
}

int main() {
	unsigned int n = std::thread::hardware_concurrency();
	std::cout << n << " concurrent threads are supported.\n";
	std::vector<int> vec;
	double totalValue = 0;
	for (int i = 0; i < 100000; ++i) {
		vec.push_back(rand() % 100);
	}
	Counter counter;
	realWork(counter, totalValue, vec.begin(), vec.end());

	std::cout << "total times: " << counter.count() << " " << totalValue << std::endl;
	totalValue = 0;
	Counter counter2;
	std::thread printCount([&counter2] {
		while (!printStep(counter2, 100000));
	});
	
	auto iter = vec.begin() + (vec.size() / 3);
	auto iter2 = vec.begin() + (vec.size() / 3 * 2);
	std::thread b([&counter2, &totalValue, iter, iter2] {
		realWork(counter2, totalValue, iter, iter2);
	});
		auto end = vec.end();
	double totalC = 0;
	std::thread c([&counter2, &totalValue, iter, end] {
		realWork(counter2, totalValue, iter, end);
	});

	b.join();
	c.join();
	std::cout << "total times use multithread: " << counter2.count() << " " << totalValue << std::endl;

	system("pause");
	return 0;
}
*/

/*666666
struct BankAccount {
	BankAccount(int b) : Balance(b) {}
	int Balance;
	std::mutex Mutex;
};

template<typename T>
class Lock {
public:
	Lock(T& m) :m_mutex(m) { m_mutex.lock(); }
	~Lock() { m_mutex.unlock(); }
private:
	T& m_mutex;
};

void transferMoney(BankAccount& a, BankAccount& b, int money) {
	std::lock(a.Mutex, b.Mutex//......);
	std::lock_guard<std::mutex> lockA(a.Mutex, std::adopt_lock);
	std::lock_guard<std::mutex> lockB(b.Mutex, std::adopt_lock);

	if (a.Balance <= money) {
		return;
	}
	a.Balance -= money;
	b.Balance += money;


	//死锁
	std::lock_guard<std::mutex> lock(a.Mutex);
	std::lock_guard<std::mutex> lock(b.Mutex);
	if (a.Balance <= money) 
		return;
	a.Balance -= money;
	b.Balance += money;
	
}
*/

/*777777
void joinWorker() {

}

class Obj {
public:
	Obj() { std::cout << "hello "; }
	~Obj() { std::cout << "world"; }
};

void detachWorker() {
	Obj obj;
}

int main() {
	std::thread j(joinWorker);
	std::thread w(detachWorker);
	w.detach();
	j.join();
	return 0;
}
*/

/*888888
std::mutex mutex;
std::atomic<bool> ready{ false };

void worker(int i) {
	while (!ready) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//std::this_thread::yield();
		//do nothing just wait
	}
	std::lock_guard<std::mutex> lock(mutex);
	std::cout << "hello world " << i << "\n";
	
}
int main() {
	const auto threadCount = 4;
	std::vector<std::thread> pool;
	for (int i = 0; i < threadCount; ++i) {
		pool.emplace_back(worker,i);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
	ready = true;

	for (auto& v : pool)
		v.join();

	system("pause");
	return 0;
}
*/

/*999999
class Message {
public:
	const std::string& data() const { return m_data; }
	Message(std::string d):m_data(std::move(d)) {}
	
private:
	std::string m_data;
};

std::atomic<int> totalSize{ 0 };
std::list<Message> globalList;

void worker() {
	while (!globalList.empty()) {
		auto iter = globalList.begin();
		//do real work and arase it
		totalSize += strlen((*iter).data().c_str());
		globalList.erase(iter);
	}
}

int main() {
	for (int i = 0; i < 10000; ++i) {
		globalList.push_back("this is a test" + std::to_string(i));
	}
	worker();
	for (int i = 0; i < 10000; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		globalList.push_back(std::string("second"));
	}
	worker();
	std::cout << totalSize << std::endl;

	system("pause");
	return 0;
}
*/
class Message {
public:
	const std::string& data() const { return m_data; }
	Message(std::string d=std::string()) :m_data(std::move(d)) {}

private:
	std::string m_data;
};

std::atomic<int> totalSize = { 0 };
std::mutex mutex;
std::condition_variable cv;
std::atomic<bool> ready{ false };
bool quit{ false };
std::list<Message> globalList;

void worker(int i) {
	while (!ready) {
	}
	Message msg;
	while (!quit) {
		{
			//std::lock_guard<std::mutex> lock(mutex);
			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock, [] {return quit || !globalList.empty(); });
			if (quit)
				return;
			auto iter = globalList.begin();
			msg = std::move(*iter);
			globalList.erase(iter);
		}
		totalSize += strlen(msg.data().c_str());
	}
}

int main() {
	const auto threadCount = 4;
	for (int i = 0; i < 10000; ++i) {
		globalList.push_back("this is a test" + std::to_string(i));
	}
	std::vector<std::thread> pool;
	for (int i = 0; i < threadCount; ++i) {
		pool.emplace_back(worker, i);
	}
	ready = true;
	for (int i = 0; i < 10000; ++i) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		globalList.push_back(std::string("second"));
		cv.notify_one();
	}
	while (true) {
		std::lock_guard<std::mutex> lock(mutex);
		if (globalList.empty()) {
			quit = true;
			cv.notify_all();
			break;
		}
	}
	for (auto &v : pool) {
		if (v.joinable())
			v.join();
	}
	
	std::cout<<"total size is " << totalSize << std::endl;

	system("pause");
	return 0;
}