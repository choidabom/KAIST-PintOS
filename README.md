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

- #### KAIST 권영진 교수님 OS 강의
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
 
  - **문제 해결에서의 문제: running thread가 가장 높은 priority를 가지는 것을 보장하지 못 함**
      1. 새로운 thread의 priority가 현재 running하고 있는 thread보다 우선순위가 높아질 때
      2. thread의 priority가 변경될 때
  =>  ready_list에서 우선순위가 가장 높은 thread와 running thread를 비교하여 우선순위가 더 높은 thread가 선점할 수 있게 해줌
  
### 11.15 화

### Priority Scheduling (2) Semaphore, Lock, Condition Variable 구현 
- Semaphore, Lock, Condition Variable을 사용하여 Priority Scheduling 개선
  - PintOS는 FIFO 방식을 사용, sychronization 도구들을 기다릴 때 우선순위가 가장 높은 thread가 CPU를 점유하도록 구현 


### 11.16 수

### Priority Scheduling (3) Priority donation(Prority inversion problem) 구현 
- **문제점: priority가 높은 thread가 priority가 낮은 thread를 기다리는 현상**
- **문제 해결: Priority donation**
  - Multiple donation
    - thread가 두 개 이상의 lock 보유 시 각 lock을 획득하고자 하는 thread들에게 donation 발생가능
  - Nested donation
    - 여러 번의 단계적 donation이 일어나는 상황



## SW Jungle Week09 (2022.11.17 ~ 11.28)
## PROJECT 2: USER PROGRAMS

### WIL (Weekly I Learned)
### 11.17 목

### 11.18 금

### 11.19 토


### 11.20 일

### 11.21 월

### 11.22 화

### 11.23 수
