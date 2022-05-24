#include "Sleep.h"

void fpm_wakup_cb_func(void) {
  Serial.println("Light sleep is over, either because timeout or external interrupt");
  Serial.flush();
}

void light_sleep(unsigned long timeout){
  extern os_timer_t *timer_list;
  timer_list = nullptr;

  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func);
  // light sleep function requires microseconds
  wifi_fpm_do_sleep(timeout * 1000);
  delay(timeout + 1);
}

uint32_t RTCmillis() {
  // system_get_rtc_time() is in us (but very inaccurate anyway)
  return (system_get_rtc_time() * (system_rtc_clock_cali_proc() >> 12)) / 1000;
}
