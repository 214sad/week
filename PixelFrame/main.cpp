#include "pixelFrame.hh"
#include<iostream>

int main()
{
        PixelSensor sensor;

        uint32_t readback;
        sensor.send_slowctrl_command(1,200,&readback);
        //sensor.send_slowctrl_command(2,0,&readback);
        std::cout<<"config baoguang time"<<readback<<"ms"<<std::endl;

        sensor.send_slowctrl_command(2,0,&readback);
        std::cout<<"auto trigger mode"<<readback<<std::endl;

        sensor.send_trigger();
        std::cout<<"triggger success"<<std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        PixelFrame frame;
        if(sensor.get_frame(&frame)==0)
        {
                std::cout<<"read bit successful!!!!"<<std::endl;
                sensor.print_frame_byte(frame);
                std::cout<<"------------------this is bit print----------------"<<std::endl;
                sensor.print_frame_pixel(frame);
        }
        else
                std::cout<<"fail read bit"<<std::endl;

        sensor.get_status(0,&readback);
        std::cout<<"status:"<<readback<<std::endl;

        return 0;

}
