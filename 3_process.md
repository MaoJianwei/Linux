# 3  进程

本节记录进程控制原语，在此之前先了解进程的环境。本章中将学习：当程序执行时，其main函数是如何被调用的；命令行参数是如何传递给新程序的；典型的储存空间布局是什么样式；如何分配另外的储存空间；进程如何使用环境变量；进程的各种终止方式等。

## 3.1 main函数

C程序总是从main函数开始执行。main函数的原型是：
```c
    int main(int argc, char *argv[]);
```
其中，`argc`是参数数目，`argv`是指向参数各个指针构成的数组。当内核执行C程序时，在调用`main`前先调用一个特殊的启动例程。可执行程序文件将此启动例程指定为**程序的起始地址**——这是由连接编辑器设置的，而连接编辑器是由C编译器调用。启动例程从内核获取命令行参数和环境变量值，然后为按上述方法调用main函数做好安排。

## 3.2 进程终止

有8种方式使进程终止（termination），其中5中是正常终止，他们是：

1. 从main返回；
2. 调用exit；
3. 调用_exit或_Exit;
4. 最后一个线程从起启动例程返回；
5. 从最后一个线程调用pthread_exit;
异常终止有3种方式，他们是：
6. 调用abort；
7. 接收到一个信号；
8. 最后一个线程对取消请求做出相应。

### 3.2.1 退出函数
```c
    #include <stdlib.h>

    void eixt(int status);
    void _Exit(int status);

    #include <unistd.h>

    void _exit(int status);
```

### 3.2.2 函数atexit

按照ISO C的规定，一个进程最多可以登记至多32个函数，这些函数由exit自动调用。我们称这些函数为终止处理程序（exit handler），并调用函数来登记这些函数。

```c
    #include <stdlib.h>

    int atexit(void (*func)(void));
```

其中，atexit的参数是一个函数地址，当调用此函数时无需向他传递任何参数，也不期望它返回一个值。exit调用这些函数的顺序与它们登记时的顺序相反。同一函数若登记多次，也会被调用多次。

<center>![一个C程序是如何启动和终止的](./figures/1-file-io/ioctl.png)</center>

注意，内核是程序执行的唯一方法是调用一个exec函数。进程自愿终止的唯一方法是显示或隐式调用_exit或_Exit。进程也可以非自愿地由一个信号使其终止。

## 3.3 命令行参数

当执行一个程序时，调用exec的进程可将命令行参数传递给该新程序。这是UNIX shell的一部分新常规操作。

## 3.4 环境表

每个程序都接收一张环境表。与参数表一样，环境表也是一个字符指针数组，其中每个指针包含一个以null结束的C字符串的地址。全局变量environ则包含了该指针数组的地址：

    extern char **environ;

例如，如果该环境包含5个字符串，每个字符串结尾处都有显示地有一个null字节。我们称environ为环境指针，指针数组为环境表，其中个指针指向的字符串为环境字符串。可用getenv和putenv来访问特定的环境变量。

## 3.5 C程序的储存空间布局

## 3.6 共享库

共享库使得可执行文件中不再需要包含共用的库函数，而只需要在所有进程都可引用的储存区中保存这种库例程的一个副本。程序第一次执行或者第一次调用某个库函数时，用动态链接方法将程序与共享库函数链接。这减少了每个可执行文件的长度，但增加了一些运行时间开销。这种时间开销发生在该程序第一次被执行的时，或者每个共享库第一次被调用时。共享库的另一个优点是可以用库函数的新版本代替老版本而无需对使用该库的程序重新链接编辑（假设参数的数目和类型都没有发生变化）。

## 3.7 储存空间分配

ISO C说明了3个用于储存空间动态分配的函数。

1. malloc，分配指定字节数的储存区。此储存区的初始值不确定。
2. calloc，为只多功能数量指定长度的对象分配储存空间。该空间每一位（bit）都初始化为0.
3. realloc，增加或者减少以前分配区的长度。当增加长度时，可能需要将以前分配的内容移到另一个足够大的区域，以便在尾端提供增加的储存区，而新增区域内的初始值则不确定。

