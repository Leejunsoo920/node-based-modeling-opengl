#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <memory>
#include <vector>

#include "imgui\imgui.h"

class Frame_manager
{
public:


   
    int end_frame = 199;
    int now_frame = 0;
    
    int frame_count = 0;

    bool play_frame = false;
    //bool auto_play = false;

    float dt = 20.67;

	Frame_manager(){}
	~Frame_manager() {}


    void update(

    )
    {


        if (play_frame == true)
        {
            frame_count++;
            if (frame_count == 1)
            {
                if (now_frame >= end_frame)
                {
                    now_frame = 0;
                }
                now_frame++;
                frame_count = 0;
            }

        }



        frame_manager_imgui_render();
    }




private:





    void frame_manager_imgui_render(
    )
    {
        
        const ImGuiIO& io = ImGui::GetIO();

            ImGui::Begin("frame manager");
        

        
        {
            ImGui::Checkbox("play", &play_frame);
            ImGui::SameLine();
            //ImGui::Checkbox("auto", &auto_play);


      
            ImGui::SliderInt("frame", &now_frame, 0, end_frame- 1);
            now_frame = now_frame;

        }
        ImGui::End();



    }



};





#endif