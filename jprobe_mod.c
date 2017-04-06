#include <linux/sched.h>
#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/kprobes.h>
#include<linux/jiffies.h>
#include<linux/time.h>
#include <linux/preempt_mask.h>

#define debug 
 
struct rq;
struct softirq_action;

int sleeping = 1;
long long start_time_stamp = 0;
long unsigned int loop_count;
int enqueued = 0;
long long unsigned int 	hrtimer_interrupt_ts, 	hrtimer_interrupt_delay,hrtimer_interrupt_avg,
			try_to_wake_up_delay, 	try_to_wake_up_ts, 	try_to_wake_up_avg, 
			activate_task_ts, 	activate_task_delay, 	activate_task_avg,
			enqueue_task_ts, 	enqueue_task_delay, 	enqueue_task_avg,
			check_preempt_wakeup_ts,check_preempt_wakeup_delay,check_preempt_wakeup_avg,
			finish_task_switch_ts;
			

static int __sched jdo_nanosleep(struct hrtimer_sleeper *t, enum hrtimer_mode mode)
{
	if(strcmp(current->comm, "cyclictest") == 0){
		sleeping = 1;
		#ifdef debug
			printk(KERN_INFO "%llu : jdo_nanosleep, curr : %s\n", local_clock(), current->comm);
		#endif
	}

	jprobe_return();
	return 0;
}

void jhrtimer_interrupt(struct clock_event_device *dev)
{
	hrtimer_interrupt_ts = local_clock();
	jprobe_return();
}

static int jtry_to_wake_up(struct task_struct *p, unsigned int state, int wake_flags)
{
	if(strncmp(p->comm, "cyclictest",10) == 0){
		if (in_irq())
		{
			sleeping = 0;
			start_time_stamp = hrtimer_interrupt_ts;
			try_to_wake_up_ts = local_clock();
			hrtimer_interrupt_delay = try_to_wake_up_ts - hrtimer_interrupt_ts;
			hrtimer_interrupt_avg += hrtimer_interrupt_delay;
			#ifdef debug
				printk(KERN_INFO "%llu : hrtimer_interrupt -> jtry_to_wake_up\n", hrtimer_interrupt_delay);
			#endif
		}
	}
	jprobe_return();
	return 0;
}

void jactivate_task(struct rq * rqa, struct task_struct *p, int flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		if(sleeping == 0)
		{
			activate_task_ts = local_clock();
			try_to_wake_up_delay = activate_task_ts - try_to_wake_up_ts ;
			try_to_wake_up_avg += try_to_wake_up_delay;
			#ifdef debug
				printk(KERN_INFO "%llu : try_to_wake_up -> activate_task\n", try_to_wake_up_delay);
			#endif
		}	
	}

	jprobe_return();

}


static void jenqueue_task(struct rq *rq, struct task_struct *p, int flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){

		if(sleeping == 0)
		{
			enqueue_task_ts = local_clock();
			activate_task_delay = enqueue_task_ts - activate_task_ts ;
			activate_task_avg += activate_task_delay;
			enqueued++;
			#ifdef debug
				printk(KERN_INFO "%llu : activate_task -> enqueue_task \n", activate_task_delay);
			#endif
		}
	}
	jprobe_return();
}

/*
static void jcheck_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	if(strcmp(p->comm, "cyclictest") == 0){
		if (enqueued > 0){	
			check_preempt_wakeup_ts = local_clock();
			enqueue_task_delay = check_preempt_wakeup_ts - enqueue_task_ts;
			enqueue_task_avg += enqueue_task_delay;
			loop_count++;
			enqueued--;
			#ifdef debug
				printk(KERN_INFO "%llu :enqueue_task -> jcheck_preempt_wakeup\n", enqueue_task_delay);
			#endif
		}
	}
	jprobe_return();	
}
*/

static void jfinish_task_switch(struct rq *rq, struct task_struct *prev) 
		__releases(rq->lock)
{
	if(strcmp(current->comm, "cyclictest") == 0){
		if(sleeping == 0){
			finish_task_switch_ts = local_clock();
			enqueue_task_delay = finish_task_switch_ts - enqueue_task_ts;
			enqueue_task_avg += enqueue_task_delay;
			#ifdef debug			
				printk(KERN_INFO "%llu : enqueue -> jfinish_task_switch\n", enqueue_task_delay);
			#endif
			loop_count++;
		}
	}

	jprobe_return();

}	
/*

static void jrun_timer_softirq(struct softirq_action *h)
{
	if(sleeping == 0)
	{
		//run_timer_softirq_ts = local_clock(); 
	}
	jprobe_return();
}

static void jcall_timer_fn(struct timer_list *timer, void (*fn)(unsigned long),
                          unsigned long data)
{
	if(sleeping == 0)
	{
		//call_timer_fn_ts = local_clock(); 		
		//printk(KERN_INFO "%llu : call_timer_fn\n",call_timer_fn_ts);
	}
	jprobe_return();
} 

*/

struct jprobe jp0 = {
		.entry = jdo_nanosleep,
		.kp = {
		    .symbol_name = "do_nanosleep",
		},
	};

struct jprobe jp1 = {
		.entry = jhrtimer_interrupt,
		.kp = {
		    .symbol_name = "hrtimer_interrupt",
		},
	};


struct jprobe jp2 = {
		.entry = jtry_to_wake_up,
		.kp = {
		    .symbol_name = "try_to_wake_up",
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

/*
struct jprobe jp5 = {
		.entry = jcheck_preempt_wakeup,
		.kp = {
		    .symbol_name = "check_preempt_wakeup",
		},
	};
*/
struct jprobe jp5 = {
		.entry = jfinish_task_switch,
		.kp = {
		    .symbol_name = "finish_task_switch",
		},
	};


static void checkRegStatus(int ret, const char * name){
	if(ret < 0){
		printk(KERN_ALERT "%s register_jprobe failed, returned %d\n", name, ret);
	}
}

int __init jprobe_init(void)
{
	int ret = 0;
    
	printk(KERN_ALERT "module inserted\n ");

	ret = register_jprobe(&jp0);
	checkRegStatus(ret, jp0.kp.symbol_name);

	ret = register_jprobe(&jp1);
	checkRegStatus(ret, jp1.kp.symbol_name);

	ret = register_jprobe(&jp2);
	checkRegStatus(ret, jp2.kp.symbol_name);

	ret = register_jprobe(&jp3);
	checkRegStatus(ret, jp3.kp.symbol_name);

    	ret = register_jprobe(&jp4);
	checkRegStatus(ret, jp4.kp.symbol_name);

    	ret = register_jprobe(&jp5);
	checkRegStatus(ret, jp5.kp.symbol_name);


    return 0;
}
 
static void __exit jprobe_exit(void)
{
	unregister_jprobe(&jp0);
	unregister_jprobe(&jp1);
	unregister_jprobe(&jp2);
	unregister_jprobe(&jp3);
	unregister_jprobe(&jp4);
	unregister_jprobe(&jp5);
	printk(KERN_INFO "\nhrtimer_interrupt_avg %llu: \ntry_to_wake_up_avg: %llu \nactivate_task_avg : %llu \nenqueue_task_avg : %llu \n,loop_count : %lu\n", hrtimer_interrupt_avg / loop_count, try_to_wake_up_avg / loop_count, activate_task_avg / loop_count, enqueue_task_avg / loop_count, loop_count);

	
}
 
module_init(jprobe_init)
module_exit(jprobe_exit)

MODULE_AUTHOR("Vikram");
MODULE_DESCRIPTION("JPROBE MODULE");
MODULE_LICENSE("GPL");
