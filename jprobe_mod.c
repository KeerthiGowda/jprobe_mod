#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/kprobes.h>
#include<linux/jiffies.h>

extern unsigned long volatile __jiffy_data jiffies;
 
//static unsigned int counter = 0;


static int jtry_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :jtry_to_wake_up, task : %s\n", jiffies, p->comm);
	}
	jprobe_return();
	return 0;
}

static void jttwu_queue(struct task_struct *p, int cpu)
{
     if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :ttwu_queue : %s\n", jiffies, p->comm);
	}
	jprobe_return();
}

static void jttwu_queue_remote(struct task_struct *p, int cpu)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :ttwu_queue_remote : %s\n", jiffies, p->comm);
	}
	jprobe_return();
}

static void jttwu_do_activate(struct rq *rq, struct task_struct *p, int wake_flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :ttwu_do_activate : %s\n", jiffies, p->comm);
	}
	jprobe_return();
}

void jwq_worker_waking_up(struct task_struct *task, int cpu)
{
	if(strcmp(task->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :jwq_worker_waking_up : %s\n", jiffies, task->comm);
	}
	jprobe_return();
}


void jactivate_task(struct rq *rq, struct task_struct *p, int flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : activate_task, curr : %s\n", jiffies, p->comm);
	}

	jprobe_return();

}

static void jenqueue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : enqueue, curr : %s\n", jiffies, p->comm);
	}

	jprobe_return();
}

static void jcheck_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :jcheck_preempt_wakeup, task Name %s\n", jiffies, p->comm);
	}
	jprobe_return();	
}

void jresched_task(struct task_struct *p)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu :jresched_task, task Name %s\n", jiffies, p->comm);
	}
	jprobe_return();
}

void jupdate_process_times(int user_tick)
{
	if(strcmp(current->comm, "cyclictest") == 0){
//		printk(KERN_ALERT "%lu : update_process_times %d \n ", jiffies, user_tick);
	}
	jprobe_return();
}

void jupdate_wall_time(void)
{
	//printk(KERN_ALERT "%lu : update_wall_time\n ", jiffies);
	jprobe_return();
}

