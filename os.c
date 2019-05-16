#include "os.h"
// note: make below task assignment const if you want to save some ram, adding and removing task function will not support if const
struct EVENT_STR str_event;

Fun_Ptr  tsk_1ms[4]    = {app_test_1ms,
                                #if(ADC_MODULE == REQUIRE) 
                                adc_mgr, 
                                #endif 
                                0};
Fun_Ptr  tsk_10ms[]   = {app_test_10ms,0};
Fun_Ptr  tsk_100ms[]  = {app_test_100ms,0};
Fun_Ptr  tsk_1000ms[] = {app_test_1000ms,0};
Fun_Ptr  tsk_BG[]     = {app_test_BG,0};

struct STR_OS os =
{
	{tsk_1ms,tsk_10ms,tsk_100ms,tsk_1000ms,tsk_BG}, // pro_ptr
    {0},							// sys timer in ms
    {0},                            // sys timer in seconds , 18 hrs max
	{0,0,0,0,1}, // background process always active
	{0,0,0,0,0}, // active task index bits
	{0,0,0,0,0} // time keeper
};

uint8 i_1=0u,j_1=0u;
Fun_Ptr tmp_task;

void main(void)
{
    //OSCILLATOR
    Sys_Ini();  
    #if(DEBUG_MSG == REQUIRE)
    com_send_string("Init...\n");
    #endif
    
    Run_Os();
    //while(1);
    
 }
void Run_Os(void)
{
    
	while(1)
	{
        tmp_task = NULL;
		for(i_1=0;i_1<(MAX_PRO);i_1++) // scan all processes exclude baground tak whcih is at location (MAX_PRO -1) , till null pointer
		{
			if(os.pro_bit[i_1]) // if task in current process , Note , this bit for baground task is alwys set
			{
                CRITICAL_EN;
                tmp_task = *(os.pro_ptr[i_1] + os.tsk_count[i_1]);
                CRITICAL_DIS;

                if(tmp_task) // check if task is not null
                {
#if(DEBUG_MSG == REQUIRE)
                    if(i_1 == (BAG_PRO_IDX - 1)) //Display only 1sec task
                    {
                        com_send_string("T:");
                        com_send_Dat(((i_1+1) + '0')); // 10 , 20 ..ms
                        SEND_EOL; // end of line
                       
                    }
#endif  
                    (*tmp_task)(); // execute the task , ither one of the task from process or baground task if nothing to execute
                    os.tsk_count[i_1]++;// execute task in current process and break to ckeck if higer prority task active               
                    break;
                }
                else
                {
                     if(i_1<BAG_PRO_IDX)
                     {
                         os.pro_bit[i_1] = 0u; // proces is complete keep task active alwys
                     }
                     os.tsk_count[i_1] = 0u;//reset the process to starting
                }
			}
		}        
        if(os.time_keeper[0])
        {
            for(j_1=0u;j_1<(BAG_PRO_IDX);j_1++) // scall all process , exclude baground process
            {
                if(os.time_keeper[j_1]>=10u)
                {
                  os.pro_bit[j_1]=1;           // make task container active
                  os.time_keeper[j_1+1u]++;    // increment next task time keeper   
                  os.time_keeper[j_1]=0u;      // make current task time keeper zero
                }
            }
            os.os_time_ms++; // overflow after 50 days of operation
        }
        // go to sleep , only periperal are active CPU in sleep, wake up by any Interrupt
        
        if((i_1 == MAX_PRO) && (os.tsk_count[BAG_PRO_IDX]==0) && (os.pro_bit[0]==0))// if no process pending is active go to sleep
        {  
            while(com_get_tx_buf_lnt());
            SLEEP();
            NOP();
            NOP();
           
        }
        
    }
}


uint32 os_get_sys_tim(void)
{
	uint32 temp_tim;
	CRITICAL_EN;
	temp_tim=os.os_time_ms;
	CRITICAL_DIS;
	return (temp_tim);
}
/*
void Run_Container(Fun_Ptr *F_Ptr)
{
    while(*F_Ptr)
    {
        (*F_Ptr)();
        F_Ptr++;
    }
}
*/ 

uint8 os_remove_task(uint8 pocess_id,Fun_Ptr F_Ptr) // remove perticular task from procees
{
    // scan through task
    uint8 i=0, reap=0;
    while((*(os.pro_ptr[pocess_id] + i))!= 0)
    {
        if((*(os.pro_ptr[pocess_id] + i)) == F_Ptr)
        {
            while((*(os.pro_ptr[pocess_id] + i))!= 0)
            {
              (*(os.pro_ptr[pocess_id] + i))=(*(os.pro_ptr[pocess_id] + (i+1)));  // shift the array
              reap=1;
              i++;
            }
            break;
        }
        i++;
    }
    return reap;
}

void os_insert_task(uint8 pocess_id, Fun_Ptr F_Ptr) // // add perticular task at end of process
{
    // scan through task
    uint8 i=0;
    while((*(os.pro_ptr[pocess_id] + i))!=0)
    {
        i++;
    }
    (*(os.pro_ptr[pocess_id] + i))=F_Ptr; // make sure task array have enough space to incert the new task
    (*(os.pro_ptr[pocess_id] + (i+1)))=0;

} 

void event_push(uint8 Event_ID)
{
    if(!str_event.overflow)  // check if there is space for event in queue
    {
        str_event.queue[str_event.head] = Event_ID;
        if(str_event.head<MAX_EVENT_IDX){str_event.head++;}else {str_event.head = 0;} 
        
        if(str_event.head == str_event.tale)
        {
            str_event.overflow=1;
        }
    }
    else
    {
        // no more event can be queued
    }
}
uint8 event_read(void)
{
    // returns 0 if no event in queue, wlse retuen event ID
    uint8 temp_event_id=0;
    if((str_event.head != str_event.tale)||str_event.overflow) // something is present in queue
    {
        temp_event_id = str_event.queue[str_event.tale];
        if(str_event.tale<MAX_EVENT_IDX){str_event.tale ++;}else{str_event.tale = 0;} 
        str_event.overflow=0;
    }
    return (temp_event_id);
}