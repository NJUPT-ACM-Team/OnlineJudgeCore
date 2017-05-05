#ifndef __COMMON__
#define __COMMON__

#include <string>
using namespace std;

namespace LIMIT {

	int JUDGE_TIME 	 	= 15000;
	int SPJ_TIME		= 10000;
	int COMPILE_TIME 	= 10000;
	int STACK_SIZE 		= 8192;

	const int MEM_UNIT 	= 1024;
	const int TIME_UNIT = 1000;
}

namespace RESULT {

	const int NONE 	= 0;
	const int CE 	= 1;
	const int RE 	= 2;
	const int TLE 	= 3;
	const int MLE 	= 4;
	const int OLE 	= 5;
	const int WA 	= 6;
	const int PE 	= 7;
	const int AC 	= 8;
}

namespace LANG {

	const int UNKNOWN = 0;
	const int C = 1;
	const int CPP = 2;
	const int JAVA = 3;
	const int PYTHON = 4;

}

namespace PROBLEM {

	int lang = 0;
	int time_limit = 1000;
	int memory_limit = 65536;
	int output_limit = 1024000;
	int time_usage = 0;
	int memory_usage = 0;
	int time_usage_sum = 0;
	int memory_usage_sum = 0;
	int test_file_num = 0;
	int result = RESULT::NONE;
	bool is_spj = false;
	bool is_recompile = false;
	int spj_lang = 0;
	int pass_case_num = 0;
	string extra_message;

}

namespace FILE_PATH {

	const string TMP = "/tmp";
	string spj_code;
	string source_code;
	string exec;
	string exec_spj;
	string input_dir;
	string output_dir;
	string exec_output_dir;
	string cur_input;
	string cur_output;
	string cur_exec_output;
	string result;
	string runtime_dir;
	string compiler_stderr;

}

namespace EXIT {

	const int OK               = 0;  	//正常退出
	const int UNPRIVILEGED     = 1;  	//缺乏权限退出
	const int UNKNOWN_PARAM    = 3;  	//参数错误退出
	const int UNKNOWN_LANG 	   = 4;
	const int SET_TIMER_ERROR  = 5;  	//judge程序运行超时退出
	const int COMPILE          = 6;  	//编译错误退出
	const int COMPILE_SPJ      = 7;  	//spj编译错误退出
	const int PRE_JUDGE        = 9;  	//预处理错误退出，如文件无法打开，无法fork进程等
	const int PRE_JUDGE_PTRACE = 10;  	//ptrace运行错误退出
	const int PRE_SPJ 		   = 11;  	//预处理spj错误
	const int SET_LIMIT        = 15;  	//设置时间限制错误退出
	const int SET_SECURITY     = 17;  	//设置安全限制错误退出
	const int JUDGE            = 21;  	//程序调用错误退出
	const int COMPILE_TIMEOUT  = 36;  	//超时退出
	const int UNKNOWN          = 127; 	//不详

}

#endif