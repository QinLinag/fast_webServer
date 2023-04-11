FileUtil作用是封装了fopen打开文件，fwrite向文件写入，fflush刷新缓存到文件中，这些方法

LogFile是对FileUtil进一步的封装，因为对文件操作需要加锁，所以LogFile中有锁，这把锁对
FileUtil中对文件操作的方法进行封装。

LogStream中实现了一个FixedBuffer类，所有向文件写入的日志，现写到FixedBuffer对象中，
LogStreadm中还实现了一个LogStream类，这个类重写了所有类型的<<操作符（目的时方便我们
日志记录，如LOG << "这是日志"），LogStream类中有FixedBuffer对象buffer，所有日志通过
<<方式输入到buffer中保存。

AsycnLogging实现了新线程异步将日志写入文件。AsyncLoggin类中，有FixedBuffer集合先将
日志保存起来。AsyncLogging中有Thread对象，还有一个threadFunc作为thread执行的函数。
start函数中执行了thread的start函数开启pthread_create。


最后Logging。define了一个LOG代表无名对象Logger.stream。需要写日志的时候，就可以直接
LOG << "这是日志";  然后LOG是无名对象，当Logger析构是，调用了output函数，Logger对象
会将其内部类impl中的stream的buffer数据给到output，最后output函数中，调用了asyncLogging
将数据append到asyncLogging中。然后append函数最后调用了cond.notify(),通知
log线程的执行函数void AsyncLogging::threadFunc()有内容需要向文件中写入了，
这是一个生产者和消费者的问题，
