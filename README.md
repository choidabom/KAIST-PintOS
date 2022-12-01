# PintOS

Brand new pintos for Operating Systems and Lab (CS330), KAIST, by Youngjin Kwon.

The manual is available at https://casys-kaist.github.io/pintos-kaist/.

## SW Jungle Week08 (2022.11.10 ~ 11.16)

## PROJECT 1: THREADS

<details><summary style="color:skyblue">PintOS에서 thread와 process의 관계</summary>
<p>
실제 OS(Operating System)에서는 하나의 process 안에 여러 개의 thread가 존재할 수 있다. 여러 개의 thread들은 같은 virtual address space를 공유한다. PintOS에서는 구현을 단순화하기 위해서 하나의 process에 하나의 thread만 있도록 구성되어 
</p>
</details>
  
### WIL (Weekly I Learned)
### 11.10 목

- 작업환경 세팅 (EC2, Ubuntu18.04) + repo 생성
- PintOS project Git Book 읽기

### 11.11 금

- #### KAIST 권영진 교수님 OS 첫 번째 강의
- ~~**공부로 도망치지 마세요.** from 코치님~~

### 11.12 토

#### PintOS 전반적인 코드 파악

- `/threads/init.c`
- `/threads/thread.c`
- `/lib/kernel/list.c & /include/lib/kernel/list.h`
- `/tests/threads/tests.c`
- `/threads/synch.c`

### 11.13 일

### Alarm clock 구현

- Alarm Clock이란?
  - **호출한 프로세스를 정해진 시간 후에 다시 시작하는 커널 내부 함수**
  - PintOS에서는 thread가 CPU를 사용할 시간(할당시간)은 4 tick(40ms)으로 한정되어 있음
  - 4 tick이 지나면 thread는 ready_list의 맨 뒤에 추가됨
- **문제점: Pintos의 Alarm clock은 loop 기반의 Busy waiting 방식**
  - Busy waiting 방식: time_interrupt가 일어날 때마다 ready_list를 순회하며 thread들이 사용해야할 타이밍인지를 체크함
  - 즉, 자고 있는 thread들 중에 아직 일어날 시간이 아닌 thread들도 running에 상태에 넣어 CPU를 쓰면서 대기하고 있는 상태
- **목표: 프로세스를 재울 때 시스템 자원 낭비를 최소화하는 것**
- **문제 해결: sleep/wake up으로 문제점 개선**
  - sleep_list를 하나 만들어서 thread들을 blocked된 상태(잠자는 상태)로 넣어줌
  - time_interrupt가 일어날 때마다 sleep_list에 존재하는 thread들 중 깨워야할 tick이 된 thread를 ready_list에 넣고 상태를 ready로 바꿔줌
  - 아직 깨어날 시간(tick)이 되지 않은 thread들은 CPU를 사용하지 않고 대기할 수 있기에 문제 해결

### 11.14 월

### Priority Scheduling (1) Priority Scheduling 구현

- Scheduling란?
  - 여러 프로세스가 번갈아가며 사용하는 자원을 어떤 시점에 어떤 프로세스에게 자원을 할당할 지 결정하는 것
- **문제점: thread들 간의 우선순위 없이 ready_list에 들어온 순서대로 실행됨(Round-Robin 방식)**

  - Pintos는 Round-Robin 방식 채택, 할당된 시간 4 tick이 지나면 running thread는 다른 thread에게 선점당함
  - Pintos는 새로운 thread를 ready_list에 넣을 때 항상 맨 뒤에 넣고, ready_list에서 다음 CPU에 할당할 thread를 찾을 때는 앞에서 꺼냄

