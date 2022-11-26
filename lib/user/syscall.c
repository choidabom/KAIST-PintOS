#include <syscall.h>
#include <stdint.h>
#include "../syscall-nr.h"

/* 시스템 콜 핸들러 syscall_handler()가 제어권을 얻으면 시스템 콜 번호는 rax에 있고,
인자는 %rdi, %rsi, %rdx, %r8, %r9 순서로 전달된다. */
__attribute__((always_inline)) static __inline int64_t syscall(uint64_t num_, uint64_t a1_, uint64_t a2_,
															   uint64_t a3_, uint64_t a4_, uint64_t a5_, uint64_t a6_)
{
	int64_t ret;
	register uint64_t *num asm("rax") = (uint64_t *)num_;
	register uint64_t *a1 asm("rdi") = (uint64_t *)a1_;
	register uint64_t *a2 asm("rsi") = (uint64_t *)a2_;
	register uint64_t *a3 asm("rdx") = (uint64_t *)a3_;
	register uint64_t *a4 asm("r10") = (uint64_t *)a4_;
	register uint64_t *a5 asm("r8") = (uint64_t *)a5_;
	register uint64_t *a6 asm("r9") = (uint64_t *)a6_;

	__asm __volatile(
		"mov %1, %%rax\n"
		"mov %2, %%rdi\n"
		"mov %3, %%rsi\n"
		"mov %4, %%rdx\n"
		"mov %5, %%r10\n"
		"mov %6, %%r8\n"
		"mov %7, %%r9\n"
		"syscall\n"
		: "=a"(ret)
		: "g"(num), "g"(a1), "g"(a2), "g"(a3), "g"(a4), "g"(a5), "g"(a6)
		: "cc", "memory");
	return ret;
	// syscall 인스트럭션이 실행되면 syscall-entry.S로 진입
}

/* Invokes syscall NUMBER, passing no arguments, and returns the
   return value as an `int'. */
#define syscall0(NUMBER) ( \
	syscall(((uint64_t)NUMBER), 0, 0, 0, 0, 0, 0))

/* Invokes syscall NUMBER, passing argument ARG0, and returns the
   return value as an `int'. */
#define syscall1(NUMBER, ARG0) ( \
	syscall(((uint64_t)NUMBER),  \
			((uint64_t)ARG0), 0, 0, 0, 0, 0))
/* Invokes syscall NUMBER, passing arguments ARG0 and ARG1, and
   returns the return value as an `int'. */
#define syscall2(NUMBER, ARG0, ARG1) ( \
	syscall(((uint64_t)NUMBER),        \
			((uint64_t)ARG0),          \
			((uint64_t)ARG1),          \
			0, 0, 0, 0))

#define syscall3(NUMBER, ARG0, ARG1, ARG2) ( \
	syscall(((uint64_t)NUMBER),              \
			((uint64_t)ARG0),                \
			((uint64_t)ARG1),                \
			((uint64_t)ARG2), 0, 0, 0))

#define syscall4(NUMBER, ARG0, ARG1, ARG2, ARG3) ( \
	syscall(((uint64_t *)NUMBER),                  \
			((uint64_t)ARG0),                      \
			((uint64_t)ARG1),                      \
			((uint64_t)ARG2),                      \
			((uint64_t)ARG3), 0, 0))

#define syscall5(NUMBER, ARG0, ARG1, ARG2, ARG3, ARG4) ( \
	syscall(((uint64_t)NUMBER),                          \
			((uint64_t)ARG0),                            \
			((uint64_t)ARG1),                            \
			((uint64_t)ARG2),                            \
			((uint64_t)ARG3),                            \
			((uint64_t)ARG4),                            \
			0))

/* 0번콜: power-of와 같은 기능으로, pintos를 종료시키는 시스템 콜 */
void halt(void)
{
	syscall0(SYS_HALT);
	NOT_REACHED();
}

