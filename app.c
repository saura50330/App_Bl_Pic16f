#include "common.h"

uint8 tmr_1ses;
uint8 sw0_st_prev;
inline void app_ini(void)
{
    // test LED
    // Test Switch
    sw0_st_prev = ! (get_pin_5);
    OS_Init_Tmr(tmr_1ses);
    
    // Test Communication
    // test Operatig system
}
/*private function inclusion*/
void app_test_1ms(void)
{

    toggle_pin_1;     
}
void app_test_10ms(void)
{
   
}

void app_test_100ms(void)
{
    uint8 cmd_res_data[5],i=0;
    volatile uint8 sw0_st;
    
    if(com_get_rx_buf_lnt()) // buffer have some data
    {  
        while(i<5)
        {
           cmd_res_data[i] = 0xFF;  // cmd, data ,data,data,data,checksum,cntr
           i++;
        }
        i =0;
       
        while(com_get_rx_buf_lnt()) // read all data
        {
            cmd_res_data[i]=com_rx_read_char(); 
            i++;
        }
        
        if( cmd_res_data[0] == PING )  // default supported services
        {
            cmd_res_data[0] = PING_APP_RESP;
            led0_blink(2);
        }
        else if( cmd_res_data[0] == RSTCMD )
        {
            asm("RESET");
        }
        else if( cmd_res_data[0] == FLASH )
        {
            eep_write_char(0xF0,0x11);// make application invalid
            asm("RESET");// reset the device
        }
        else if( cmd_res_data[0] == WR_EEP )
        {
            eep_write_char(cmd_res_data[1],cmd_res_data[3u]);
        }
        else if( (cmd_res_data[0] == RD_EEP) )
        {
              cmd_res_data[3] = read_mem(*((uint16 *)&cmd_res_data[1]),1); // cmd,add,data
        }
        //------------APP COMMANDS------------------
        else if( i == 5 ) // all app command are of length 5
        {
            if (cmd_res_data[0] ==  READ_RAM)  // its multi byte  command, ther may be possiblity that only command is received
            {
                *((uint16*)(&cmd_res_data[3])) = *(uint16*)(*((uint16*)&cmd_res_data[1u])); // read two bytes in ram
            }
            else if (cmd_res_data[0] ==  CLR_GPIO_0)  // its multi byte  command, ther may be possiblity that only command is received
            {
                clear_pin_0;
            }
            else if (cmd_res_data[0] ==  SET_GPIO_0)  // application cases starts here command can have value from 0x11 to 0xFE
            {
                led0_blink(2);
            }
            else
            {
                cmd_res_data[0]=0xFF;// not supported
            }
        }
        else
        {
            cmd_res_data[0]=0xFF;// not supported
        }
        if(cmd_res_data[0] != 0xFF) // check for multi frame responce
        {
            // calculate and update checksum and send responce at byte 6 if needed
            com_send_dat(cmd_res_data,5);
        }
    }
    
   //------------switch event---------------
  
    sw0_st = get_pin_5;  
    if(sw0_st_prev!=sw0_st)
    {
       // event_push(FDR);
        if(sw0_st)
        {    
            event_push(SW0_RELEASE);     
        }
        else
        {
            event_push(SW0_PRESS);         
        }        
        sw0_st_prev=sw0_st;
    }
    
    // LED 
    led0_task();
}
void app_test_1000ms(void)
{
    uint8 temp_time;
    
    toggle_pin_2;

    os.os_time_sec++;
    OS_Run_Tmr(tmr_1ses); 
    
    temp_time = OS_Read_Tmr(tmr_1ses);
     
    if((temp_time >= TEN_SEC) && (temp_time <= FIFTEEN_SEC))
    {
        led0_blink(4); 
    }
     
}
void app_test_BG(void)
{
    uint8 temp_time;
     
    switch(event_read())
    {
        case SW0_PRESS :
            OS_Start_Tmr(tmr_1ses);
            led0_blink(2);  
        break;
        case SW0_RELEASE :
            temp_time = OS_Read_Tmr(tmr_1ses); // alwys read timer before stopping
            OS_Stop_Tmr(tmr_1ses);
            
            if((temp_time <= FIFTEEN_SEC))
            {
                if(temp_time >= TEN_SEC)
                {
                  event_push(FDR);
                }
            }   
            
        break;
        
        case FDR :
        led0_blink(40); // odd no is to compenste for FDR   
        break;   
    }
     
}