- **목표: thread의 우선순위대로 scheduling 하는 것**
- **문제 해결: 우선순위를 비교하여 우선순위가 가장 높은 thread를 ready_list의 맨 앞에 위치시킴**

  - thread를 ready_list에 우선순위대로 정렬되도록 삽입
  - ready_list에서 thread를 뺄 때 앞에서부터 빼기 때문

  - **문제 해결에서의 문제: running thread가 가장 높은 priority를 가지는 것을 보장하지 못 함** 1. 새로운 thread의 priority가 현재 running하고 있는 thread보다 우선순위가 높아질 때 2. thread의 priority가 변경될 때
    => ready_list에서 우선순위가 가장 높은 thread와 running thread를 비교하여 우선순위가 더 높은 thread가 선점할 수 있게 해줌

### 11.15 화

### Priority Scheduling (2) Semaphore, Lock, Condition Variable 구현

- Semaphore, Lock, Condition Variable을 사용하여 Priority Scheduling 개선
  - PintOS는 여러 thread들이 Semaphore, Lock, Condition Variable를 얻기 위해 기다릴 경우 먼저 온 thread가 먼저 사용하는 FIFO 방식을 사용
  - **Sychronization 도구들을 기다릴 때, 우선순위가 가장 높은 thread가 CPU를 점유하도록 구현**

### 11.16 수

### Priority Scheduling (3) Priority donation(Prority inversion problem) 구현

- **문제점: 여러 thread가 lock을 요청하고 점유하는 과정에서 Priority Inversion 발생**
  - priority가 높은 thread가 priority가 낮은 thread를 기다리는 현상
- **문제 해결: Priority donation**
  - Multiple donation
    - thread가 두 개 이상의 lock 보유 시 각 lock을 획득하고자 하는 thread들에게 donation 발생가능
    - 예시) thread L이 lock A와 lock B를 점유하고 있을 때 thread M이 lock A를 요청, thread H가 lock B를 요청하는 상황
  - Nested donation
    - 여러 번의 단계적 donation이 일어나는 상황

### 11.17 목

#### Project 1 결과

- 10:00 ~ 11:00 발표 진행
<p align="center"><img width="50%" src="https://user-images.githubusercontent.com/48302257/202519011-3aa9d8eb-9f14-466c-a3ac-fa41bfb67ea1.png"/></p>

## SW Jungle Week09 (2022.11.17 ~ 11.28)

## PROJECT 2: USER PROGRAMS

### WIL (Weekly I Learned)

### 11.18 금

- Git book 과제 설명서 공부

### 11.19 토

### Argument Passing 큰 그림 그리기

<p align="center"><img width="70%" src="https://user-images.githubusercontent.com/48302257/204595998-f91011f5-cbd8-482a-99ab-19afae17009e.png"/></p>

- `init.c`
- init.c의 main 함수에서 `read_command_line()`함수를 호출하여 명령어를 읽어온다. -> argv
  명령어로 들어오는 인자의 형태는 명령어와 그 명령어의 대상이다.
- 예를 들어 인자가 1개만 들어오는 경우(args-single.ck)라면, argv는 `run 'args-single onearg'`의 형태를 가진다.
- 호출된 명령어 parse_options를 통해 option에 따라 명령어를 적절히 parsing한다.
- 명령어는 run_action 함수의 인자로 전달된다. argv안의 명령어 문자열에 있는 명령어 run은 run_task 함수를 호출한다.
- run_task 함수에서 task = argv[1]로 정해진다. 그 이유는 명령어로 들어오는 인자의 형태는 명령어와 그 명령어의 대상이다. 따라서 명령어의 대상에 해당되는 인자인 `'args-single onearg'`가 `process_create_initd`의 인자가 된다.
  해당 인자는 프로그램 파일이름과 프로그램들의 인자들이 같이 위치한다. 따라서 프로그램 파일명과 인자들을 공백을 기준으로 parsing하여야 한다.

