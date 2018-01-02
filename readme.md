
Based on [g3log](https://github.com/KjellKod/g3log) and [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp).

g3log is cool for its asynchronous and "crash safe". Sometimes we want to log some data in order to analysis by python/matlab later, one method is to write the data to some text files, but the simple writing usually is not asynchronous and may lead to many files. Another method is to write the data to a log file, but you may have to write some scripts to extract what you really need from the whole file.

Here is another method, we can write these data to a database, then the data is structured instead of just text information.

### Usage

This demo is easy to understand, you can define items which you want to insert to database in `my_sqlite3_table.h`, then call macro `MyDatabaseLog` to write data just like text logger. For example, in `main.cpp`
```
  MyDatabaseLog(DbTableIdx_Tri3dCorres) << camTMapIdx_1 << ", " << camTMapIdx_2 << ", " << tri3d_loc_idx;
```

You can also use `LOG(INFO)` to write normal text log items.

You can use [DB Browser for SQLite](http://sqlitebrowser.org/) to visualize the data:

![db_browser](https://github.com/extr15/my_database_logger/blob/master/images/db_browser.png)

### matlab exmple

`load_sqlitedb.m` is an example to read the database file and plot some results.

For example:

![matlab](https://github.com/extr15/my_database_logger/blob/master/images/matlab.png)

### 中文说明
调用了g3log和SQLiteCpp，类似写log的写法把数据写到sqlite数据库中，保留下来的数据可以用其他脚本语言去分析，便于调试和改进程序性能。

g3log是异步的，而且会在程序crash之前把log都保留下来，这方面用在调试上比glog更好用。写到数据库的好处是写成了结构化的数据，而不是像log中每一行都是文本数据，还需要另外的处理才能单独提取出所需要的数据。
