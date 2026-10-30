From b39f76ea466c30993fd1a69c7b6ce45016fd7b42 Mon Sep 17 00:00:00 2001
From: Sheng <hsdai22@m.fudan.edu.cn>
Date: Thu, 22 Oct 2026 18:32:12 +0800
Subject: [PATCH] 
 0011-src-include-Make-lwip-sockets.h-compatible-with-Linux-kernel

Rebased Unikraft adaptation for lwIP 2.2.1.
---
 src/include/lwip/sockets.h | 6 +++---
 1 file changed, 3 insertions(+), 3 deletions(-)

diff --git a/src/include/lwip/sockets.h b/src/include/lwip/sockets.h
index 59c6b659..46a8c072 100644
--- a/src/include/lwip/sockets.h
+++ b/src/include/lwip/sockets.h
@@ -60,9 +60,9 @@ extern "C" {
  */
 #define TCP_NODELAY    0x01    /* don't delay send to coalesce packets */
 #define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
-#define TCP_KEEPIDLE   0x03    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
-#define TCP_KEEPINTVL  0x04    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
-#define TCP_KEEPCNT    0x05    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */
+#define TCP_KEEPIDLE   0x04    /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
+#define TCP_KEEPINTVL  0x05    /* set pcb->keep_intvl - Use seconds for get/setsockopt */
+#define TCP_KEEPCNT    0x06    /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */
 #endif /* LWIP_TCP */
 
 /* FD_SET used for lwip_select */
