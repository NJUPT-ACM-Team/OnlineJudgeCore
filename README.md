#### NJUPT OnlineJudgeCore #### 

##环境##

Linux操作系统

##编译判题内核##

由于结果以json格式输出，需添加依赖库C++ Jsoncpp

```
sudo apt-get install scons
```

下载jsoncpp-src-0.5.0.tar.gz(http://sourceforge.net/projects/jsoncpp/files/)

```
tar -xvzf jsoncpp-src-0.5.0.tar.gz

cd jsoncpp-src-0.5.0

scons platform=linux-gcc
```

进入jsoncpp-src-0.5.0/libs/Linux-gcc-5.4.0

```
mv libjson_linux-gcc-4.9.1_libmt.a libjson.a

sudo cp -rf jsoncpp-src-0.5.0/include/json /usr/include/

sudo cp jsoncpp-src-0.5.0/libs/Linux-gcc-5.4.0/libjson /usr/local/lib/
```

然后执行make即可生成可运行文件judge

```
sudo make
```

##命令行参数##

-c 源代码相对路径

-t 运行时的时间限制

-m 运行时的内存限制

-d 运行时"沙盒"的相对路径

-s 加上此参数表示需要进行special judge

-C special judge程序的相对路径，加上此参数spj会重新编译

##示例##

```
sudo ./judge -c test/a.cpp -t 1000 -m 65535
```

```
sudo ./jduge -c test/a.cpp -t 1000 -m 65535 -s -C test/SPJ.cpp
```

##运行结果##

结果存放在运行文件下的result.json中，如:

```
{
   "Memory" : 0,
   "PassNum" : 3,
   "Result" : "AC",
   "Time" : 0
}
```

##约定##
1) 输入数据均放在in文件夹下，并以xxx.in命名
2) 输出数据均放在out文件夹下，并以xxx.out命名
3) 每组输入输出数据的文件名必须对应
4) 若需special judge，必须加上-s和-C参数，且spj程序均以SPJ.xxx命名

