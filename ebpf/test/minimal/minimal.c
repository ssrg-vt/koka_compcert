#include <linux/types.h>
#include <asm/types.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
//#include <linux/bpf.h>
//#include <bpf/bpf_helpers.h>
#include </home/swarnp/research/compcert_bpf/CompCert/ebpf/lib/bpf.h>
#include </home/swarnp/research/compcert_bpf/CompCert/ebpf/lib/bpf/bpf_helpers.h>

/*
#undef bpf_printk
#define bpf_printk(fmt, ...)                           \
{                                                      \
        char ____fmt[] = fmt;                           \
        bpf_trace_printk(____fmt, sizeof(____fmt),      \
                         ##__VA_ARGS__);                \
}*/

#undef SEC
#define SEC(A) __attribute__ ((section(A),used))

char LICENSE[] SEC("license") = "Dual BSD/GPL";


SEC("tp/syscalls/sys_enter_write")
int handle_tp(void *ctx)
{   
	int64_t my_pid = 0;
	printf("Hello from minimal!");
	bpf_printk("Hello from minimal!");
	int64_t pid =  (int64_t)bpf_get_current_pid_tgid() >> (int64_t)32;
	__builtin_annot("pid is evaulated to 0"); // treated as observable events in the program execution
	//bpf_printk("The value of pid is %lld", pid);

	if (pid == my_pid) {
		bpf_printk("The value pf pid is 0"); }
	
	else { return 0; }
	
    printf("Printing from printf %lld", pid);
    bpf_printk("BPF triggered from PID" /*%lld", pid*/);
	//bpf_printk("BPF triggered from PID %lld", pid);

	return 0;
}

int main() {
	handle_tp(NULL);
    return 0;
}

// ./ccomp -D __bpf_helper_as_extern__ -S -o ebpf/test/minimal/minimal.s ebpf/test/minimal/minimal.c 
