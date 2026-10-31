/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef UK_LWIP_IPV6_H
#define UK_LWIP_IPV6_H
#ifdef __cplusplus
extern "C" {
#endif
#define UK_LWIP_IP6_SLAAC 1U
/* Caller asserts this named interface receives IPv6 multicast without a
 * hardware filter callback. Do not set this on an unqualified transport.
 */
#define UK_LWIP_IP6_RX_MULTICAST 2U
/* Threaded lwIP only. Call from an application thread, never TCP/IP thread.
 * Configure one explicitly named, up Ethernet netif; optional unicast address.
 * Wait for link-local/static DAD, not acquisition of a SLAAC global address.
 * timeout_ms (1..60000) bounds DAD polling; queued callbacks always finish
 * before return. Returns 0 or negative errno (ENODEV, EADDRINUSE, ETIMEDOUT,
 * EOPNOTSUPP, EINVAL, ENOMEM, EIO). Configuration is not rolled back on
 * failure.
 */
int uk_lwip_ip6_configure(const char *ifname, const char *address,
			  unsigned flags, unsigned timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
