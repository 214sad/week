
#pragma once
#include <iostream>
#include<array>
#include<deque>
#include<mutex>
#include<map>
#include<chrono>
#include<thread>
#include<random>
#include<iomanip>
#include<cstdint>
enum STATUS_TYPE{
        RUN_STATUS=0,
        ERROR_STATUS,
        CTRL_STATUS
};
struct PixelFrame
{
        uint8_t d[64][1024];

};
class PixelSensor{
        public:
                PixelSensor();
                ~PixelSensor();
                //find status
                uint32_t get_status(uint32_t status_key,uint32_t *readback);
                //写寄存器
                uint32_t send_slowctrl_command(uint32_t cmd_key,uint32_t value,uint32_t *readback);
                //触发采集
                uint32_t send_trigger();
                //读取单帧
                uint32_t get_frame(PixelFrame *frame);
                //读取多帧
                uint32_t get_frames(uint32_t num,PixelFrame *frame);
                //持续传输线程
                uint32_t run_thread(std::deque<PixelFrame>&m_frame_fifo);
                //生成随机数
                uint32_t nornal_distribution(std::deque<PixelFrame>&m_frame_fifo);
                void print_frame_byte(const PixelFrame& frame);
                void print_frame_pixel(const PixelFrame &frame);
        private:
                //帧缓存FIFO
                std::deque<PixelFrame> m_frame_fifo;
                //32个控制
                std::array<uint32_t,32>m_reg_table;
                //硬件触发引脚状态
                bool m_pin_trigger;
                std::mutex m_data_mutex;
                std::map<uint32_t,uint32_t>histogram;
                std::thread m_run_thread;
                bool m_thread_running;
};


