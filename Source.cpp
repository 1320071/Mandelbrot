#include "Creation.h"

typedef std::chrono::steady_clock the_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::complex;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::vector;
using std::thread;


int main(int argc, char *argv[])
{
	Creation create;
	//for (int x = 0; x < 50; x++) << for testing
	//the_clock::time_point start = the_clock::now();
	
	create.SetSliceSize((thread::hardware_concurrency()));
	create.ComputeThreads((thread::hardware_concurrency()));

	//create.WriteTGA("Test.tga");
	/* the_clock::time_point end = the_clock::now();
	auto time_taken = duration_cast<milliseconds>(end - start).count();
	cout << time_taken << endl; */
	
	cin.ignore(); // Stops it just closing instantly when done

	return 0;
}