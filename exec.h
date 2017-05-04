#ifndef __LANG__
#define __LANG__

#include <string>
#include <unistd.h>
#include "common.h"
#include "logger.h"

void exec_compile(int lang) {
	switch (lang) {
		case LANG::C:
			LOG_TRACE("Start compile: gcc -o %s %s -static -w -lm -std=c99 -O2 -DONLINE_JUDGE",
            		FILE_PATH::exec.c_str(), FILE_PATH::source_code.c_str());
            execlp("gcc", "gcc", "-o", FILE_PATH::exec.c_str(), FILE_PATH::source_code.c_str(),
            		"-static", "-w", "-lm", "-std=c99", "-O2", "-DONLINE_JUDGE", NULL);
            break;
		case LANG::CPP:
			LOG_TRACE("Start compile: g++ -o %s %s -static -w -lm -O2 -DONLINE_JUDGE",
                    FILE_PATH::exec.c_str(), FILE_PATH::source_code.c_str());
            execlp("g++", "g++", "-o", FILE_PATH::exec.c_str(), FILE_PATH::source_code.c_str(),
                    "-static", "-w", "-lm", "-std=c++11", "-DONLINE_JUDGE", NULL);
            break;
		case LANG::JAVA:
			LOG_TRACE("Start compile: javac %s -d %s", FILE_PATH::source_code.c_str(), 
					FILE_PATH::runtime_dir.c_str());
            execlp("javac", "javac", FILE_PATH::source_code.c_str(), "-d", 
            		FILE_PATH::runtime_dir.c_str(), NULL);
            break;
		case LANG::PYTHON:
			break;
		default:
			LOG_WARNING("exec compile error");
        	exit(EXIT::COMPILE);
	}
}

void exec_run(int lang) {
	switch (lang) {
		case LANG::C:
		case LANG::CPP:
			execl("./a.out", "a.out", NULL);
            break;
		case LANG::JAVA:
			execlp("java", "java", "Main", NULL);
            break;
		case LANG::PYTHON:
			execlp("python", "python", FILE_PATH::source_code.c_str(), NULL);
			break;
		default:
			LOG_WARNING("exec run error");
        	exit(EXIT::PRE_JUDGE_PTRACE);
	}
}

void exec_compile_spj(int lang) {
	switch (lang) {
		case LANG::C:
			LOG_TRACE("Start compile spj: gcc %s -o %s",
            		FILE_PATH::spj_code.c_str(), FILE_PATH::exec.c_str());
            execlp("gcc", "gcc", FILE_PATH::spj_code.c_str(), "-o", 
            		FILE_PATH::exec_spj.c_str(), NULL);
            break;
		case LANG::CPP:
			LOG_TRACE("Start compile spj: g++ %s -o %s",
                    FILE_PATH::spj_code.c_str(), FILE_PATH::exec.c_str()); 
			//cout << FILE_PATH::spj_code << endl;
			//cout << FILE_PATH::exec_spj << endl;
            execlp("g++", "g++", FILE_PATH::spj_code.c_str(), "-o", 
            		FILE_PATH::exec_spj.c_str(), NULL);
            break;
		case LANG::JAVA:
			LOG_TRACE("Start compile spj: javac %s -d %s", FILE_PATH::source_code.c_str(), 
					FILE_PATH::runtime_dir.c_str());
            execlp("javac", "javac", FILE_PATH::spj_code.c_str(), "-d", 
            		FILE_PATH::runtime_dir.c_str(), NULL);
            break;
		case LANG::PYTHON:
			break;
		default:
			LOG_WARNING("exec compile spj error");
        	exit(EXIT::COMPILE_SPJ);
	}
}

void exec_spj(int lang) {
	switch (lang) {
		case LANG::C:
		case LANG::CPP:
			execl((FILE_PATH::runtime_dir + "/SPJ").c_str(), "SPJ", NULL);
            break;
		case LANG::JAVA:
			execlp((FILE_PATH::runtime_dir + "/java").c_str(), "java", "SPJ", NULL);
            break;
		case LANG::PYTHON:
			execlp((FILE_PATH::runtime_dir + "/python").c_str(), "python", "SPJ", NULL);
			break;
		default:
			LOG_WARNING("exec spj error");
        	exit(EXIT::PRE_JUDGE_PTRACE);
	}
}

#endif