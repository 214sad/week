#include "pixelFrame.hh"

PixelSensor::PixelSensor():m_pin_trigger(false),m_thread_running(false)
{
        //寄存器初始化
        m_reg_table.fill(0);
        m_reg_table[0]=RUN_STATUS;
        m_reg_table[1]=100;
        m_reg_table[2]=0;

        m_thread_running=true;

        m_run_thread=std::thread(&PixelSensor::run_thread,this,std::ref(m_frame_fifo));

}
PixelSensor::~PixelSensor()
{
        m_thread_running=false;
        if(m_run_thread.joinable())
        {
                m_run_thread.join();
        }


}
//实现寄存器的初始化，读，写， 读寄存器
uint32_t PixelSensor::get_status(uint32_t status_key,uint32_t *readback)
{

        if(status_key>=32||readback==nullptr)
                return 1;
        std::lock_guard<std::mutex>lock(m_data_mutex);
        *readback=m_reg_table[status_key];
        return 0;
}
//写寄存器
uint32_t PixelSensor::send_slowctrl_command(uint32_t cmd_key,uint32_t value,uint32_t *readback)

{
        if(cmd_key>=32||readback==nullptr)
                return 1;
        std::lock_guard<std::mutex> lock(m_data_mutex);
        m_reg_table[cmd_key]=value;
        *readback=m_reg_table[cmd_key];
        return 0;

}
//触发引脚
uint32_t PixelSensor::send_trigger()
{
        std::lock_guard<std::mutex> lock(m_data_mutex);
        m_pin_trigger=true;
        return 0;
}
//帧数据生成
uint32_t PixelSensor::nornal_distribution(std::deque<PixelFrame>&m_frame_fifo)
{
        PixelFrame frame;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(128,30);


        for(int row=0; row<1024;row++)
        {
                for(int col=0;col<64;col++)
                {
                        uint8_t pixel_byte=0;
                        for(int bit=0;bit<8;bit++)

                        {
                                int pixel_val=dis(gen)>128?1:0;
                                pixel_byte |=(pixel_val<<bit);
                        }
                        frame.d[col][row]=pixel_byte;
                }
        }
        std::lock_guard<std::mutex> lock(m_data_mutex);
        m_frame_fifo.push_back(frame);

        if(m_frame_fifo.size()>10)
        {
                m_frame_fifo.pop_front();
        }
        return 0;
}

//持续传输线程
uint32_t PixelSensor::run_thread(std::deque<PixelFrame> &m_frame_fifo)
{
        while(m_thread_running)
        {
                bool trigger=false;
                {
                        std::lock_guard<std::mutex>lock(m_data_mutex);
                        trigger=m_pin_trigger ||(m_reg_table[2]==1);
                }
                if(trigger)
                {
                        nornal_distribution(m_frame_fifo);

                        std::lock_guard<std::mutex>lock(m_data_mutex);
                        m_pin_trigger=false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return 0;
}

uint32_t PixelSensor::get_frame(PixelFrame *frame)
{
        if(frame==nullptr)
                return 1;

        std::lock_guard<std::mutex>lock (m_data_mutex);
        if(m_frame_fifo.empty())
                return 1;
        *frame=m_frame_fifo.front();
        m_frame_fifo.pop_front();
        return 0;
}
uint32_t PixelSensor::get_frames(uint32_t num,PixelFrame *frame)
{
        if(frame==nullptr||num==0)
                return 1;
        for(uint32_t i=0;i<num;i++)
        {
                if(get_frame(&frame[i])!=0)
                        return 1;
        }
        return 0;

}
//print data
void PixelSensor::print_frame_byte(const PixelFrame &frame)
{
        for(int row=0;row<10;row++)
        {
                for(int col=0;col<64;col++)
                {
                        std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)frame.d[col][row]<<" ";
                }
                std::cout<<std::endl;
        }
        std::cout<<std::dec;
}
//输出像素
void PixelSensor::print_frame_pixel(const PixelFrame &frame)
{
        for(int row=0;row<1;row++)
        {
                for(int col=0;col<7;col++)
                { 
                        uint8_t byte=frame.d[col][row];
                        for(int bit=7;bit>=0;bit--)
                        {
                                std::cout<<((byte>>bit)&1)<<" ";
                                }
                             std::cout<<std::endl;
                }
               
        }
}


















