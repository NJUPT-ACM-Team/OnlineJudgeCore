#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/ptrace.h>

#include "common.h"
#include "logger.h"
#include "exec.h"

static string get_filename(string file) {
	string filename = "";
	for (int i = 0; i < file.size(); i ++) {
		if (file[i] == '.') {
			break;
		}
		filename += file[i];
	}
	return filename;
}

static int get_language(const string &filename) {
	string suffix = "";
    for (int i = filename.size() - 1; i >= 0 && filename[i] != '.'; i --) {
        suffix += filename[i];
    }
    reverse(suffix.begin(), suffix.end());
    if (suffix == "c") {
    	return LANG::C;
    } else if (suffix == "cpp") {
    	return LANG::CPP;
    } else if (suffix == "java") {
    	return LANG::JAVA;
    } else if (suffix == "py") {
    	return LANG::PYTHON;
    }
    return LANG::UNKNOWN;
}

static int set_timer(int which, int milliseconds) {
	struct itimerval delay;
	delay.it_value.tv_sec 		= milliseconds / 1000;
	delay.it_value.tv_usec 		= 0;
	delay.it_interval.tv_sec  	= 0;
    delay.it_interval.tv_usec 	= 0;
    return setitimer(which, &delay, NULL);
}

static void timeout_callback(int signo) {
    if (signo == SIGALRM) {
    	LOG_WARNING("Compile timeout");
    	PROBLEM::result = RESULT::CE;
        PROBLEM::extra_message = "Compile Out of Time Limit";
        exit(EXIT::COMPILE_TIMEOUT);
    }
}

static void parse_parameters_and_init(int argc, char *argv[]) {
	extern char *optarg;
	int opt;
	while ((opt = getopt(argc, argv, "c:t:m:d:C:s")) != -1) {
		switch (opt) {
			case 'c': FILE_PATH::source_code 	= optarg; 		break;
			case 't': PROBLEM::time_limit 		= atoi(optarg); break;
			case 'm': PROBLEM::memory_limit 	= atoi(optarg);	break;
			case 'd': FILE_PATH::runtime_dir 	= optarg; 		break;
			case 's': PROBLEM::is_spj			= true;			break;
			case 'C': FILE_PATH::spj_code		= optarg; 		
					  PROBLEM::is_recompile 	= true;			break;
			default:
				LOG_WARNING("Unknown parameter: -%c %s", opt, optarg);
                exit(EXIT::UNKNOWN_PARAM);
		}
	}
 	PROBLEM::lang = get_language(FILE_PATH::source_code);
 	if (PROBLEM::lang == LANG::UNKNOWN) {
 		LOG_WARNING("Unknown language: %d", PROBLEM::lang);
        exit(EXIT::UNKNOWN_LANG);
 	}

 	if (PROBLEM::is_spj) {
 		PROBLEM::spj_lang = get_language(FILE_PATH::spj_code);
 		FILE_PATH::exec_spj = FILE_PATH::runtime_dir + "/SPJ";
 		if (PROBLEM::spj_lang == LANG::UNKNOWN) {
 			LOG_WARNING("Unknown language: %d", PROBLEM::spj_lang);
        	exit(EXIT::UNKNOWN_LANG);
 		}
 	}

 	FILE_PATH::exec 			= FILE_PATH::runtime_dir + "/a.out";
    FILE_PATH::input_dir 		= FILE_PATH::runtime_dir + "/in";
    FILE_PATH::output_dir 		= FILE_PATH::runtime_dir + "/out";
    FILE_PATH::exec_output_dir 	= FILE_PATH::runtime_dir + "/exec_out";
    FILE_PATH::result 			= FILE_PATH::runtime_dir + "/result.txt";
    FILE_PATH::compiler_stderr 	= FILE_PATH::runtime_dir + "/stderr_file_compiler.txt";

    LIMIT::JUDGE_TIME += PROBLEM::time_limit;
}

static void get_CE_message() {
    FILE *ce_msg = fopen(FILE_PATH::compiler_stderr.c_str(), "r");
    string message = "";
    char tmp[1024];

    while (fgets(tmp, sizeof(tmp), ce_msg)) {
        message += tmp;
    }
    PROBLEM::extra_message = message;
}