long jhrtimer_nanosleep(struct timespec *rqtp, struct timespec __user *rmtp,
                       const enum hrtimer_mode mode, const clockid_t clockid)
{
	if(strcmp(current->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : hrtimer_nanosleep, task Name %s\n", jiffies, current->comm);
	}
	jprobe_return();
	return 0;
}


static void __sched j__schedule(void)
{
	
	if(strcmp(current->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : __schedule , curr : %s\n", jiffies, current->comm);
	}	
	jprobe_return();
}


static void jfinish_task_switch(struct rq *rq, struct task_struct *prev)
        __releases(rq->lock)
{
	if(strcmp(prev->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : jfinish_task_switch, prev : %s , curr : %s\n", jiffies, prev->comm, current->comm);
	}


	if(strcmp(current->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : jfinish_task_switch, prev : %s , curr : %s\n", jiffies, prev->comm, current->comm);
	}

	jprobe_return();

}	

static int __sched jdo_nanosleep(struct hrtimer_sleeper *t, enum hrtimer_mode mode)
{
	if(strcmp(current->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : jdo_nanosleep, curr : %s\n", jiffies, current->comm);
	}

	jprobe_return();
	return 0;
}

void jhrtimer_interrupt(struct clock_event_device *dev)
{
//	printk(KERN_INFO "%lu : jhrtimer_interrupt\n", jiffies);
	jprobe_return();
}



void jdeactivate_task(struct rq *rq, struct task_struct *p, int flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		printk(KERN_INFO "%lu : deactivate_task, curr : %s\n", jiffies, p->comm);
	}

	jprobe_return();
} 


/*
// crashes
void jdo_irq(int vec, struct pt_regs *fp)
{
	printk(KERN_INFO "%lu : do_irq\n", jiffies);
	jprobe_return();
}
*/
 

struct jprobe jp0 = {
		.entry = jtry_to_wake_up,
		.kp = {
		    .symbol_name = "try_to_wake_up",
		},
	};
 
struct jprobe jp0_1 = {
		.entry = jttwu_queue,
		.kp = {
		    .symbol_name = "ttwu_queue",
		},
	};

struct jprobe jp0_2 = {
		.entry = jttwu_queue_remote,
		.kp = {
		    .symbol_name = "ttwu_queue_remote",
		},
	};

struct jprobe jp1 = {
		.entry = jttwu_do_activate,
		.kp = {
		    .symbol_name = "ttwu_do_activate",
		},
	};

struct jprobe jp2 = {
		.entry = jwq_worker_waking_up,
		.kp = {
		    .symbol_name = "wq_worker_waking_up",
		},
	};

struct jprobe jp3 = {
		.entry = jactivate_task,
		.kp = {
		    .symbol_name = "activate_task",
		},
	};

struct jprobe jp4 = {
		.entry = jenqueue_task,
		.kp = {
		    .symbol_name = "enqueue_task",
		},
	};


struct jprobe jp5 = {
		.entry = jcheck_preempt_wakeup,
		.kp = {
		    .symbol_name = "check_preempt_wakeup",
		},
	};

struct jprobe jp6 = {
		.entry = jresched_task,
		.kp = {
		    .symbol_name = "resched_task",
		},
	};

struct jprobe jp7 = {
		.entry = jupdate_process_times,
		.kp = {
		    .symbol_name = "update_process_times",
		},
	};

struct jprobe jp8 = {
		.entry = jhrtimer_nanosleep,
		.kp = {
		    .symbol_name = "hrtimer_nanosleep",
		},
	};

struct jprobe jp9 = {
		.entry = j__schedule,
		.kp = {
		    .symbol_name = "__schedule",
		},
	};


struct jprobe jp10 = {
		.entry = jfinish_task_switch,
		.kp = {
		    .symbol_name = "finish_task_switch",
		},
	};


struct jprobe jp11 = {
		.entry = jdo_nanosleep,
		.kp = {
		    .symbol_name = "do_nanosleep",
		},
	};

struct jprobe jp12 = {
		.entry = jhrtimer_interrupt,
		.kp = {
		    .symbol_name = "hrtimer_interrupt",
		},
	};

struct jprobe jp13 = {
		.entry = jdeactivate_task,
		.kp = {
		    .symbol_name = "deactivate_task",
		},
	};

int __init jprobe_init(void)
{
	int ret = 0;
    
	printk(KERN_ALERT "module inserted\n ");

	ret = register_jprobe(&jp0);
	ret = register_jprobe(&jp0_1);
	ret = register_jprobe(&jp0_2);
	ret = register_jprobe(&jp1);
	ret = register_jprobe(&jp2);
	ret = register_jprobe(&jp3);
    	ret = register_jprobe(&jp4);
    	ret = register_jprobe(&jp5);
    	ret = register_jprobe(&jp6);
    	ret = register_jprobe(&jp7);
    	ret = register_jprobe(&jp8);
    	ret = register_jprobe(&jp9);
    	ret = register_jprobe(&jp10);
	ret = register_jprobe(&jp11);
    	ret = register_jprobe(&jp12);
    	ret = register_jprobe(&jp13);
    	
	if(ret < 0){
		printk(KERN_ALERT "register_jprobe failed, returned %d\n", ret);
		return -1;
	}
    return 0;
}
 
static void __exit jprobe_exit(void)
{
	unregister_jprobe(&jp0);
  unregister_jprobe(&jp0_1);
  unregister_jprobe(&jp0_2);
	unregister_jprobe(&jp1);
	unregister_jprobe(&jp2);
	unregister_jprobe(&jp3);
	unregister_jprobe(&jp4);
	unregister_jprobe(&jp5);
	unregister_jprobe(&jp6);
	unregister_jprobe(&jp7);
	unregister_jprobe(&jp8);
	unregister_jprobe(&jp9);
	unregister_jprobe(&jp10);
	unregister_jprobe(&jp11);
	unregister_jprobe(&jp12);
	unregister_jprobe(&jp13);

//	printk(KERN_ALERT "jprobe at %p unregistered\n", jp.kp.addr);
//	printk(KERN_ALERT "jprobe at %p unregistered\n", jp1.kp.addr);
}
 
module_init(jprobe_init)
module_exit(jprobe_exit)

MODULE_AUTHOR("Vikram");
MODULE_DESCRIPTION("JPROBE MODULE");
MODULE_LICENSE("GPL");
