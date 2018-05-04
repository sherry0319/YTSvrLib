# YTSvrLib [![TravisCI Status](https://travis-ci.org/sherry0319/YTSvrLib.svg?branch=master)](https://travis-ci.org/sherry0319/YTSvrLib)  [![AppVeyor Status](https://ci.appveyor.com/api/projects/status/github/sherry0319/ytsvrlib?branch=master&svg=true)](https://ci.appveyor.com/project/sherry0319/ytsvrlib)
A simple game server library for linux/windows. Easy access to network(TCP and websocket), database, thread management, common game server features. Quickly develop your game server.
一个简单的网络游戏服务器库.轻松获得网络,数据库,线程管理以及常用游戏算法.

# Install Windows
1.Install Visual Studio 2013.

2.Start with YTSvrLib.sln

3.Build solution, get the YTSvrLib.lib

4.Use the .lib in your project.

# Install Linux
1.Install dependent libraries : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc libwebsockets(The lib mysqlpp must use the version in the third_party/)

2.cd src && ./configure && make install

3.You can get libytsvrlib.so.

4.Use the libytsvrlib.so in your project.See Sample for more usage example.
# How to use
See the project "Sample".


# 在Windows下使用
1.安装 Visual Studio 2013 

2.使用YTSvrLib.sln打开项目

3.编译项目,你可以得到YTSvrLib.lib

4.在你的工程中链接lib

# 在Linux下使用
1.先安装以下依赖库 : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc libwebsockets(mysqlpp 需要用项目中third_party里带的版本,对原本的库做了点小修改)

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
