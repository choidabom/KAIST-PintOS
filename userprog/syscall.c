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
#include "threads/synch.h"

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
void seek_handler(int fd, unsigned position);
unsigned tell_handler(int fd);
void close_handler(int fd);
int process_add_file(struct file *f); /* 파일 객체에 대한 파일 디스크립터 생성 */
struct lock filesys_lock;			  /* read(), write()에서 파일 접근 전 lock을 획득하도록 구현*/

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
	lock_init(&filesys_lock); /* filesys_lock 초기화 코드 */
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
		f->R.rax = create_handler(a1, a2);
		break;
	case SYS_REMOVE:
		f->R.rax = remove_handler(a1);
		break;
	case SYS_OPEN:
		f->R.rax = open_handler(a1);
		break;
	case SYS_FILESIZE:
		f->R.rax = filesize_handler(a1);
		break;
	case SYS_READ:
		f->R.rax = read_handler(a1, a2, a3);
		break;
	case SYS_WRITE:
		printf("%s", a2);
		break;
	case SYS_SEEK:
		seek_handler(a1, a2);
		break;
	case SYS_TELL:
		f->R.rax = tell_handler(a1);
		break;
	case SYS_CLOSE:
		close_handler(a1);
		break;
	default:
		break;
	}
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
	check_address(file);
	if (*file == NULL)
		return -1;

	struct file *open_file = filesys_open(file);
	if (open_file == NULL)
		return -1;
	int fd = process_add_file(open_file);

	return fd;
}

/* fd로 열려있는 파일의 사이즈를 리턴해주는 시스템 콜 */
int filesize_handler(int fd)
{
	struct thread *curr = thread_current();
	struct list_elem *start;

	for (start = list_begin(&curr->fd_list); start != list_end(&curr->fd_list); start = list_next(start))
	{
		struct file_fd *now_fd = list_entry(start, struct file_fd, fd_elem);
		if (now_fd->fd == fd)
		{
			return file_length(now_fd->file);
		}
	}
}

int read_handler(int fd, void *buffer, unsigned size)
{
	struct thread *curr = thread_current();
	struct list_elem *start;
	lock_acquire(&filesys_lock);
	if (fd == 0)
	{
		return input_getc();
	}
	else if (fd < 0 || fd == NULL)
	{
		exit_handler(-1);
	}
	// bad-fd는 page-fault를 일으키기 때문에 page-fault를 처리하는 함수에서 확인

	for (start = list_begin(&curr->fd_list); start != list_end(&curr->fd_list); start = list_next(start))
	{
		struct file_fd *read_fd = list_entry(start, struct file_fd, fd_elem);
		if (read_fd->fd == fd)
		{
			check_address(read_fd->file);
			off_t buff_size = file_read(read_fd->file, buffer, size);
			lock_release(&filesys_lock);
			return buff_size;
		}
		exit_handler(-1);
	}
}

int write_handler(int fd, const void *buffer, unsigned size)
{
	struct thread *curr = thread_current();
	struct list_elem *start;

	for (start = list_begin(&curr->fd_list); start != list_end(&curr->fd_list); start = list_next(start))
	{
		struct file_fd *write_fd = list_entry(start, struct file_fd, fd_elem);
		if (write_fd->fd == fd)
		{
			file_write(write_fd->fd, buffer, strlen(buffer));
		}
	}
}

void seek_handler(int fd, unsigned position)
{
}

/* fd에서 읽히거나 써질 다음 바이트의 위치를 반환 */
unsigned
tell_handler(int fd)
{
	struct thread *curr = thread_current();
	struct list_elem *start;

	for (start = list_begin(&curr->fd_list); start != list_end(&curr->fd_list); start = list_next(start))
	{
		struct file_fd *tell_fd = list_entry(start, struct file_fd, fd_elem);
		if (tell_fd->fd == fd)
		{
			return file_tell(tell_fd->file);
		}
	}
}

/* fd를 닫는 시스템 콜 */
void close_handler(int fd)
{
	struct thread *curr = thread_current();
	struct list_elem *start;
	// start->fd_list = curr->fd_list;
	for (start = list_begin(&curr->fd_list); start != list_end(&curr->fd_list); start = list_next(start))
	{
		struct file_fd *close_fd = list_entry(start, struct file_fd, fd_elem);
		if (close_fd->fd == fd)
		{
			file_close(close_fd->file);
			close_fd->fd = NULL;
		}
	}
	return;
}

void check_address(void *addr)
{
	/* 포인터가 가리키는 주소가 유저영역의 주소인지 확인 */
	// if (&addr < 0x8048000 || &addr > 0xc0000000)

	// 유저 가상 공간에 존재하지만 페이지에 할당되지 않은 경우도 존재함
	if (is_user_vaddr(addr) && pml4_get_page(thread_current()->pml4, addr) && addr != NULL)
		return;
	else
		exit_handler(-1);
}

// int process_add_file(struct file *f)
// {
// 	struct thread *curr = thread_current();
// 	struct file_fd *new_fd = malloc(sizeof(struct file_fd));

// 	// curr에 있는 fd_list의 fd를 확인하기 위한 작업
// 	// list_begin 했을 경우 fd = 0 출력되고, list_back 했을 경우 fd = 1 출력됨
// 	struct list_elem *curr_elem = list_back(&curr->fd_list);
// 	struct file_fd *curr_fd = list_entry(curr_elem, struct file_fd, fd_elem);
// 	printf("%d\n", curr_fd->fd);

// 	curr_fd->fd += 1;
// 	new_fd->fd = curr_fd->fd; => 이 부분에 확신이 없음
// 	new_fd->file = f;
// 	list_push_back(&curr->fd_list, &new_fd->fd_elem);

// 	return new_fd->fd;
// }

int process_add_file(struct file *f)
{
	struct thread *curr = thread_current();
	struct file_fd *new_fd = malloc(sizeof(struct file_fd));

	// curr에 있는 fd_list의 fd를 확인하기 위한 작업
	// list_begin 했을 경우 fd = 0 출력되고, list_back 했을 경우 fd = 1 출력됨
	// struct list_elem *check = list_begin(&curr->fd_list);
	// struct file_fd *check_fd = list_entry(check, struct file_fd, fd_elem);
	// printf("%d\n", check_fd->fd);
	// 악 대박 fd 나옴 ~!~!

	curr->fd_count += 1;
	new_fd->fd = curr->fd_count;
	new_fd->file = f;
	list_push_back(&curr->fd_list, &new_fd->fd_elem);

	return new_fd->fd;
}