static void compile_source_code() {
	pid_t compiler = fork();
	int status = 0;
	if (compiler < 0) {
		LOG_WARNING("Fail to fork compiler");
		exit(EXIT::COMPILE);
	} else if (compiler == 0) {
		log_add_info("compiler");

		stderr = freopen(FILE_PATH::compiler_stderr.c_str(), "w", stderr);
        if (stderr == NULL) {
            LOG_WARNING("Fail to freopen in compiler: stdout(%p) stderr(%p)", stdout, stderr);
            exit(EXIT::COMPILE);
        }

		if (EXIT_SUCCESS != set_timer(ITIMER_REAL, LIMIT::JUDGE_TIME)) {
        	LOG_WARNING("Fail to set alarm, %d: %s", errno, strerror(errno));
        	exit(EXIT::SET_TIMER_ERROR);
    	}
    	signal(SIGALRM, timeout_callback);

    	exec_compile(PROBLEM::lang);
	} else {
		if (waitpid(compiler, &status, WUNTRACED) == -1) {
            LOG_WARNING("Waitpid error");
            exit(EXIT::COMPILE);
        }
		LOG_TRACE("Compile finished");

		if (WIFEXITED(status)) {
            if (EXIT_SUCCESS == WEXITSTATUS(status)) {
                LOG_TRACE("Compile succeeded");
            } else if (EXIT_FAILURE == WEXITSTATUS(status)){
                LOG_TRACE("Compile error");
                PROBLEM::result = RESULT::CE;
                get_CE_message();
                exit(EXIT::OK);
            } else {
                LOG_WARNING("Unknown error occur when compiling the source code. Exit status %d", 
                			WEXITSTATUS(status));
                exit(EXIT::COMPILE);
            }
        } else {
            if (WIFSIGNALED(status)){
            	int signo = WTERMSIG(status);
                if (SIGALRM == signo) {
                    LOG_WARNING("Compile timeout");
                    PROBLEM::result = RESULT::CE;
                    PROBLEM::extra_message = "Compile Out of Time Limit";
                    exit(EXIT::OK);
                } else {
                    LOG_WARNING("Compiler has killed by signal %d : %s", signo, strsignal(signo));
                }
            } else if (WIFSTOPPED(status)){
                LOG_WARNING("Compiler has stopped by signal");
            } else {
                LOG_WARNING("Something wrong");
            }
            exit(EXIT::COMPILE);
        }
	}
}

static void compile_spj_code() {
	pid_t spj_compiler = fork();
	int status = 0;
	if (spj_compiler < 0) {
		LOG_WARNING("Fail to fork spj_compiler");
		exit(EXIT::COMPILE_SPJ);
	} else if (spj_compiler == 0) {
		log_add_info("spj_compiler");

		if (EXIT_SUCCESS != set_timer(ITIMER_REAL, LIMIT::JUDGE_TIME)) {
        	LOG_WARNING("Fail to set alarm, %d: %s", errno, strerror(errno));
        	exit(EXIT::SET_TIMER_ERROR);
    	}
    	signal(SIGALRM, timeout_callback);

    	exec_compile_spj(PROBLEM::spj_lang);
	} else {
		if (waitpid(spj_compiler, &status, WUNTRACED) == -1) {
            LOG_WARNING("Waitpid error");
            exit(EXIT::COMPILE_SPJ);
        }
		LOG_TRACE("Compile spj finished");

		if (WIFEXITED(status)) {
            if (EXIT_SUCCESS == WEXITSTATUS(status)) {
                LOG_TRACE("Compile spj succeeded");
            } else if (EXIT_FAILURE == WEXITSTATUS(status)){
                LOG_TRACE("Compile spj error");
                exit(EXIT::OK);
            } else {
                LOG_WARNING("Unknown error occur when compiling the spj code. Exit status %d", 
                			WEXITSTATUS(status));
                exit(EXIT::COMPILE_SPJ);
            }
        } else {
            if (WIFSIGNALED(status)){
            	int signo = WTERMSIG(status);
                if (SIGALRM == signo) {
                    LOG_WARNING("spj_Compile timeout");
                    exit(EXIT::OK);
                } else {
                    LOG_WARNING("spj_Compiler has killed by signal %d : %s", signo, strsignal(signo));
                }
            } else if (WIFSTOPPED(status)){
                LOG_WARNING("spj_Compiler has stopped by signal");
            } else {
                LOG_WARNING("spj_Something wrong");
            }
            exit(EXIT::COMPILE_SPJ);
        }
	}
}

static void set_io_redirect() {
	LOG_TRACE("Start to redirect the IO.");
    stdin = freopen((FILE_PATH::input_dir + "/" + FILE_PATH::cur_input).c_str(), "r", stdin);
    stdout = freopen((FILE_PATH::exec_output_dir + "/" + FILE_PATH::cur_exec_output).c_str(), "w", stdout);
    if (stdin == NULL || stdout == NULL) {
        LOG_WARNING("It occur a error when freopen: stdin(%p) stdout(%p)", stdin, stdout);
        exit(EXIT::PRE_JUDGE);
    }
    LOG_TRACE("Redirect io done");
}

