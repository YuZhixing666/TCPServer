# TCPServer
A Lightweight TCP server


### 基于动态线程池的并发模型
	常见的Server会fork一个新的进程去处理客户端的请求，但是进程的创建和销毁开销很大，性能会有影响。采用多线程的模型开销会大大降低。为了更好地管理线程，设计了一个动态的线程池。
	
	所谓“动态”，我们在初始化线程池的时候只规定了该线程池的最大线程数量，而并不是固定地新建若干个线程，等待任务的到来。我们将客户端到达的请求存入到一个“任务队列”中，按照FIFO的思想按顺序地将任务分配给线程。在一开始，只有主线程在执行。当任务队列有任务时，线程池才会新建立线程去处理请求。为了降低线程频繁创建和销毁的开销，处理完请求之后线程不会马上退出并且销毁，而是继续执行下一个任务，如果任务队列中已经没有任务了，那么它将等待一段时间，再销毁，如果这段时间内有新的任务，那么将继续工作。
	
	我们将任务队列存放到线程池的结构中，一方面设计较为简单，另一方面这也符合“可扩展性”，因为随着Server接受的任务种类越来越多，其实是可以建立多个线程池的，异构的任务可以分配给不同的线程池处理，并且每个线程池的大小也可以不同。

### I/O模型
	阻塞式I/O，对于每一个套接字上的系统调用都会阻塞，如果一个系统在一个套接字上阻塞，那么其他套接字也没办法继续使用。但是多线程的好处是，即使这一个线程阻塞，还会有其他线程执行其他的任务。因此在处理客户端请求时，单个线程的socket是阻塞式I/O。
	
	非阻塞I/O可以在read和write等系统调用中，如果数据数据未准备好，不进行线程阻塞而是立刻返回一个错误，先处理其他事情，在回过头来继续调用read看看数据是否准备好，虽然执行速度上有所提高，但是需要不断地检查数据，并且程序设计上更复杂。另一方面，由于我们采用了多线程的并发模型，没有必要将线程套接字设计为非阻塞的，因为这一个线程就是为这一个任务服务的，其他到来的新请求可以由新的线程执行。
	
	I/O多路复用模型是用的比较多的模型，是Web Server的不错选择，如Java NIO就是I/O复用模型。它允许我们不阻塞与单一一个套接字，可以在一个线程中（如主线程）管理多个socket。主线程会不断地轮询多个socket的状态，当socket有读/写/异常事件时，才会真正的调用读写操作。
	主线程中我们采用epoll边缘触发的I/O多路复用来侦听多个套接字。

	epoll相较于select和poll的优势有几点：
	- 在轮询fd的过程中，epoll调用回调函数，把就绪fd放入就绪链表中，并唤醒在epoll_wait中进入睡眠的进程，而select和poll则需要轮询的时候把所有的fd遍历一遍查找是否有就绪的fd，epoll只需要看看就绪链表是否有fd就可以了。
	- select，poll每次调用都要把fd集合从用户态往内核态拷贝一次，而epoll只需要一次。
	- select支持的fd数量默认是1024，而epoll没有限制，支持的是系统FD的上限，一般远远大于2048。

### 定时器的设置(未完善)
	虽然说单个线程可以一直为一个task服务，但是如果客户端一直保持着连接，长期占用某个线程是可能的。因此除了线程池的等待机制之外，还应在主线程维护一个定时器。在主线程的轮询中，需要更新定时器，并且通过查找已过期的线程将其使用pthread_cancel/pthread_kill将其销毁(这里面还有很多问题)。获取最小的定时器的方法有挺多的，如Nginx那样的严谨的Server采用的是红黑树来查找最近过期的线程，比较简单的方法还可以用小顶堆等。
	

### 有待改进的设计
	- 如果频繁进行malloc/calloc/free来分配/释放内存，会导致内存碎片化，不容易让系统直接回收内存。可采取的方案是使用内存池分配内存，这样能够有效地提高内存的使用效率。可以针对每一个线程分配一定大小的额内存池。
	- HTTP协议层，将TCP Server改进成一个Web Server。并且考虑如何将多个HTTP请求复用同一个TCP连接等问题。
	- 数据安全性问题，在传输数据的过程中，需要压缩、加密等。
