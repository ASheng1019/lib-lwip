/* SPDX-License-Identifier: BSD-3-Clause */
#include <errno.h>
#include <lwip/ip6.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <uk/lwip-ipv6.h>
struct setup {
	sys_sem_t done;
	const char *name;
	ip6_addr_t address;
	struct netif *selected;
	unsigned flags;
	s8_t index;
	int has_address, initialized, result;
};
static void configure(void *arg)
{
	struct setup *s = arg;
	struct netif *nf = netif_find(s->name);
	s->result = -ENODEV;
	if (!nf || !netif_is_up(nf) || !(nf->flags & NETIF_FLAG_ETHARP) ||
	    (s->initialized && nf != s->selected))
		goto done;
	if (!s->initialized) {
#if LWIP_IPV6_MLD
		if (s->flags & UK_LWIP_IP6_RX_MULTICAST)
			netif_set_flags(nf, NETIF_FLAG_MLD6);
		if (!(nf->flags & NETIF_FLAG_MLD6)) {
			s->result = -EOPNOTSUPP;
			goto done;
		}
#endif
		if (ip6_addr_isany(netif_ip6_addr(nf, 0)))
			netif_create_ip6_linklocal_address(nf, 1);
#if LWIP_IPV6_AUTOCONFIG
		nf->ip6_autoconfig_enabled = !!(s->flags & UK_LWIP_IP6_SLAAC);
#else
		if (s->flags & UK_LWIP_IP6_SLAAC) {
			s->result = -EOPNOTSUPP;
			goto done;
		}
#endif
		if (s->has_address &&
		    netif_add_ip6_address(nf, &s->address, &s->index) !=
			ERR_OK) {
			s->result = -ENOMEM;
			goto done;
		}
		s->selected = nf;
		s->initialized = 1;
	}
	s->result = 0;
	if (netif_ip6_addr_state(nf, 0) == IP6_ADDR_DUPLICATED ||
	    netif_ip6_addr_state(nf, s->index) == IP6_ADDR_DUPLICATED)
		s->result = -EADDRINUSE;
	else if (!ip6_addr_isvalid(netif_ip6_addr_state(nf, 0)) ||
		 !ip6_addr_isvalid(netif_ip6_addr_state(nf, s->index)))
		s->result = -EAGAIN;
done:
	sys_sem_signal(&s->done);
}
int uk_lwip_ip6_configure(const char *name, const char *address, unsigned flags,
			  unsigned timeout_ms)
{
	struct setup s = {.name = name, .flags = flags};
	u32_t start;
	int result;
	if (!name || !*name || !timeout_ms || timeout_ms > 60000 ||
	    flags & ~(UK_LWIP_IP6_SLAAC | UK_LWIP_IP6_RX_MULTICAST))
		return -EINVAL;
	if (address) {
		if (!ip6addr_aton(address, &s.address) ||
		    ip6_addr_isany(&s.address) ||
		    ip6_addr_ismulticast(&s.address))
			return -EINVAL;
		s.has_address = 1;
	}
	if (sys_sem_new(&s.done, 0) != ERR_OK)
		return -ENOMEM;
	start = sys_now();
	for (;;) {
		if (tcpip_callback_with_block(configure, &s, 1) != ERR_OK) {
			result = -EIO;
			break;
		}
		/* Cannot abandon a queued callback with a stack-owned argument.
		 */
		sys_arch_sem_wait(&s.done, 0);
		result = s.result;
		if (result != -EAGAIN)
			break;
		u32_t elapsed = sys_now() - start;
		if (elapsed >= timeout_ms) {
			result = -ETIMEDOUT;
			break;
		}
		sys_msleep(timeout_ms - elapsed < 100 ? timeout_ms - elapsed
						      : 100);
	}
	sys_sem_free(&s.done);
	return result;
}