static void set_security_control() {
    struct passwd *nobody = getpwnam("nobody");
    if (nobody == NULL){
        LOG_WARNING("Cannot find nobody. %d: %s", errno, strerror(errno));
        exit(EXIT::SET_SECURITY);
    }

    if (EXIT_SUCCESS != chdir(FILE_PATH::runtime_dir.c_str())) {
        LOG_WARNING("Chdir(%s) failed, %d: %s", 
            FILE_PATH::runtime_dir.c_str(), errno, strerror(errno));
        exit(EXIT::SET_SECURITY);
    }

    char cwd[1024];
    if (getcwd(cwd, 1024) == NULL) {
        LOG_WARNING("Getcwd failed. %d: %s", errno, strerror(errno));
        exit(EXIT::SET_SECURITY);
    }

    if (PROBLEM::lang != LANG::JAVA) {
        if (EXIT_SUCCESS != chroot(cwd)) {
            LOG_WARNING("chroot(%s) failed. %d: %s", cwd, errno, strerror(errno));
            exit(EXIT::SET_SECURITY);
        }
        if (EXIT_SUCCESS != setuid(nobody -> pw_uid)) {
            LOG_WARNING("setuid(%d) failed. %d: %s", 
                nobody -> pw_uid, errno, strerror(errno));
            exit(EXIT::SET_SECURITY);
        }
    }
}

static void set_runtime_limit() {
    rlimit lim;

    lim.rlim_max = (PROBLEM::time_limit + 999) / 1000 + 1;
    lim.rlim_cur = lim.rlim_max;
    if (setrlimit(RLIMIT_CPU, &lim) < 0) {
        LOG_WARNING("Fail to setrlimit for RLIMIT_CPU");
        exit(EXIT::SET_LIMIT);
    }

    lim.rlim_max = LIMIT::STACK_SIZE * LIMIT::MEM_UNIT;
    lim.rlim_cur = lim.rlim_max;
	if (setrlimit(RLIMIT_STACK, &lim) < 0) {
        LOG_WARNING("Fail to setrlimit for RLIMIT_STACK");
        exit(EXIT::SET_LIMIT);
    }

    log_close();

    lim.rlim_max = PROBLEM::output_limit * LIMIT::MEM_UNIT;
    lim.rlim_cur = lim.rlim_max;
    if (setrlimit(RLIMIT_FSIZE, &lim) < 0) {
    	LOG_WARNING("Fail to setrlimit for RLIMIT_FSIZE");
        exit(EXIT::SET_LIMIT);
    }

    if (EXIT_SUCCESS != set_timer(ITIMER_REAL, PROBLEM::time_limit)) {
    	LOG_WARNING("Fail to set alarm, %d: %s", errno, strerror(errno));
        exit(EXIT::SET_LIMIT);
    }
}

