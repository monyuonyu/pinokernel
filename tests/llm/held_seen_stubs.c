/* held_seen_stubs.c — link-only stubs for student_seen_test.c (the same as
 * run_yield.sh generates): the chat/frontier verbs are never driven there.
 * conscience returns CONS_ALLOW (0). */
int conscience_check(unsigned char site, const void *q) { (void)site; (void)q; return 0; }
const char *conscience_on_refuse(unsigned char site, int verdict) { (void)site; (void)verdict; return ""; }
void frontier_self_test(void (*emit)(const char *)) { (void)emit; }