```c
    #include <stdlib.h>

    void *malloc(size_t size);
    void *calloc(size_t mobj, size_t size);
    void *realloc(void *ptr, size_t newsize);

    void free(void *ptr);
```
这三个函数所返回的指针是适当对其的，使其可用于各种对象。这些分配例程通常使用sbrk(2)系统调用实现。该系统调用扩充（或缩小）进程的堆。

**替代的储存空间分配程序**

有很多可以替代malloc和free的函数。某些系统已经提供代替储存空间分配函数的库，另一些系统只提供标准储存空间分配程序。

1. libmalloc
2. vmalloc
3. quick-fit
4. jemalloc
5. TCMalloc
6. alloca

## 3.8 环境变量

环境变量的字符串形式是：
name = value

ISO C定义了一个函数getenv，可以用其取环境变量的值，但是该标准又称环境的内容是由实现定义的。

```c
    #include <stdlib.h>

    char *getenv(const char*name);
```
函数返回一个指针，它指向name=value字符串中的value。我们应当使用getenv从环境变量中取一个指定环境变量的值，而不是访问environ。

除了获取环境变量的值，有时也需要设置环境变量。但是并不是所有的系统都支持这种能力
```c
    #include <stdlib.h>

    int putenv(char *string);   //若成功，返回0；若出错，返回非0
    int setenv(const char *name, const char *value, int overwrite);
    int unsetenv(const char *name);
    //两个函数返回值：若成功，返回0；若出错，返回-1；
```

## 7.9 函数setjmp和longjmp

在C中，goto语句是不能跨越函数的，而执行这种跳转功能的是函数setjmp和longjmp。这两个函数在处理发生在很深层嵌套函数调用中的出错情况是非常有用的。

我们知道，每个进程在内核中都有一个进程控制块(PCB)来维护进程相关的信息，Linux内核进程控制块是task_struct结构体.

    * 进程id。系统中每个进程有唯一的id,在C语言中用pid_t类型表示,其实就是一个非负整数。

    * 进程的状态,有运行、挂起、停止、僵尸等状态。

    * 进程切换时需要保存和恢复的一些CPU寄存器。

    * 描述虚拟地址空间的信息。

    * 描述控制终端的信息。

    * 当前工作目录(Current Working Directory)。

    * umask掩码。

    * 文件描述符表,包含很多指向file结构体的指针。

    * 和信号相关的信息。

    * 用户id和组id。

    * 控制终端、Session和进程组。

    * 进程可以使用的资源上限(Resource Limit)。

    // cat /proc/self/limits
    
    // ulimit -a

## 3.1  环境变量

libc中定义的全局变量environ指向环境变量表，environ没有包含在任何头文件中，所以在使用时要用extern声明。例如：

```c
    #include <stdio.h>
    int main(void)
    {
        extern char **environ;
        int i;
        for(i=0; environ[i]!=NULL; i++)
            printf("%s\n", environ[i]);
        return 0;
    }
```    

由于父进程在调用fork创建子进程时会把自己的环境变量表也复制给子进程，所以a.out打印的环境变量和Shell进程的环境变量是相同的。
    
按照惯例，环境变量字符串都是name=value这样的形式，大多数name由大写字母加下划线组成，一般把name的部分叫做环境变量，value的部分则是环境变量的值。环境变量定义了进程的运行环境，一些比较重要的环境变量的含义如下：

PATH

* 可执行文件的搜索路径。ls命令也是一个程序，执行它不需要提供完整的路径名/bin/ls，然而通常我们执行当前目录下的程序a.out却需要提供完整的路径名./a.out，这是因为PATH环境变量的值里面包含了ls命令所在的目录/bin，却不包含a.out所在的目录。PATH环境变量的值可以包含多个目录，用:号隔开。在Shell中用echo命令可以查看这个环境变量的值：

$ echo $PATH

SHELL

* 当前Shell，它的值通常是/bin/bash。

TERM

* 当前终端类型，在图形界面终端下它的值通常是xterm，终端类型决定了一些程序的输出显示方式，比如图形界面终端可以显示汉字，而字符终端一般不行。

