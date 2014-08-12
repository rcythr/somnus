Somnus
======

Somnus is a co-operative threading support library  (See: [Fiber](http://en.wikipedia.org/wiki/Fiber_%28computer_science%29)).

Fibers are a powerful concurrency tool that prevents unnecessary blocking and operating system thread context switching. It's not a panacea, but it is a useful tool which, utilized in the proper way, yields massive performance gains.

Refer to the [wiki](https://github.com/rcythr/somnus/wiki) for type and function documentation.

### Channel Examples

The canonical example of any coroutine based model is an example with a channel. The following two examples show the use of a channel for both pointer types and non-pointer types.

[Non pointer type example](examples/test2.cc)

[Pointer type example](examples/test3.cc)

[std::shared_ptr type example](examples/test4.cc)

### Other Examples

Long running tasks prevent other tasks from doing useful work, including initiation of long running operations. To prevent this, long running tasks should carefully place calls to **defer()** which switches to other waiting tasks, if needed. Execution will resume after all other waiting tasks have had an opportunity to run.

[Defer Functionality](examples/test5.cc)