static void execute_source_code() {
	struct rusage usage;
    pid_t executor = fork();
    if (executor < 0) {
    	LOG_WARNING("Fail to fork executor");
		exit(EXIT::PRE_JUDGE);
    } else if (executor == 0) {
    	LOG_TRACE("Start executing");

    	set_io_redirect();

    	set_security_control();

    	set_runtime_limit();

    	exec_run(PROBLEM::lang);
    } else {
    	int status = 0;
    	int syscall_id = 0;
    	struct user_regs_struct regs;

    	while (true) {
    		if (wait4(executor, &status, 0, &usage) < 0) {
    			LOG_WARNING("Wait4 failed.");
    			exit(EXIT::JUDGE);
    		}

            PROBLEM::memory_usage = max((long int)PROBLEM::memory_usage,
                    	usage.ru_minflt * (getpagesize() / LIMIT::MEM_UNIT));
            // MLE
            if (PROBLEM::memory_usage > PROBLEM::memory_limit) {
            	LOG_TRACE("Memory Limit Exceeded");
                PROBLEM::time_usage = 0;
                PROBLEM::memory_usage = PROBLEM::memory_limit;
                PROBLEM::result = RESULT::MLE;
        	}

    		if (WIFEXITED(status)) {
                if (EXIT_SUCCESS == WEXITSTATUS(status)) {
                    LOG_TRACE("OK. All is good");
                    PROBLEM::time_usage += (usage.ru_utime.tv_sec * LIMIT::TIME_UNIT +
                                usage.ru_utime.tv_usec / LIMIT::TIME_UNIT);
                    PROBLEM::time_usage += (usage.ru_stime.tv_sec * LIMIT::TIME_UNIT +
                                usage.ru_stime.tv_usec / LIMIT::TIME_UNIT);
                } else {
                    LOG_WARNING("Some errors occured");
                    PROBLEM::result = RESULT::RE;
                }
                break;
            } else {
            	int signo = 0;
            	if (WIFSIGNALED(status)) {
            		signo = WTERMSIG(status);
                    LOG_WARNING("Judger has killed by signal %d : %s", signo, strsignal(signo));
            	} else if (WIFSTOPPED(status)) {
            		signo = WSTOPSIG(status);
            		if (SIGTRAP != signo) {
            			LOG_WARNING("Judger has stopped by signal %d : %s", signo, strsignal(signo));
            		}
            	}

            	switch (signo) {

            		// TLE
            		case SIGALRM:
                    case SIGVTALRM:
                    case SIGPROF:
                    case SIGXCPU:
                    case SIGKILL:
                   		LOG_TRACE("Time Limit Exeeded");
                        PROBLEM::time_usage = PROBLEM::time_limit;
                        PROBLEM::memory_usage = 0;
                        PROBLEM::result = RESULT::TLE;
                        break;

                    // OLE
                    case SIGXFSZ:
                        LOG_TRACE("Output Limit Exceeded");
                        PROBLEM::time_usage = 0;
                        PROBLEM::memory_usage = 0;
                        PROBLEM::result = RESULT::OLE;
                        break;

                    default: 
                    	LOG_TRACE("Runtime Error");
                        PROBLEM::time_usage = 0;
                        PROBLEM::memory_usage = 0;
                        PROBLEM::result = RESULT::RE;
                        break;
            	}

            	ptrace(PTRACE_KILL, executor, NULL, NULL);
                exit(EXIT::OK);
            }
    	}
    }
}

static void judge_output_result() {
	string AC_command = "diff -q --strip-trailing-cr ";
	string PE_command = "diff -q -B -b -w --strip-trailing-cr ";
	string cmp_files = FILE_PATH::output_dir + "/" + FILE_PATH::cur_output + " " + 
						FILE_PATH::exec_output_dir + "/" + FILE_PATH::cur_exec_output;
	AC_command += cmp_files;
 	if (!system(AC_command.c_str())) {
 		PROBLEM::result = RESULT::AC;
 		return;
 	}

 	PE_command += cmp_files;
 	if (!system(PE_command.c_str())) {
 		PROBLEM::result = RESULT::PE;
 		return;
 	}

 	PROBLEM::result = RESULT::WA;
}

static void judge_output_result_spj() {
	pid_t spj = fork();
	int status = 0;
	if (spj == -1) {
		LOG_WARNING("Fail to fork spj");
		exit(EXIT::PRE_SPJ);
	} else if (spj == 0) {
		LOG_TRACE("Start special judge");
		
		//set_io_redirect();

        if (EXIT_SUCCESS != set_timer(ITIMER_REAL, LIMIT::SPJ_TIME)) {
            LOG_WARNING("Fail to set alarm, %d: %s", errno, strerror(errno));
            exit(EXIT::SET_TIMER_ERROR);
        }

        exec_spj(PROBLEM::spj_lang);
	} else {

		if (wait4(spj, &status, 0, NULL) < 0) {
            LOG_WARNING("Wait4 failed.");
    		exit(EXIT::JUDGE);
        }

        if (WIFEXITED(status)) {
            int spj_exit = WEXITSTATUS(status);
            if (spj_exit >= 0 && spj_exit < 4) {
                LOG_TRACE("SpecialJudge success and quit");
                switch (spj_exit) {
                	case 0:
                    	PROBLEM::result = RESULT::AC;
                    	break;
                	case 1:
                    	PROBLEM::result = RESULT::WA;
                    	break;
                	case 2:
                    	PROBLEM::result = RESULT::PE;
                    	break;
                }
                ptrace(PTRACE_KILL, spj, NULL, NULL);
                return;
            } else {
                LOG_WARNING("SpecialJudge quit abnormally %d", WEXITSTATUS(status));
            }
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
            LOG_WARNING("SpecialJudge time exceed limit");
        } else {
            LOG_WARNING("SpecialJudge quit abnormally");
        }
	}
}