LANG

* 语言和locale，决定了字符编码以及时间、货币等信息的显示格式。

HOME
* 当前用户主目录的路径，很多程序需要在主目录下保存配置文件，使得每个用户在运行该程序时都有自己的一套配置。

用environ指针可以查看所有环境变量字符串，但是不够方便，如果给出name要在环境变量表中查找它对应的value，可以用getenv函数。
```c   
    #include <stdlib.h>

    char *getenv(const char *name);
```  
getenv的返回值是指向value的指针，若未找到则为NULL。
修改环境变量可以用以下函数
```c
        #include <stdlib.h>

        int setenv(const char *name, const char *value, int rewrite);
        void unsetenv(const char *name);
```    
putenv和setenv函数若成功则返回为0，若出错则返回非0。
setenv将环境变量name的值设置为value。如果已存在环境变量name，那么
若rewrite非0，则覆盖原来的定义；
若rewrite为0，则不覆盖原来的定义，也不返回错误。
unsetenv删除name的定义。即使name没有定义也不返回错误。
例 修改环境变量
```c    
    #include <stdlib.h>
    #include <stdio.h>
    int main(void)
    {
        printf("PATH=%s\n", getenv("PATH"));
        setenv("PATH", "hello", 1);
        printf("PATH=%s\n", getenv("PATH"));
        return 0;
    }
```    
## 3.2  进程状态

    修改进程资源限制，软限制可改，最大值不能超过硬限制，硬限制只有root用户可以修改
```c
    #include <sys/time.h>
    #include <sys/resource.h>
    int getrlimit(int resource, struct rlimit *rlim);
    int setrlimit(int resource, const struct rlimit *rlim);
```

查看进程资源限制
```sh
    cat /proc/self/limits
    ulimit -a
```        
##  3.3  进程原语

### 3.3.1  fork
```c
    #include <unistd.h>
    
    pid_t fork(void);
```        
        子进程复制父进程的0到3G空间和父进程内核中的PCB，但id不同。
        fork调用一次返回两次
        
        + 父进程中返回子进程ID
        + 子进程中返回0
        + 读时共享，写时复制
        fork()在父进程中的返回值与子进程中的getpid()返回值相同。

        fork
```c
    #include <sys/types.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    int main(void)
    {
        pid_t pid;
        char *message;
        int n;
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0) {
            message = "This is the child\n";
            n = 6;
        } else {
            message = "This is the parent\n";
            n = 3;
        }
        for(; n > 0; n--) {
            printf(message);
            sleep(1);
        }
        return 0;
    }
```

进程相关函数
    getpid/getppid
```c        
    #include <sys/types.h>
    #include <unistd.h>
    pid_t getpid(void); //返回调用进程的PID号
    pid_t getppid(void); //返回调用进程父进程的PID号
```
        getuid
```c        
    #include <unistd.h>
    #include <sys/types.h>
    uid_t getuid(void); //返回实际用户ID
    uid_t geteuid(void); //返回有效用户ID，如调用sudo指令
```
        getgid
```c
    #include <unistd.h>
    #include <sys/types.h>
    gid_t getgid(void); //返回实际用户组ID
    gid_t getegid(void); //返回有效用户组ID
```
    
vfork
    
* 用于fork后马上调用exec函数
    
* 父子进程，共用同一地址空间,子进程如果没有马上exec而是修改了父进程出得到的变量值，此修改会在父进程中生效
    
* 设计初衷，提高系统效率，减少不必要的开销
    
* 现在fork已经具备读时共享写时复制机制，vfork逐渐废弃

