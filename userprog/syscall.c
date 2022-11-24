#include "userprog/syscall.h"
#include <stdio.h>
#include <stdbool.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

void syscall_entry(void);
void syscall_handler(struct intr_frame *);
void check_address(void *addr);
void exit_handler(int status);
bool create_handler(const char *file, unsigned initial_size);
bool remove_handler(const char *file);
int open_handler(const char *file);
int filesize_handler(int fd);
int read_handler(int fd, void *buffer, unsigned size);
int write_handler(int fd, const void *buffer, unsigned size);
void seek_handelr(int fd, unsigned position);
unsigned
tell_handler(int fd);
void close_handler(int fd);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081			/* Segment selector msr */
#define MSR_LSTAR 0xc0000082		/* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void syscall_init(void)
{
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48 |
							((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t)syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			  FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void syscall_handler(struct intr_frame *f UNUSED)
{
	// TODO: Your implementation goes here.
	int syscall_num = f->R.rax;
	uint64_t a1 = f->R.rdi;
	uint64_t a2 = f->R.rsi;
	uint64_t a3 = f->R.rdx;
	uint64_t a4 = f->R.r10;
	uint64_t a5 = f->R.r8;
	uint64_t a6 = f->R.r9;

	// printf("syscall_num 확인 !!!!!!!!!!!%d\n", syscall_num);
	switch (syscall_num)
	{
	case SYS_HALT:
		halt_handler();
	case SYS_EXIT:
		exit_handler(a1);
		break;
	case SYS_FORK:

		break;
	case SYS_EXEC:

		break;
	case SYS_WAIT:

		break;
	case SYS_CREATE:
		f->R.rax = create_handler(f->R.rdi, f->R.rsi);
		return;
		break;
	case SYS_REMOVE:
		f->R.rax = remove_handler(f->R.rdi);
		break;
	case SYS_OPEN:
		f->R.rax = open_handler(f->R.rdi);
		break;
	case SYS_FILESIZE:
		f->R.rax = filesize_handler(f->R.rdi);
		break;
	case SYS_READ:

		break;
	case SYS_WRITE:
		printf("%s", f->R.rsi);
		break;
	case SYS_SEEK:

		break;
	case SYS_TELL:

		break;
	case SYS_CLOSE:

		break;
	default:
		break;
	}
	// printf("반환값 넣을자리 ?\n");
}

void halt_handler()
{
	power_off();
}

/* 현재 프로세스를 종료시키는 시스템 콜 */
/* staus: 프로그램이 정상적으로 종료됐는지 확인, 정상적 종료 시 status = 0*/
void exit_handler(int status)
{
	struct thread *cur = thread_current();
	cur->exit_status = status;
	thread_exit();
}

/* 파일을 생성하는 시스템 콜 */
bool create_handler(const char *file, unsigned initial_size)
{
	check_address(file);
	// printf("file name = %s\n",file);
	// printf("size = %d\n",initial_size);
	if (file == NULL)
	{
		exit_handler(-1);
	}
	return filesys_create(file, initial_size);
}

/* 파일을 삭제하는 시스템 콜 */
/* file: 제거할 파일의 이름 및 경로 정보 */
bool remove_handler(const char *file)
{
	check_address(file);
	return filesys_remove(file);
}

int open_handler(const char *file)
{
	if (*file == NULL)
		return -1;
	check_address(file);

	struct file *open_file = filesys_open(file);
	if (open_file == NULL)
		return -1;
	int fd = open_file->inode;
	return fd;
	// 프로세스가 fd들을 관리할 수 있도록 배열로 만들어줘야한다...~
}

/* fd로 열려있는 파일의 사이즈를 리턴해주는 시스템 콜 */
int filesize_handler(int fd)
{
	return inode_length(fd);
}

int read_handler(int fd, void *buffer, unsigned size)
{
}

int write_handler(int fd, const void *buffer, unsigned size)
{
}

void seek_handelr(int fd, unsigned position)
{
}

/* fd에서 읽히거나 써질 다음 바이트의 위치를 반환 */
unsigned
tell_handler(int fd)
{
}

/* fd를 닫는 시스템 콜 */
void close_handler(int fd)
{
}

void check_address(void *addr)
{
	/* 포인터가 가리키는 주소가 유저영역의 주소인지 확인 */
	// if (&addr < 0x8048000 || &addr > 0xc0000000)

	// 유저 가상 공간에 존재하지만 페이지에 할당되지 않은 경우도 존재함
	if (is_user_vaddr(addr) && pml4_get_page(thread_current()->pml4, addr) && addr != NULL)
	{
		return;
	}
	else
		exit_handler(-1);
}