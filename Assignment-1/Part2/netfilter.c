// Kaustav Vats (2016048)
// Ref:- https://stackoverflow.com/questions/39426783/netfilter-kernel-module-to-intercept-packets-and-log-them
// Ref:- https://nmap.org/book/scan-methods-null-fin-xmas-scan.html
// Ref:- https://github.com/clan/kernel/blob/master/net/pkt_chk.c
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

static struct nf_hook_ops nfhook;

unsigned int NmapFunc(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
	struct iphdr * ip_header;
	struct tcphdr * tcph;
//	printk("ok1");
	ip_header = (struct iphdr *)skb_network_header(skb);
//	printk("ok2 %d lol\n", ip_header->protocol);
	if (ip_header->protocol == IPPROTO_TCP) {
//		printk("ok3");
		tcph = (struct tcphdr *)skb_transport_header(skb);
		if (tcph->source) {
//			printk("ok4");
			// TCP XMAS SCAN
			// FIN, PSH AND URG FLAG SET
			if (tcph->fin && tcph->psh && !tcph->ack && !tcph->syn && tcph->urg && !tcph->rst) {
				printk("TCP: XMAS Scan Detected!\n");
			}
			// TCP FIN SCAN
			// ONLY FIN FLAG SET 
			else if ( tcph->fin && !tcph->psh && !tcph->ack && !tcph->syn && !tcph->urg && !tcph->rst ) {
				printk("TCP: FIN Scan Detected!\n");
			}
			// TCP SYN SCAN
			// ONLY SIN FLAG SET
			else if ( !tcph->fin && !tcph->psh && !tcph->ack && tcph->syn && !tcph->urg && !tcph->rst ) {
				printk("TCP: SYN Scan Detected!\n");
			}
			// TCP NULL SCAN
			// ALL BITS FLAG SET 
			else if ( !tcph->fin && !tcph->psh && !tcph->ack && !tcph->syn && !tcph->urg && !tcph->rst ) {
				printk("TCP: NULL Scan Detected!\n");
			}
		}	
	}

	return NF_ACCEPT;
}

static int __init netfilter_module_init(void) {
	nfhook.hook = (nf_hookfn *) NmapFunc;
	nfhook.hooknum = NF_INET_PRE_ROUTING;
	nfhook.pf = PF_INET;
	nfhook.priority = NF_IP_PRI_FIRST;
	nf_register_net_hook(&init_net, &nfhook);
	printk("Registered Device.\n");
	return 0;
}

static void __exit netfilter_module_exit(void) {
	nf_unregister_net_hook(&init_net, &nfhook);
	printk("Unregistered Device.\n");
}

module_init(netfilter_module_init);
module_exit(netfilter_module_exit);