static void export_result() {
	FILE* result_file = fopen(FILE_PATH::result.c_str(), "w");
	string result = "";
    switch (PROBLEM::result) {
        case RESULT::CE : result = "CE";	break;
        case RESULT::RE : result = "RE"; 	break;
        case RESULT::TLE: result = "TLE"; 	break;
        case RESULT::MLE: result = "MLE"; 	break;
        case RESULT::OLE: result = "OLE";	break;
        case RESULT::WA : result = "WA"; 	break;
        case RESULT::PE : result = "PE"; 	break;
        case RESULT::AC : result = "AC"; 	break;
        default: result = "SE"; 			break;
    }
    string extraMessage = PROBLEM::extra_message;
    fprintf(result_file, "Result = %s\n", result.c_str());
    int time_usage = 0;
    int memory_usage = 0;
    if (result == "AC" || result == "PE" || result == "WA") {
    	time_usage = PROBLEM::time_usage_sum / PROBLEM::test_file_num;
    	memory_usage = PROBLEM::memory_usage_sum / PROBLEM::test_file_num;
    } else {
    	time_usage = PROBLEM::time_usage;
    	memory_usage = PROBLEM::memory_usage;
    }
    fprintf(result_file, "PassNum = %d\n", PROBLEM::pass_case_num);
    fprintf(result_file, "Time = %d\n", time_usage);
    fprintf(result_file, "Memory = %d\n", memory_usage);
    if (extraMessage.length() != 0) {
        fprintf(result_file, "CE = %s\n", extraMessage.c_str());
    }

    LOG_TRACE("The final result is %s %d %d %s",
            result.c_str(), time_usage,
            memory_usage, PROBLEM::extra_message.c_str());
    //string rm_exec_output_files = "rm " + FILE_PATH::exec_output_dir + "/*";
    //system(rm_exec_output_files.c_str());
}

static vector<string> get_files(string cur_dir) {

	DIR *dir;
	vector<string> files;  
    struct dirent *ptr;    
   
    if ((dir = opendir(cur_dir.c_str())) == NULL)  {  
        LOG_WARNING("Opendir failed. %d: %s", errno, strerror(errno));
        exit(EXIT::PRE_JUDGE); 
    }  
   
    while ((ptr = readdir(dir)) != NULL)  {  
        if (ptr -> d_type == DT_REG) {
            files.push_back(ptr -> d_name);  
        } 
    }  
    closedir(dir);    
    sort(files.begin(), files.end());  
    return files; 
}

int main(int argc, char *argv[]) {

	log_open("./judge_log.txt");

	atexit(export_result);

	if (geteuid() != 0) {
		LOG_FATAL("You must run this program as root");
        exit(EXIT::UNPRIVILEGED);
	}

	parse_parameters_and_init(argc, argv);

	compile_source_code();

	if (PROBLEM::is_recompile) {
		compile_spj_code();
	}

	vector<string> input_files = get_files(FILE_PATH::input_dir);

	for (int i = 0; i < input_files.size(); i ++) {
		FILE_PATH::cur_input = input_files[i];
		FILE_PATH::cur_exec_output = get_filename(input_files[i]) + ".out";
		execute_source_code();
		if (PROBLEM::result == RESULT::NONE) {
			PROBLEM::time_usage_sum += PROBLEM::time_usage;
			PROBLEM::memory_usage_sum += PROBLEM::memory_usage;
			PROBLEM::time_usage = 0;
			PROBLEM::memory_usage = 0;
		}	
	}

	vector<string> output_files = get_files(FILE_PATH::output_dir);
	vector<string> exec_output_files = get_files(FILE_PATH::exec_output_dir);

	if (output_files.size() != exec_output_files.size()) {
		LOG_WARNING("Executive output files do not match origin output files");
		exit(EXIT::JUDGE);
	}

	if (output_files.size() == 0 || exec_output_files.size() == 0) {
		LOG_WARNING("Executive output files or origin output files miss");
		exit(EXIT::JUDGE);
	}	

	PROBLEM::test_file_num = output_files.size();

	for (int i = 0; i < output_files.size(); i ++) {
		FILE_PATH::cur_input = input_files[i];
		FILE_PATH::cur_output = output_files[i];
		FILE_PATH::cur_exec_output = exec_output_files[i];
		if (PROBLEM::is_spj) {
			string tmp_path = FILE_PATH::runtime_dir + FILE_PATH::TMP;
			ofstream tmp(tmp_path.c_str(), ios::out | ios::trunc);
			tmp << FILE_PATH::input_dir + "/" + FILE_PATH::cur_input << endl;
			tmp << FILE_PATH::output_dir + "/" + FILE_PATH::cur_output << endl;
			tmp << FILE_PATH::exec_output_dir + "/" + FILE_PATH::cur_exec_output << endl;
		}
		if (PROBLEM::is_spj) {
			judge_output_result_spj();
		} else {
			judge_output_result();
		}
		if (PROBLEM::result != RESULT::AC) {
			break;
		}
		PROBLEM::pass_case_num ++;
	}
}