/* 1번콜: 현재 프로세스를 종료시키는 시스템 콜 (Git book: 현재 동작 중인 유저 프로그램을 종료, 커널에 상태를 리턴하면서 종료)*/
void exit(int status)
{
	syscall1(SYS_EXIT, status);
	NOT_REACHED();
}

/* 2번콜: thread_name이라는 이름을 가진 (현재 프로세스의 복제본인) 새 프로세스를 만들어주는 시스템 콜 */
/* 자식 프로세스의 pid를 반환해야 함. 유효한 pid가 아닌 경우 0을 반환해야함. */
pid_t fork(const char *thread_name)
{
	return (pid_t)syscall1(SYS_FORK, thread_name);
}

/* 3번콜: 현재 프로세스를 실행가능한 프로세스로 변경해주는 시스템 콜 */
int exec(const char *file)
{
	return (pid_t)syscall1(SYS_EXEC, file);
}

/* 4번콜: 자식의 종료 상태를 가져오는 시스템 콜 */
int wait(pid_t pid)
{
	return syscall1(SYS_WAIT, pid);
}

/* 5번콜: 파일을 생성하는 시스템 콜 */
bool create(const char *file, unsigned initial_size)
{
	return syscall2(SYS_CREATE, file, initial_size);
}

/* 6번콜: 파일을 삭제하는 시스템 콜 */
bool remove(const char *file)
{
	return syscall1(SYS_REMOVE, file);
}

/* 7번콜: 파일을 오픈해주는 시스템 콜 */
int open(const char *file)
{
	return syscall1(SYS_OPEN, file);
}

/* 8번콜: fd로 열려있는 파일의 사이즈를 리턴해주는 시스템 콜 */
int filesize(int fd)
{
	return syscall1(SYS_FILESIZE, fd);
}

/* 9번콜: read해주는 시스템 콜 (fd를 통해 열린 파일의 내용) => (버퍼)*/
int read(int fd, void *buffer, unsigned size)
{
	return syscall3(SYS_READ, fd, buffer, size);
}

/* 10번콜: write 해주는 시스템 콜 (버퍼) => (fd를 통해 열린 파일의 내용)*/
int write(int fd, const void *buffer, unsigned size)
{
	return syscall3(SYS_WRITE, fd, buffer, size);
}

/* 11번콜: fd로 열려있는 파일의 (읽고 쓸 위치를 알려주는)포인터의 위치를 변경해주는 시스템 콜 */
void seek(int fd, unsigned position)
{
	syscall2(SYS_SEEK, fd, position);
}

/* 12번콜: fd에서 읽히거나 써질 다음 바이트의 위치를 반환 */
unsigned
tell(int fd)
{
	return syscall1(SYS_TELL, fd);
}

/* 13번콜: fd를 닫는 시스템 콜 */
void close(int fd)
{
	syscall1(SYS_CLOSE, fd);
}

int dup2(int oldfd, int newfd)
{
	return syscall2(SYS_DUP2, oldfd, newfd);
}

void *
mmap(void *addr, size_t length, int writable, int fd, off_t offset)
{
	return (void *)syscall5(SYS_MMAP, addr, length, writable, fd, offset);
}

void munmap(void *addr)
{
	syscall1(SYS_MUNMAP, addr);
}

bool chdir(const char *dir)
{
	return syscall1(SYS_CHDIR, dir);
}

bool mkdir(const char *dir)
{
	return syscall1(SYS_MKDIR, dir);
}

bool readdir(int fd, char name[READDIR_MAX_LEN + 1])
{
	return syscall2(SYS_READDIR, fd, name);
}

bool isdir(int fd)
{
	return syscall1(SYS_ISDIR, fd);
}

int inumber(int fd)
{
	return syscall1(SYS_INUMBER, fd);
}

int symlink(const char *target, const char *linkpath)
{
	return syscall2(SYS_SYMLINK, target, linkpath);
}

int mount(const char *path, int chan_no, int dev_no)
{
	return syscall3(SYS_MOUNT, path, chan_no, dev_no);
}

int umount(const char *path)
{
	return syscall1(SYS_UMOUNT, path);
}
