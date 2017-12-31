#A logger writing data to sqlite3 database

Based on [g3log](https://github.com/KjellKod/g3log) and [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp).
g3log is cool for its asynchronous and "crash safe". Sometimes we want to log some data in order to analysis by python/matlab later, one method is to write the data to some text files, but the simple writing usually is not asynchronous and may lead to many files. Another method is to write the data to a log file, but you may have to write some scripts to extract what you really need from the whole file.
Here is another method, we can write these data to a database, then the data is structured instead of just text information.

#Usage

This demo is easy to understand, you can define items which you want to insert to database in `my_sqlite3_table.h`, then call macro `MyDatabaseLog` to write data just like text logger. For example, in `main.cpp`
```
  MyDatabaseLog(DbTableIdx_Tri3dCorres) << camTMapIdx_1 << ", " << camTMapIdx_2 << ", " << tri3d_loc_idx;
```

#matlab exmple

`load_sqlitedb.m` is an example to read the database file and plot some results.

