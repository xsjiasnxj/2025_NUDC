
#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_
void control();
void start_svpwm();
void stop_svpwm();
void protect_svpwm();
extern unsigned int full_duty;
extern float module_rate;
#endif /* INC_CONTROL_H_ */
