# HTTP/3 integration for the existing lwIP port

The service selects upstream lwIP2.2.1 at commit
77dcd25a72509eb83f72b033d219b1d40cd8eb95. Makefile.uk fetches the immutable
archive and applies three groups in order, with zero fuzz:

1. patches/2.2.1/common: thirteen existing Unikraft/libc adaptations, preserving
   their authorship. The obsolete address-check patch is omitted because the
   release has its own bounds checks.
2. patches/2.2.1/fixes: one independent patch per submitted upstream correction.
3. patches/2.2.1/native: Unikraft-specific socket and timing adaptations.

| Generic fix | Upstream | Contents |
|---|---|---|
|0001 pointer width|lwip-tcpip/lwip#112|GNU pointer-size detection in upstream unit configuration; no guest configuration change|
|0002 callback type|lwip-tcpip/lwip#113|Type-correct loopback callback and native UDP regression|
|0003 MQTT length|lwip-tcpip/lwip#114|Reject a fourth continuation byte, plus valid/invalid fragmented-input tests|
|0004 recvmsg length|lwip-tcpip/lwip#115|Return min(datagram length, total capacity), comparing as size_t before conversion; native socket regressions|

Each patch records its source commit and PR. MQTT test context is backported
without importing master's unrelated existing rx_overflow test. These patches
also apply to the pristine release for native testing with
`tests/upstream/lwip.py --release` from the workspace. The unit pointer-width
patch only affects upstream tests. MQTT is not compiled by the HTTP/3 app.
When the selected source version contains a fix, remove its patch and retest.
A merge on master alone does not fix an older release archive.

| Native patch | Topic |
|---|---|
|0016 sockaddr-lengths|Family-specific actual lengths and bounded copies for musl socket interfaces|
|0017 udp-ancillary-data|RX/TX address and traffic-class metadata|
|0018 udp-do-not-fragment|DF/EMSGSIZE, using0017 storage|
|0019 tcp-readiness|Persistent Unikraft eventpoll state; uses common/0006 callback|
|0021 native-msghdr-iovlen-type|Derive iovec index type from the native libc declaration|

Port source/configuration (uknetdev.c, Config.uk, Makefile.uk, lwipopts.h,
socket glue) is maintained directly, not exported as patches to reapply.
RX software checksum validation and dispatcher handoff belong to this port;
partial-checksum/GSO input is unsupported at the device boundary. Poll-only
mode cannot use the deferred RX dispatcher. Preserve contributors' licenses.

Original2.1 source choices remain available. Their existing independent
recvmsg correction is updated to the same expression as#115. These choices
have source-selection/patch coverage, not the2.2.1 HTTP/3 runtime qualification.
The2.2.1 port additionally handles callback_arg.socket and the new ACD module.

The native metadata patches compile for IPv4-only, IPv6-only and dual-stack
selections on both pinned versions. Only the dual-stack nghttpx profile is
runtime-qualified. The IPv4 IP_MTU_DISCOVER adapter sets DF, preserves it when
UDP allocates a separate header pbuf, and reports interface-MTU violations as
EMSGSIZE through both sendmsg and sendto. It does not emulate Linux's PMTU cache.

The base submission series ends at dev/lwip-native-integration. It contains no
independent ACK-deadline code or Kconfig setting. The separately retained
dev/lwip-ack-deadline branch adds that policy for the measured application;
it is not part of the initial port PRs. Existing upstream common adaptations,
including its TIME_WAIT handling, are preserved and are not new policy claims.
