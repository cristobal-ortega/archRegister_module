/**
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>

//64 bits register
#define REG_SIZE 64

#define PROC_FILE "HID_reg"

#define INIT_VALUE "Hello World"

#define mov() (\
    {unsigned long int rval; \
    asm volatile( \
        "mov %%rbx %%rax\n\t" \
        "mov %%eax, %0\n\t" \
        : "=m" (rval) \
        : \
        : "%rax");\
        rval;})

// Holds the current state of the register
char internalRegister[REG_SIZE];



/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char *buf, size_t count, loff_t *pos);
// ssize_t proc_write(struct file *file, const char *buffer, unsigned long count,void *data);



static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
	.write = proc_write,
};

// Currently reading cr0 for debugging purposes
uint64_t readRegister(void) {
	uint64_t reg;
	__asm__ __volatile__ (
		"mov %%cr0, %%rax\n\t"
		"mov %%eax, %0\n\t"
		: "=m" (reg)
		: /* no input */
		: "%rax"
	);
	return reg;
}

uint64_t string_to_int(uint64_t base, char *string, size_t count) {
	uint64_t value = 0;
	uint64_t baseCounter = 1;
	for(size_t i = 0; i < strlen(string); ++i) {
		value = value + baseCounter*(int)(string[i] - '0');
		baseCounter = baseCounter * base;
	}
	return value;

}

/* This function is called when the module is loaded. */
int proc_init(void)
{

        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_FILE, 0, NULL, &proc_ops);

	// Initialization of our internal buffer
	// Should read architectural register, and initialize to that.
	sprintf(internalRegister, INIT_VALUE);


        printk(KERN_INFO "/proc/%s created\n", PROC_FILE);
        printk(KERN_INFO "%llu\n", readRegister());

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/hello entry
       remove_proc_entry(PROC_FILE, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_FILE);
}

/**
 * This function is called each time the /proc/hello is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int len = 0;

	if (*pos > 0 || count < REG_SIZE) {
		return 0;
	}

	len = sprintf(usr_buf, internalRegister);

	printk(KERN_ALERT "internalRegister: %s with len: %i\n", internalRegister, len);

        // copies the contents of buffer to userspace usr_buf
	if (copy_to_user(usr_buf, internalRegister, len))
		return -EFAULT;

	*pos = len;
        return len;
}

static ssize_t proc_write(struct file *file, const char *buf, size_t count, loff_t *pos)
{
	printk(KERN_ALERT"Write function\n");
	char tmpBuf[REG_SIZE] = {0};

	// First time we call write
	if (*pos > 0 || count > REG_SIZE) {
		return -EFAULT;
	}
	if (copy_from_user(tmpBuf,buf,count))
		return -EFAULT;

	// Ensure internalRegister is cleared with temporary buffer initialized to 0s
	sprintf(internalRegister, tmpBuf);

	*pos = count;
	printk(KERN_ALERT "internalRegister: %s\n", internalRegister);
	printk(KERN_ALERT "Number would be: %llu\n", string_to_int(2, internalRegister, count));

	return count;
}



/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("BSD");
MODULE_AUTHOR("Cristobal Ortega");
MODULE_DESCRIPTION("Expose architectural register to the OS through file in /proc/ fs");
