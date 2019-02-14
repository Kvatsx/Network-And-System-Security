// Kaustav Vats (2016048)
// Ref:- https://stackoverflow.com/questions/39426783/netfilter-kernel-module-to-intercept-packets-and-log-them
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

static struct nf_hook_ops nfhook;

unsigned int NmapFunc(unsigned int hookNum, struct sk_buff *skbuffer) {
  printk("All packets accepted!\n");                                             
  return NF_ACCEPT;
}

int netfilter_module_init(void) {
	nfhook.hook = (nf_hookfn *) NmapFunc;
	nfhook.hooknum = NF_INET_PRE_ROUTING;
	nfhook.pf = PF_INET;
	nfhook.priority = NF_IP_PRI_FIRST;
	nf_register_net_hook(&init_net, &nfhook);
	printk("Registered Device.\n");
	return 0;
}

void netfilter_module_exit(void) {
	nf_unregister_net_hook(&init_net, &nfhook);
	printk("Unregistered Device.\n");
}

module_init(netfilter_module_init);
module_exit(netfilter_module_exit);