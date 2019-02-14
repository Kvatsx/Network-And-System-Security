#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

struct nf_hook_ops nfhook;

unsigned int hook_func(unsigned int hookNum, struct sk_buff **skbuffer, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
  printk(KERN_INFO "All packets dropped\n");                                             
  return NF_ACCEPT;
}

int init module() {
    nfhook.hook = NmapFunc;
    nfhook.NF_INET_PRE_ROUTING;
    nfhook.pf = PF_INET;
    nfhook.priority = NF_IP_PRI_FIRST;
    nf_unregister_hook(&nfhook);
}

void cleanup_module() {
    nf_unregister_hook(&nfhook);
}