- `process.c`
- `process_create_initd()`의 인자로 입력받은 명령어의 대상(task = argv[1])이 들어온다. 해당 문자열은 thread_create의 인자로 들어가서 해당 인자를 이름으로 한 kernel 스레드를 생성한다. 해당 스레드는 생성된 이후 running thread가 되는 시점에 인자로 들어간 함수 initd를 호출한다.
- `initd()` 함수는 `process_exec()`함수를 호출한다.
- `process_exec()` 함수는 `load()`함수를 호출한다.
- load함수에서 입력받은 인자를 parsing하고 해당 커널 스레드의 인터럽트 프레임에 인자의 주소를 저장하고, 이후 실행할 명령어의 주소를 스택에 저장한다.(if->rip)
  이때 filesys_open에 전달되는 file_name은 입력받은 인자를 공백으로 parsing할 때 등장하는 첫 번째 문자열이다.두 번째 문자열부터 인자가 된다.
- 파싱한 문자열을 스택에 쌓는다. 스택은 주소가 감소하면서 확장한다.(위->아래). 인자 -> 8바이트 정렬을 맞추기 위한 공백 공간 -> 파싱한 인자의 스택 주소 -> 가짜 반환 주소 순으로 저장한다.

**과정 중 헷갈렸던 부분과 알게 된 점**

1. `userprog/process.c`의 `tid_t process_create_initd (const char *file_name)`

- Q. tid를 반환하는 `thread_create` 함수에서 인자로 받은 함수가 언제 실행되는 것인가?
  - `thread_create(file_name, PRI_DEFAULT, initd, fn_copy)`를 호출하며 새로운 kernel 스레드를 생성한다. 이 kernel 스레드는 `initd()` 함수를 thread routine으로 가지는 스레드로, 생성 후 ready list에 들어간 뒤, running thread가 되는 시점에 `initd()`를 실행한다.

### 11.20 일

### Argument Passing

- **목표: 입력받은 인자를 공백을 기준으로 파싱하기**
- **문제 해결: strok_r함수를 이용하여 공백을 기준으로 파싱함.**

1. 파싱한 인자의 첫 번째 문자열은 프로그램 이름이 된다. => process_name
2. 파싱한 모든 문자열을 스택에 쌓는다. (스택의 주소를 감소시키면서)
3. 문자열을 저장하고 8byte word-align을 맞춰주기 위해 남은 공간에 null을 넣어준다.
4. 2단계에서 넣은 문자열들이 위치한 스택 주소를 넣어준다.
5. 가짜 반환 주소를 넣어준다. 함수가 호출된 이후 return 된 이후 pc가 읽을 인스트럭션 주소를 return address로 넣는다.

다만 해당 함수는 반환되지 않기 때문에 해당 return address로 이동하지 않는다. 하지만 다른 스택 프레임과 동일한 구조를 갖기 위해서 가짜 반환주소를 넣는다.

<p align="center"><img width="80%" src="https://user-images.githubusercontent.com/48302257/204596426-353ede05-f05e-4164-a0d2-8f0e1ca2cf10.png"/></p>

- Argument Passing은 `load()`(in userprog/process.c)내에서 구현한다.

### 11.21 월

### System Calls

- **`thread.h` 내의 struct fild_fd 선언과 struct thread 내 멤버 추가**

```c
struct file_fd
{
	int fd;					          /* fd: 파일 식별자 */
	struct file *file;		    /* file */
	struct list_elem fd_elem; /* list 구조체의 구성원 */
};
```

- 파일을 open하게 되면 file을 open하고 이에 대응하는 fd를 갱신하여 mapping할 필요가 있음
- 이를 위해 file과 fd를 멤버변수로 갖는 file_fd 구조체를
- open 할때는 새로운 file_fd 구조체를 만들고 file을 열고 fd를 갱신하여 저장함.
- close 할때는 close 하려는 fd에 대응하는 file을 닫고 fd를 NULL로 변경함.

```c
struct thread
{
	....
	struct list fd_list;		/* file_fd 구조체를 저장하는 Doubley Linked List */
	int fd_count;			/* fd를 확인하기 위한 count*/
	int exit_status
	struct semaphore fork_sema;   	/* 자식 프로세스의 fork가 완료될 때까지 기다리도록 하기 위한 세마포어 */
	struct semaphore wait_sema;
	struct semaphore exit_sema;

	struct list child_list;		/* 자식 스레드를 보관하는 리스트 */
	struct list_elem child_elem;  	/* 자식 리스트 element */
	struct file *now_file;        	/* 현재 프로세스가 실행 중인 파일을 저장하기 위한 변수  */
	....
};
```

