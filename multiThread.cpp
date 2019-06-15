#include<iostream>
#include<thread>
#include<future>
#include<cmath>
#include<vector>
#include<cstdlib>
#include<chrono>
#include<ctime>

//Ϊʲô�ö��߳�����������
//1.��ǰ�Ļ����ĵ��˺����Ѿ�����
//2.���߳�ӵ�����������
//3.API������죬����ϵͳ�ͱ�׼�ⶼ�Ѿ�֧���˶��߳�
//Ҫ������IO�ͼ����ҵ�ƽ���
//process ����
//thread �߳�

//���ܵ��µ�����
//1.����
//2.����
//3.��������������ɵ�����
//4.��Ч��

//c++�����Ǵ������¸���
//1.�߽׶˿ڣ���async��future��
//2.�ͽ׶˿ڣ���thread��mutex��
//���߳�����������

/*111111
void helloWorld() {
	std::cout << "hello world\n";
}

int main() {
	//����һ���߳�
	std::thread t(helloWorld);
	std::cout << "hello main thread\n";

	//�̵߳��ս�
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
	auto mainThreadId = std::this_thread::get_id();//�õ������̵߳�id
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

//333333
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

int main() {
	unsigned int n = std::thread::hardware_concurrency();//�鿴��ǰ����֧�ּ����߳�
	std::cout << n << "\tconcurrent threads are supported.\n";
	std::vector<int> vec;
	double totalValue = 0;
	for (int i = 0; i < 10000000; ++i) {
		vec.push_back(rand() % 100);
	}
	Counter counter;
	for (auto v : vec) {
		totalValue += work(v);
		counter.addCount();
	}
	//do work

	std::cout << "total times:" << counter.count() << std::endl;


	system("pause");
	return 0;
}