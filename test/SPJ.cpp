// TMP dir
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
using namespace std;

const string TMP = "/test/tmp";

const int AC = 0;
const int WA = 1;
const int PE = 2;

int main() {
	string dir = getcwd(NULL, 0) + TMP;
	freopen(dir.c_str(), "r", stdin);

	string in, out, exec_out;
	cin >> in >> out >> exec_out;
	ifstream input(in.c_str(), ios::in);
	ifstream output(out.c_str(), ios::in);
	ifstream exec_output(exec_out.c_str(), ios::in);
	bool flag = true;
	int a = 0, b = 0, c = 0, d = 0;
	while (exec_output >> a >> b) {
		output >> c >> d;
		if(a + b != c + d) {
			flag = false;
		}
		// printf("%d %d %d %d\n", a, b, c, d);
	}
	return flag ? AC : WA;
}