- open 된 파일을 확인하기 위한 fd를 저장하는 fd_list라는 list를 선언함.
- fd_elem은 fd_list라는 연결 리스트에 list_elem(fd_elem)으로 삽입되어 저장됨.
- fd_count는 file을 열 때마다 갱신하기 위한 변수이다. 파일을 open할 때마다 fd_count를 1씩 증가시키고 증가된 fd_count를 fd로 할당한다. 그렇기에 open될 때마다 서로 다른 새로운 fd를 할당할 수 있게 한다.

### 11.22 화

### System Calls

- **목표: 총 14개의 구현해야할 syscall**
- 프로세스 관련 system call
  - `halt()`, `wait()`, `fork()`, `exit()`, `exec()`
- 파일 관련 system call

  - `open()`, `filesize()`, `close()`, `read()`, `write()`, `seek`(), `tell()`, `create()`, `remove()`

- **create()와 remove()를 제외한 파일 관련 system call들은 file descriptor를 반환하거나, file descriptor를 이용해서 file에 대한 작업을 수행한다.**
- kernel은 file descriptor와 실제 file 구조체를 매핑하여 관리하며 이를 위한 도구가 바로 **fd table**이다.

### System Calls - halt(), exit()

- `halt()`: pintos를 종료시키는 시스템 콜
  - `power_off()`함수를 사용하여 pintos를 종료시켰다.
- `exit()`: 현재 프로세스를 종료시키는 시스템 콜
  - 스레드 구조체 안에 exit_status 멤버 변수를 선언하여 인자로 받은 종료 상태를 갱신한다.

### Process Termination Message

- `exit()` 함수를 호출했거나 다른 어떤 이유들로 유저 프로세스 종료 시 프로세스 이름과 exit code를 아래와 같이 지정된 형식으로 출력한다.
  - `printf("%s: exit(%d)\n", ....);`

### 11.23 수

### System Calls - filesize(), seek(), tell()

- `filesize()`: fd로 열려있는 파일 사이즈를 리턴해주는 시스템 콜
  - 열려져 있는 file을 관리하는 `fd_list`를 순회하면서 찾으려는 fd와 mapping된 file을 찾는다.
  - file의 크기를 byte단위로 반환하는 함수 `file_length()`를 호출한다.
- `seek()` : fd로 열려있는 파일의 (읽고 쓸 위치를 알려주는) 포인터의 위치를 변경해주는 시스템 콜
  - 열려져 있는 file을 관리하는 `fd_list`를 순회하면서 찾으려는 fd와 mapping된 file을 찾는다.
  - file의 현재 위치를 인자로 들어간 position으로 변경시켜 주는 함수 `file_seek()`를 호출한다.
- `tell()` : fd에서 읽히거나 써질 다음 바이트의 위치를 반환
  - 열려져 있는 file을 관리하는 `fd_list`를 순회하면서 찾으려는 fd와 mapping된 file을 찾는다.
  - file의 현재 위치를 알려주는 함수 `file_tell()`를 호출한다.
- ~~ `filesize()`, `seek()`, `tell()`은 시스템 콜이 잘 동작하는지 확인할 수 있는 test case 부제로 test를 돌릴 수 있는 시스템 콜에서 호출하는 방식으로 호출됨을 확인하였다.~~

### 11.24 목

### System Calls - create(), remove()

- `create()` : 파일을 생성하는 시스템 콜

  - check_address함수를 선언하여 포인터가 가르키는 주소가 유저 영역에 존재하는지, 페이지가 할당되었는지 여부를 확인한다.
  - 파일명 인자 file이 NULL일 경우 exit(-1)을 실행하도록 한다.
  - file을 이름으로 하고 initial_size인자를 크기로 한 file을 생성하고, 성공여부를 반환하는 함수 `filesys_create()`를 호출 한다.
  - 파일이 성공적으로 생성되면 true를 아니면 false를 반환한다.

