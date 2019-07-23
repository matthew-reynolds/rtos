/**
 * main.c
 *
 * Test selection
 */

extern void test_mutex_prioinherit(void);
extern void test_semaphore_blocking(void);

int main(void) {
  //test_mutex_prioinherit();
  test_semaphore_blocking();
}
