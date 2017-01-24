一个简单的 Lua 性能分析工具
=====

* profiler.start([co,] count [, interval])

针对当前线程，或指定一个线程 (co) 开启性能分析。同一个线程 (coroutine) 只可以同时开始一次。count 指最多记录多少条探测记录，interval 指每隔多少条字节码探测一次。

* profiler.info([co])

返回 info 表和 count ，info 表里有每个探测点所在的源文件构成的子表，子表中根据每个函数定义开始行为 key 统计被探测到的次数。count 指从开始到当下，总共被探测到的次数。注意，这个返回次数有可能大于 start 时传入的次数，这种情况下，info 表中的统计次数是按 start 传入的次数（最后发生的次数）计算的。

* profiler.stop([co])

停止分析统计。info 必须先于 stop 调用。由于 info 函数本身是 C 函数，所以调用 info 并不会影响统计结果。