- `remove()`: 파일을 삭제하는 시스템 콜
  - check_address함수를 선언하여 포s인터가 가르키는 주소가 유저 영역에 존재하는지, 페이지가 할당되었는지 여부를 확인한다.
  - file이라는 이름을 가진 파일을 삭제하고 성공여부를 반환하는 함수 `filesys_remove()`를 호출한다.

### 11.25 금

### System Calls - read()

- `read()` : fd를 통해 열린 파일의 데이터를 읽는 시스템 콜.
  - fd가 0일 때는 키보드의 데이터를 읽어 버퍼에 저장하고 그 크기를 반환. -> input_getc() 활용
  - fd가 0보다 작거나 비어 있거나 1일 경우에는 read에서 유효하지 않은 fd이므로 exit(-1)
  - 파일에 동시 접근이 이뤄질 수 있으므로 이를 방지하기 위해 lock 구조체 filesys_lock을 선언하여 활용.
  - 읽은 데이터의 크기를 byte단위로 반환하는 함수, `file_read()` 함수를 호출함.

### 11.26 토

### System Calls - write()

- `write()` : fd를 통해 열린 파일의 데이터를 기록하는 시스템 콜.
  - fd가 1이라는 의미는 표준 출력을 의미한다. 따라서 문자열을 화면에 출력해주는 putbuf() 함수 활용 (putbuf 함수는 buffer에 입력된 데이터를 size만큼 화면에 출력하는 함수)
  - fd가 0보다 작거나 비어 있는 경우에는 write에서 유효하지 않은 fd이므로 exit(-1)
  - 파일에 동시 접근이 이뤄질 수 있으므로 이를 방지하기 위해 lock 구조체 filesys_lock을 선언하여 활용.
  - fd가 1이 아닐 경우 버퍼에 저장된 데이터를 크기만큼 파일에 기록한 데이터의 크기를 byte 단위로 반환하는 함수, `file_write()` 함수를 호출함.

### Deny Write on Executables

- **문제: 실행 중인 파일에 쓰기 작업을 수행하면 예상치 못한 결과 얻을 수 있다.**
- **목표: 실행 중인 파일에 쓰기 작업을 수행하지 않도록 하는 것**
- **문제 해결: 함수 활용**
  - `file_deny_write()`: 파일을 open 할 때, 실행 파일에 대해 쓰기를 방지한다. 메모리에 파일을 load한 후에 수정하면 안 되기 때문이다. 이를 통해, file synchronization Issue를 해결할 수 있다.
  - `file_allow_write()`: 파일의 데이터가 변경되는 것을 허락하는 함수이다. `file_close()` 함수 호출 시 해당 함수가 호출된다.

### 11.27 일

### System Calls - exec(), fork()

- `exec()` : 현재 프로세스를 인자로 주어진 이름을 갖는 실행 파일로 변경하는 시스템 콜.

  - 새로운 file을 palloc_get_page를 통해 주소를 할당한다.
  - 인자로 주어진 file_name 문자열을 strlcpy를 통해 새롭게 생성된 file 문자열에 복사한다.
  - file을 실행한다. -> `process_exec()` 호출한다.
  - process_exec()의 반환값이 -1이면 성공하지 못했다는 의미이므로 -1을 반환한다.

