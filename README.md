# YTSvrLib [![TravisCI Status](https://travis-ci.org/sherry0319/YTSvrLib.svg?branch=master)](https://travis-ci.org/sherry0319/YTSvrLib)  [![AppVeyor Status](https://ci.appveyor.com/api/projects/status/github/sherry0319/ytsvrlib?branch=master&svg=true)](https://ci.appveyor.com/project/sherry0319/ytsvrlib)
一个简单但完善的网络游戏服务器库.轻松获得网络,数据库,线程管理以及常用游戏算法.
A simple game server library for linux/windows. Easy access to network(TCP and websocket), database, thread management, common game server features. Quickly develop your game server.

# 主要特性

## 快速开始你的工作
只需要在Sample中根据你的需要做出简单修改.就可以快速连通各环节开始编写你的游戏服务器!

## 在Windows上用Visual Studio写游戏并运行调试.同时允许在Linux上编译并运行.
可以愉快的使用Visual Studio强大的功能加速你的开发.同时又不影响在Linux上编译运行.

## 简单易用,逻辑清晰的接口封装
尽量做到一个调用完成一个操作

## 完善的功能
异步日志.异常捕获.对象池.各种锁都实现了抹平平台差异的封装.

# 有任何问题欢迎提Issue或联系QQ:81045135

# Install Windows
1.Install Visual Studio 2019.

2.Start with YTSvrLib.sln

3.Build solution, get the YTSvrLib.lib

4.Use the .lib in your project.

# Install Linux
1.Install dependent libraries : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc (The lib mysqlpp must use the version in the third_party/)

2.cd src && ./configure && make install

3.You can get libytsvrlib.so.

4.Use the libytsvrlib.so in your project.See Sample for more usage example.
# How to use
See the project "Sample".


# 在Windows下使用
1.安装 Visual Studio 2019 

2.使用YTSvrLib.sln打开项目

3.编译项目,你可以得到YTSvrLib.lib

4.在你的工程中链接lib

# 在Linux下使用
1.先安装以下依赖库 : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc (mysqlpp 需要用项目中third_party里带的版本,对原本的库做了点小修改)

2.cd src && ./configure && make install (或者你可以直接用./makeall.sh来获得DEBUG版本或者./makeall_release.sh来获得release版本)

3.你将会得到libytsvrlib.so

4.将它用在你的项目中.

# 如何使用
请参考"Sample"项目了解使用方法.

//////////////////////////////////////////////////////////////////////////////////////////////////////

主要特性:
1.创建服务器或客户端TCP连接.

2.创建一个基于websocket的服务器监听并处理数据.

3.创建一个连接到MySQL或MariaDB的数据库连接.并且同步或异步的接收查询返回.基于连接池的多个连接.

4.创建连接到redis的同步或异步连接.

5.完善的基础功能.日志系统.时间处理.文件操作.