### 3.3.2  exec族

        用fork创建子进程后执行的是和父进程相同的程序（但有可能执行不同的代码分支），子进程往往要调用一种exec函数以执行另一个程序。当进程调用一种exec函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动例程开始执行。调用exec并不创建新进程，所以调用exec前后该进程的id并未改变。其实有六种以exec开头的函数，统称exec函数：

           #include <unistd.h>

           extern char **environ;

           int execl(const char *path, const char *arg, ...
                           /* (char  *) NULL */);
           int execlp(const char *file, const char *arg, ...
                           /* (char  *) NULL */);
           int execle(const char *path, const char *arg, ...
                           /*, (char *) NULL, char * const envp[] */);
           int execv(const char *path, char *const argv[]);
           int execvp(const char *file, char *const argv[]);
           int execvpe(const char *file, char *const argv[],
                           char *const envp[]);

        这些函数如果调用成功则加载新的程序从启动代码开始执行，不再返回，如果调用出错则返回-1，所以exec函数只有出错的返回值而没有成功的返回值。
        这些函数原型看起来很容易混，但只要掌握了规律就很好记。不带字母p（表示path）的exec函数第一个参数必须是程序的相对路径或绝对路径，例如“/bin/ls”或“./a.out”，而不能是“ls”或“a.out”。对于带字母p的函数：
        如果参数中包含/，则将其视为路径名。
        否则视为不带路径的程序名，在PATH环境变量的目录列表中搜索这个程序。
        带有字母l（表示list）的exec函数要求将新程序的每个命令行参数都当作一个参数传给它，命令行参数的个数是可变的，因此函数原型中有…，…中的最后一个可变参数应该是NULL，起sentinel的作用。对于带有字母v（表示vector）的函数，则应该先构造一个指向各参数的指针数组，然后将该数组的首地址当作参数传给它，数组中的最后一个指针也应该是NULL，就像main函数的argv参数或者环境变量表一样。
        对于以e（表示environment）结尾的exec函数，可以把一份新的环境变量表传给它，其他exec函数仍使用当前的环境变量表执行新程序。
        exec调用举例如下：

        char *const ps_argv[] ={"ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL};
        char *const ps_envp[] ={"PATH=/bin:/usr/bin", "TERM=console", NULL};
        execl("/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
        execv("/bin/ps", ps_argv);
        execle("/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL, ps_envp);
        execve("/bin/ps", ps_argv, ps_envp);
        execlp("ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
        execvp("ps", ps_argv);

        事实上，只有execve()是真正的系统调用，其它五个函数最终都调用execve()，所以execve()在man手册第2节，其它函数在man手册第3节。这些函数之间的关系如下图所示。

        一个完整的例子：
```
        #include <unistd.h>
        #include <stdlib.h>
        int main(void)
        {
        execlp("ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
        perror("exec ps");
        exit(1);
        }
```

        由于exec函数只有错误返回值，只要返回了一定是出错了，所以不需要判断它的返回值，直接在后面调用perror即可。注意在调用execlp时传了两个“ps”参数，第一个“ps”是程序名，execlp函数要在PATH环境变量中找到这个程序并执行它，而第二个“ps”是第一个命令行参数，execlp函数并不关心它的值，只是简单地把它传给ps程序，ps程序可以通过main函数的argv[0]取到这个参数。
        调用exec后，原来打开的文件描述符仍然是打开的。利用这一点可以实现I/O重定向。
        先看一个简单的例子，把标准输入转成大写然后打印到标准输出：
        例 upper

```
        /* upper.c */
        #include <stdio.h>
        int main(void)
        {
        int ch;
        while((ch = getchar()) != EOF) {
        putchar(toupper(ch));
        }
        return 0;
        }

        例 wrapper

        /* wrapper.c */
        #include <unistd.h>
        #include <stdlib.h>
        #include <stdio.h>
        #include <fcntl.h>
        int main(int argc, char *argv[])
        {
        int fd;
        if (argc != 2) {
        fputs("usage: wrapper file\n", stderr);
        exit(1);
        }
        fd = open(argv[1], O_RDONLY);
        if(fd<0) {
        perror("open");
        exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
        execl("./upper", "upper", NULL);
        perror("exec ./upper");
        exit(1);
        }
```

        wrapper程序将命令行参数当作文件名打开，将标准输入重定向到这个文件，然后调用exec执行upper程序，这时原来打开的文件描述符仍然是打开的，upper程序只负责从标准输入读入字符转成大写，并不关心标准输入对应的是文件还是终端。运行结果如下：

        exec族
        l 命令行参数列表
        p 搜素file时使用path变量
        v 使用命令行参数数组
        e 使用环境变量数组,不使用进程原有的环境变量，设置新加载程序运行的环境变量

### 3.3.3  wait/waitpid

        僵尸进程: 子进程退出，父进程没有回收子进程资源（PCB），则子进程变成僵尸进程
        孤儿进程: 父进程先于子进程结束，则子进程成为孤儿进程,子进程的父进程成为1号进程init进程，称为init进程领养孤儿进程

        #include <sys/types.h>
        #include <sys/wait.h>
        pid_t wait(int *status);
        pid_t waitpid(pid_t pid, int *status, int options);
        < -1 回收指定进程组内的任意子进程
        -1 回收任意子进程
        0 回收和当前调用waitpid一个组的所有子进程
        \> 0 回收指定ID的子进程

        一个进程在终止时会关闭所有文件描述符，释放在用户空间分配的内存，但它的PCB还保留着，内核在其中保存了一些信息：如果是正常终止则保存着退出状态，如果是异常终止则保存着导致该进程终止的信号是哪个。这个进程的父进程可以调用wait或waitpid获取这些信息，然后彻底清除掉这个进程。我们知道一个进程的退出状态可以在Shell中用特殊变量$?查看，因为Shell是它的父进程，当它终止时Shell调用wait或waitpid得到它的退出状态同时彻底清除掉这个进程。
        如果一个进程已经终止，但是它的父进程尚未调用wait或waitpid对它进行清理，这时的进程状态称为僵尸（Zombie）进程。任何进程在刚终止时都是僵尸进程，正常情况下，僵尸进程都立刻被父进程清理了，为了观察到僵尸进程，我们自己写一个不正常的程序，父进程fork出子进程，子进程终止，而父进程既不终止也不调用wait清理子进程：

        #include <unistd.h>
        #include <stdlib.h>
        int main(void)
        {
        pid_t pid=fork();
        if(pid<0) {
            perror("fork");
            exit(1);
        }
        if(pid>0) { /* parent */
            while(1);
        }
        /* child */
        return 0;
        }

        若调用成功则返回清理掉的子进程id，若调用出错则返回-1。父进程调用wait或waitpid时可能会：
        * 阻塞（如果它的所有子进程都还在运行）。
        * 带子进程的终止信息立即返回（如果一个子进程已终止，正等待父进程读取其终止信
        息）。
        * 出错立即返回（如果它没有任何子进程）。
        这两个函数的区别是：
        * 如果父进程的所有子进程都还在运行，调用wait将使父进程阻塞，而调用waitpid时如果在options参数中指定WNOHANG可以使父进程不阻塞而立即返回0。
        * wait等待第一个终止的子进程，而waitpid可以通过pid参数指定等待哪一个子进程。可见，调用wait和waitpid不仅可以获得子进程的终止信息，还可以使父进程阻塞等待子进程终止，起到进程间同步的作用。如果参数status不是空指针，则子进程的终止信息通过这个参数传出，如果只是为了同步而不关心子进程的终止信息，可以将status参数指定为NULL。
        例 waitpid

        #include <sys/types.h>
        #include <sys/wait.h>
        #include <unistd.h>
        #include <stdio.h>
        #include <stdlib.h>
        int main(void)
        {
            pid_t pid;
            pid = fork();
            if (pid < 0) {
                perror("fork failed");
                exit(1);
        }
        if (pid == 0) {
                int i;
                for (i = 3; i > 0; i--) {
                printf("This is the child\n");
                sleep(1);
        }
        exit(3);
        } else {
        int stat_val;
        waitpid(pid, &stat_val, 0);
        if (WIFEXITED(stat_val))
        printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        else if (WIFSIGNALED(stat_val))
        printf("Child terminated abnormally, signal %d\n", WTERMSIG(stat_val));
        }
        return 0;
        }

        wait阻塞函数，阻塞等待子进程结束waitpid 4种情况 < -1 = -1 = 0 > 0
        进程的退出状态
        非阻塞状态，WNOHANG
        获取进程退出状态的函数见manpages