- `fork()` : 현재 프로세스의 복제본인 프로세스를 생성하는 시스템 콜

  - 자식 프로세스의 tid를 반환해야함. 유효한 tid가 아닌 경우 0을 반환해야 함.
  - process_fork()를 호출하여 부모의 데이터와 유저레벨의 인터럽트 프레임을 복사한 자식 스레드를 만든다.
    - thread_create를 통해 자식 스레드를 생성한다. 자식 스레드는 생성된 뒤 ready_list에 들어가 running thread가 될 때 인자로 들어간 함수 do_fork를 실행한다.
    - do_fork()를 실행할 때 인터럽트가 호출될 당시 레지스터에서 작업하던 context 정보를 그대로 복사하여야 한다. 즉 부모 프로세스의 정보를 그대로 복사한다는 의미이다. 이때 주의할 점은 복사할 것은 부모 프로세스의 userland context이다.
      - syscal-entry.S를 보면 9행의`movq %rsp, %rbx`를 통해 rbx에 유저스택포인터가 저장되고 있으며, 10-12행의 `movabs $tss, %r12` -> `movq (%r12), %r12` -> `movq 4(%r12), %rsp`를 통해 rsp에는 커널 스택 포인터가 저장된다.
      - 따라서 intf_frame tf->rsp에는 커널스택의 정보가 담겨 있고 syscall_handler의 인자 intr_fram f가 유저스택의 정보를 갖고 있다. 그래서 이 인자를 그대로 넘겨 주어야 한다.
      - 이를 위해 구조체 `fork_data`를 선언하였다. `fork_data` 구조체의 멤버변수로 `thread *parent`와 `intr_frame *user_level_f`를 활용하여 부모 스레드와 유저레벨 인터럽트를 저장한 후 해당 구조체를 `__do_fork`의 인자로 전달하였다.
    - `__do_fork`함수 내부에서 `pml4_for_each()`함수를 통해 인자로 `duplicate_pte()`함수를 실행하여 실행부모의 유저 메모리 공간을 복사하여 새로 생성한 자식의 페이지에 넣어준다.
    - 또한 부모 프로세스의 `fd_list`의 요소들과 `fd_count`를 복사하여준다.
    - 세마포어 `fork_sema`를 활용해서 자식 프로세스에 대한 복사가 완료될때까지 `process_fork`함수가 끝나지 않도록 하였다.
  - 자식 스레드의 반환 값은 0이고 자식 스레드가 생성이 실패할 때 `TID_ERROR`를 반환하도록 한다.

### 11.28 월

#### KAIST 권영진 교수님 OS 두 번째 강의

### System Calls - wait()

- `wait()` : 자식 프로세스가 수행되고 종료될 때까지 부모 프로세스가 대기하는 시스템 콜
  - process_wait() 함수를 호출한다.

### 고난과 역경의 과정이 발생했던 다수의 이유

1. **`process_exit()`에서 open되어있는 파일을 close 하지 않았던 문제**

```c
	struct list *exit_list = &curr->fd_list;
	struct list_elem *start = list_begin(exit_list);
	while (!list_empty(exit_list))
	{
		struct file_fd *exit_fd = list_entry(start, struct file_fd, fd_elem);
		file_close(exit_fd->file);
		start = list_remove(&exit_fd->fd_elem);
		free(exit_fd);
	}
```

- `process_exit()` 즉, **프로세스를 종료해야할 시점에서 열린 모든 파일을 close 해주어야한다.**
  fd_list에 있는 현재 스레드의 fd_list(exit_list)가 빌 때까지 while문을 돌면서 열린 파일들을 close 해주고, fd_elem을 list에서 지움으로써 해당 파일이 fd_list에서 삭제하도록 해주었다.
- fork와 open을 할 때 file_fd 선언을 위해 malloc을 해주었기 때문에 exit할 때 free를 해줌으로써 메모리 누수가 생기지 않도록 한다.

2. **`write()`와 `read()`를 할 때 입력받는 인자 buff가 올바른 address 인지 체크하지 않았던 문제**

- buffer => 기록할 데이터를 저장한 버퍼의 주소 값이 유저 가상 공간도 아니거나, 페이지가 할당되어있지 않거나, 주소가 NULL인 경우를 체크해준다.

3. **`process_wait()`와 `process_exit()` sema_down과 sema_up의 순서파악**
<p align="center"><img width="80%" src="https://user-images.githubusercontent.com/48302257/204840230-f3f61249-f09d-4996-839c-788266333923.png"/></p>

### 11.29 화

#### Project 2 결과

- 11:00 ~ 12:00 발표 진행

