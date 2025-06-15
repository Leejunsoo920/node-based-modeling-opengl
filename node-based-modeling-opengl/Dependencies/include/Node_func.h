#ifndef  NODE_FUNC_H
#define NODE_FUNC_H


#include <algorithm> 
#include <iostream>
#include "Node_template.h"

extern Frame_manager frame_manager;
class Node_func_line : public Node_template_func
{
public:
    Node_func_line(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }


    void evaluate() override
    {

        if (input_node.size() >= 1) 
        {

            this->value = input_node[0]->value;
            this->value_vector_size = (*this->value).size();
            evaluated = true;

        }


    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }


private:

    //glm::vec3 direction = glm::vec3(0.0);

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();



        }
        ImGui::EndChild();


    }
};



class Node_func_line_to_global_matrix : public Node_template_func
{
public:
    Node_func_line_to_global_matrix(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::vec3> line;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {


                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::vec3>) {

                            line.emplace_back(arg1);

                        }
                        }, (*this->value)[i]);  // 

                }

                if (line.size() > 0)
                {
                    std::vector<glm::mat4> local_dir(line.size());
                    std::vector<glm::mat4> global_dir(line.size());
                    local_transform_matrix_to_global_transform_matrix(line, local_dir, global_dir);

                    if (input_node[0]->now_output_count == 1)
                    {

                        (*this->value).clear();
                        for (auto& it : global_dir)
                        {

                            this->value->emplace_back(it);

                        }
                        this->value_vector_size = (*this->value).size();
                        evaluated = true;
                    }
                    else
                    {
                        this->value = std::make_shared<std::vector<ValueVariant>>();

                        for (auto& it : global_dir)
                        {

                            this->value->emplace_back(it);

                        }
                        this->value_vector_size = (*this->value).size();
                        evaluated = true;

                    }


                }
                


            }

            



        }



    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }
            


        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

        }
        ImGui::EndChild();


    }



    void set_local_transform_matrix_by_line(
        const std::vector<glm::vec3> point_line,
        std::vector<glm::mat4>& local_bone_T
    )
    {
        glm::vec3 before_dir = glm::vec3(0, 1, 0);
        glm::vec3 next_dir = point_line[1] - point_line[0];

        next_dir = glm::normalize(next_dir);

        Quaternion bone_Q = Quaternion(next_dir);

        local_bone_T[0] = bone_Q.quaternion_to_r_matrix();

        before_dir = next_dir;




        for (unsigned int i = 1; i < point_line.size() - 1; i++)
        {
            next_dir = point_line[i + 1] - point_line[i];


            if (glm::length(next_dir) < 1e-6)
            {
                next_dir = glm::vec3(0, 1, 0);
            }
            else
            {

                next_dir = glm::normalize(next_dir);
            }

            bone_Q = Quaternion(before_dir, next_dir);
            local_bone_T[i] = bone_Q.quaternion_to_r_matrix();
            before_dir = next_dir;

            //before_dir = before_dir * glm::vec3(0, 1, 0);

        }


    }

    void local_transform_matrix_to_global_transform_matrix(
        const std::vector<glm::vec3>& point_line,
        std::vector<glm::mat4>& local_bone_T,
        std::vector<glm::mat4>& global_bone_T
    )
    {
        glm::vec3 before_dir = glm::vec3(0, 1, 0);
        glm::vec3 next_dir = point_line[1] - point_line[0];

        Quaternion bone_Q = Quaternion(next_dir);

        local_bone_T[0] = bone_Q.quaternion_to_r_matrix();
        global_bone_T[0] = glm::translate(glm::mat4(1.0), point_line[0]) * local_bone_T[0];


        before_dir = next_dir;




        float length_of_next_point = glm::distance(point_line[0], point_line[1]);
        global_bone_T[1] = glm::translate(global_bone_T[0], glm::vec3(0, length_of_next_point, 0));

        for (unsigned int i = 1; i < point_line.size() - 1; i++)
        {

            glm::vec3 next_point = glm::inverse(global_bone_T[i]) * glm::vec4(point_line[i + 1], 1);
            float length_of_next_point = glm::length(next_point);
            bone_Q = Quaternion(next_point);

            local_bone_T[i] = bone_Q.quaternion_to_r_matrix();
            global_bone_T[i] = global_bone_T[i] * local_bone_T[i];
            
            global_bone_T[i + 1] = glm::translate(global_bone_T[i], glm::vec3(0, length_of_next_point, 0));
        }



    }



};




class Node_func_bezier_curve : public Node_template_func
{
public:
    Node_func_bezier_curve(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {
        now_output_count = output_node_num;

        if (input_node.size() >= 2) 
        {


            this->value = input_node[0]->value;



            if (input_node[1]->value->size() == 1)
            {
                int curve_point_num;
                std::vector<glm::vec3> curve_point_vec3;

                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, int>) {

                        curve_point_num = arg1;

                    }
                    }, (*input_node[1]->value)[0]);  // 



                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {


                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::vec3>) {

                            curve_point_vec3.emplace_back(arg1);

                        }
                        }, (*this->value)[i]);  // 

                }
                std::vector<glm::vec3> final_curve;

                if (curve_point_vec3.size() >= 3)
                {

                    float gap = 1 / static_cast<float>(curve_point_num - 1);

                    
                    for (size_t i = 0; i + 2 < curve_point_vec3.size(); i += 2)
                    {
                        glm::vec3 p0 = curve_point_vec3[i];
                        glm::vec3 p1 = curve_point_vec3[i + 1];
                        glm::vec3 p2 = curve_point_vec3[i + 2];


                        float t = 0.0f;

                        for (int j = 0; j < curve_point_num; ++j)
                        {
                            glm::vec3 pt = make_bezier_curve(p0, p1, p2, t);
                            final_curve.push_back(pt);
                            t += gap;


                        }

                        if (i + 3 < curve_point_vec3.size()) {
                            final_curve.pop_back();
                        }


                    }






                    (*this->value).clear();
                    for (auto& it : final_curve)
                    {

                        this->value->emplace_back(it);

                    }

                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }

            }

        }


    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }

    



private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);

        return interpolate.do_interpolate(A, B, t);
    }

    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, const T& P3, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);
        T C = interpolate.do_interpolate(P2, P3, t);

        T D = interpolate.do_interpolate(A, B, t);
        T E = interpolate.do_interpolate(B, C, t);


        return interpolate.do_interpolate(D, E, t);




    }




};



class Node_func_cubic_bezier_curve : public Node_template_func
{
public:
    Node_func_cubic_bezier_curve(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {
        now_output_count = output_node_num;

        if (input_node.size() >= 2)
        {
            if (input_node[0]->evaluated == true && input_node[1]->evaluated == true) {

                this->value = input_node[0]->value;



                if (input_node[1]->value->size() == 1)
                {
                    int curve_point_num;
                    std::vector<glm::vec3> curve_point_vec3;

                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v<T1, int>) {

                            curve_point_num = arg1;

                        }
                        }, (*input_node[1]->value)[0]);  // 



                    for (unsigned int i = 0; i < (*this->value).size(); i++)
                    {
                        std::visit([&](auto&& arg1) {
                            using T1 = std::decay_t<decltype(arg1)>;
                            if constexpr (std::is_same_v<T1, glm::vec3>) {

                                curve_point_vec3.emplace_back(arg1);

                            }
                            }, (*this->value)[i]);  // 

                    }
                    std::vector<glm::vec3> final_curve;

                    if (curve_point_vec3.size() >= 4)
                    {

                        float gap = 1 / static_cast<float>(curve_point_num - 1);


                        for (size_t i = 0; i + 3 < curve_point_vec3.size(); i += 3)
                        {
                            glm::vec3 p0 = curve_point_vec3[i];
                            glm::vec3 p1 = curve_point_vec3[i + 1];
                            glm::vec3 p2 = curve_point_vec3[i + 2];
                            glm::vec3 p3 = curve_point_vec3[i + 3];

                            float t = 0.0f;

                            for (int j = 0; j < curve_point_num; ++j)
                            {
                                glm::vec3 pt = make_bezier_curve(p0, p1, p2, p3, t);
                                final_curve.push_back(pt);
                                t += gap;


                            }

                            if (i + 4 < curve_point_vec3.size()) {
                                final_curve.pop_back();
                            }


                        }


                        (*this->value).clear();
                        for (auto& it : final_curve)
                        {

                            this->value->emplace_back(it);

                        }

                        this->value_vector_size = (*this->value).size();
                        evaluated = true;



                    }

                }



            }
        }
        



    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }





private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {


            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);

        return interpolate.do_interpolate(A, B, t);
    }

    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, const T& P3, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);
        T C = interpolate.do_interpolate(P2, P3, t);

        T D = interpolate.do_interpolate(A, B, t);
        T E = interpolate.do_interpolate(B, C, t);


        return interpolate.do_interpolate(D, E, t);




    }




};

class Node_generate_circle_object : public Node
{
public:
    std::vector<std::shared_ptr<Object_setting>> initial_value;
    int initial_vertex_num = 12;
    float initial_radius = 0.02;

    Node_generate_circle_object(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Object_setting> object_setting,
        int object_count,
        std::shared_ptr<Shader> shader

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        Object_setting temp_Object_setting(object_count, shader);

        std::vector<int> new_indices;
        std::vector<Texture> new_texture;
        for (unsigned int i = 0; i < initial_vertex_num; i++)
        {
            new_indices.push_back(i);

        }


        Mesh new_mesh(generate_circle(initial_radius, initial_vertex_num), new_indices, new_texture);


        temp_Object_setting.object.meshes.push_back(new_mesh);




        initial_value.emplace_back(std::make_shared<Object_setting>(
            temp_Object_setting,
            object_count,
            shader

        ));

        this->value->emplace_back(initial_value[0]);


    }

    void evaluate() override
    {
        if (input_node.size() == 0)
        {
            initial_value[0]->object.meshes[0].vertices = generate_circle(initial_radius, initial_vertex_num);
            initial_value[0]->object.meshes[0].indices.clear();

            for (unsigned int i = 0; i < initial_vertex_num; i++)
            {
                initial_value[0]->object.meshes[0].indices.push_back(i);
            }

            value->clear();
            for (auto& v : initial_value)
            {
                value->emplace_back(v);
            }
            evaluated = true;
        }
        else if(input_node.size() > 0)
        {
            std::vector<glm::vec3> vertices;
            std::vector<Vertex> return_vertices;
            for (unsigned int i = 0; i < input_node[0]->value->size(); i++)
            {
                std::visit([&](auto&& arg1) 
                    {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, glm::vec3>) {

                        vertices.emplace_back(arg1);

                    }
                    }, (*input_node[0]->value)[i]); 

            }

            for (int i = 0; i < vertices.size(); ++i) 
            {
                Vertex this_vertex;
                this_vertex.Position = glm::vec3(vertices[i].x,vertices[i].y, vertices[i].z);
                return_vertices.emplace_back(this_vertex);
            }


            if (vertices.size() > 2)
            {
                initial_value[0]->object.meshes[0].vertices = return_vertices;
                initial_value[0]->object.meshes[0].indices.clear();

                value->clear();
                for (auto& v : initial_value)
                {
                    value->emplace_back(v);
                }
                evaluated = true;

            }



        }

    
    
    
    };

    
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {
        bool ret = false;
        if (this->value != nullptr)
        {


            std::visit([this, &ret](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;

                
                if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                    for (unsigned int i = 0; i < arg1->object.meshes[0].vertices.size() - 1; i++)
                    {
                        debug.set_line_group("circle", arg1->object.meshes[0].vertices[i].Position, arg1->object.meshes[0].vertices[i + 1].Position);
                    }
                    debug.set_line_group("circle", arg1->object.meshes[0].vertices[0].Position, arg1->object.meshes[0].vertices[arg1->object.meshes[0].vertices.size() - 1].Position);

                    ret = true;

                    return ret;

                }
                else
                {
                    ret = false;
                    return ret;


                }
                }, (*this->value)[0]);  // 

        }
        ret = false;
        return ret;

    }

    

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();



            bool changed = false;
            changed |= ImGui::InputFloat((name + "x").c_str(), &initial_radius, 0.1f, 1.0f, "%.3f");
            changed |= ImGui::InputInt((name + "y").c_str(), &initial_vertex_num, 1, 10);
            if (initial_vertex_num < 3)
            {
                initial_vertex_num = 3;
            }

            if (changed)
            {
                evaluated = false;
            }




        }
        ImGui::EndChild();



    }


    std::vector<Vertex> generate_circle
    (
        float radius, int num_vertices
    )
    {
        std::vector<Vertex> return_vertices;

        std::vector<glm::vec3> points;
        points.reserve(num_vertices);

        const float PI = 3.14159265358979323846f;

        for (int i = 0; i < num_vertices; ++i) {
            float angle = 2.0f * PI * i / num_vertices; // 각도: 0 ~ 2π
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);


            Vertex this_vertex;
            this_vertex.Position = glm::vec3(x, 0.0f, z);


            return_vertices.emplace_back(this_vertex); // y=0: XZ 평면
        }



        return return_vertices;
    }





};




class Node_func_sweep : public Node_template_func
{
public:
    Node_func_sweep(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected){}

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            if ((*input_node[0]->value).size() == 1)
            {

                std::vector<glm::mat4> temp_mat4;

                for (auto& it : *input_node[1]->value)
                {
                    std::visit([&](auto&& arg2) {
                        using T2 = std::decay_t<decltype(arg2)>;

                        if constexpr
                            (
                                std::is_same_v<T2, glm::mat4>)
                        {
                            temp_mat4.emplace_back(arg2);

                        }
                        else
                        {

                            return;
                        }

                        }, it);  // 

                }



                std::visit([&](auto& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;

                        if constexpr (std::is_same_v < T1, std::shared_ptr<Object_setting>>)
                        {
                            int bone_size = temp_mat4.size();

                            int vertex_num = arg1->object.meshes[0].vertices.size();

                            std::vector<Vertex> origin_vertices;
                            for (auto&& it : arg1->object.meshes[0].vertices)
                            {
                                origin_vertices.push_back(it);

                            }



                            std::vector<Vertex> new_vertices;
                            std::vector<int> new_indices;

                            for (unsigned int i = 0; i < bone_size; i++)
                            {
                                int vertex_size = origin_vertices.size();

                                for (unsigned int j = 0; j < vertex_size; j++)
                                {
                                    new_vertices.push_back(origin_vertices[j]);

                                    new_vertices.back().Position = temp_mat4[i] * glm::vec4(origin_vertices[j].Position, 1);

                                  


                                }

                                /*

                                auto vertex_index = new_vertices.end() - new_vertices.begin()  - 2 * vertex_size;

                                if (bone_size > 1)
                                {
                                    if (i > 0)
                                    {
                                        new_indices.emplace_back(vertex_index);
                                        new_indices.emplace_back(vertex_index + vertex_size);
                                        new_indices.emplace_back(vertex_index + vertex_size + 1);


                                        new_indices.emplace_back(vertex_index);
                                        new_indices.emplace_back(vertex_index +1);
                                        new_indices.emplace_back(vertex_index + 1 + vertex_size);


                                        for (unsigned int k = 1; k < vertex_size; k++)
                                        {

                                            new_indices.emplace_back(vertex_index + k);
                                            new_indices.emplace_back(vertex_index + vertex_size + k);
                                            new_indices.emplace_back(vertex_index + vertex_size + 1 + k);


                                            new_indices.emplace_back(vertex_index + k);
                                            new_indices.emplace_back(vertex_index + 1 + k);
                                            new_indices.emplace_back(vertex_index + 1 + vertex_size + k);

                                        }
                                    }

                                }
                                */
                                auto base_index = new_vertices.size() - 2 * vertex_size;
                                if (bone_size > 1 && i > 0)
                                {
                                    // 이전 단면의 시작 인덱스



                                    for (unsigned int k = 0; k < vertex_size; ++k)
                                    {
                                        // 다음 인덱스를 원형으로 돌림 (0 → 1 → ... → vertex_size-1 → 0)
                                        unsigned int next_k = (k + 1) % vertex_size;

                                        // 현재 단면과 이전 단면을 잇는 네 정점
                                        unsigned int i0 = base_index + k;
                                        unsigned int i1 = base_index + next_k;
                                        unsigned int i2 = base_index + k + vertex_size;
                                        unsigned int i3 = base_index + next_k + vertex_size;

                                        // 첫 삼각형 (i0, i2, i3)
                                        new_indices.emplace_back(i0);
                                        new_indices.emplace_back(i2);
                                        new_indices.emplace_back(i3);

                                        // 둘째 삼각형 (i0, i3, i1)
                                        new_indices.emplace_back(i0);
                                        new_indices.emplace_back(i3);
                                        new_indices.emplace_back(i1);
                                    }
                                }
                                else
                                {

                                    for (unsigned int k = 0; k < vertex_size; k++)
                                    {

                                        new_indices.emplace_back(base_index - k);


                                    }



                                }




                            }

                            //if(input_node)
                            //arg1->object.meshes[0].indices = new_indices;
                            //arg1->object.meshes[0].vertices = new_vertices;

                            //auto obj_ptr = std::make_shared<Object_setting>(arg1->shader);
                            arg1->object.meshes[0].vertices = new_vertices;
                            arg1->object.meshes[0].indices = new_indices;
                            this->value->clear();
                            this->value->emplace_back(arg1);


                            this->value_vector_size = (*this->value).size();
                            evaluated = true;
                        }
                        else
                        {
                            return;
                        }



                    }, (*input_node[0]->value)[0]);



                
            
            }


           

        }


    }



private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }






};



class Node_func_select_one_value : public Node_template_func
{
public:
    int initial_value = 0;

    Node_func_select_one_value(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected) {}

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            if ((*input_node[0]->value).size() > 1)
            {

                std::vector<glm::mat4> temp_mat4;
                auto& it = (*input_node[1]->value)[0];

                std::visit([&](auto&& arg2) {
                    using T2 = std::decay_t<decltype(arg2)>;

                    if constexpr
                        (
                            std::is_same_v<T2, int>)
                    {


                        initial_value = arg2;

                    }
                    else
                    {

                        return;
                    }

                    }, it);  // 

                int temp = (*input_node[0]->value).size();
                if (initial_value > temp)
                {
                    initial_value = temp - 1;
                }


                (*this->value).clear();
                (*this->value).emplace_back((*input_node[0]->value)[initial_value]);

                this->value_vector_size = (*this->value).size();
                evaluated = true;
            }




        }



    }



private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }






};





class Node_func_matrix_rotation : public Node_template_func
{
public:
    glm::mat4 initial_value = glm::mat4(1.0f);
    Node_func_matrix_rotation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;

                if (input_node.size() >= 2)
                {

                    std::visit([&](auto&& arg0) {
                        using T1 = std::decay_t<decltype(arg0)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            glm::mat4 rotation_mat4 = glm::mat4(1.0f);

                            rotation_mat4[0] = glm::vec4(glm::normalize(glm::vec3(arg0[0])), 0.0f); 
                            rotation_mat4[1] = glm::vec4(glm::normalize(glm::vec3(arg0[1])), 0.0f);  
                            rotation_mat4[2] = glm::vec4(glm::normalize(glm::vec3(arg0[2])), 0.0f);  
                            rotation_mat4[3] = glm::vec4(0, 0, 0, 1);  




                            initial_value = rotation_mat4;

                        }
                        }, (*input_node[1]->value)[0]);  


                }

                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {




                    
                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>) 
                        {


                            glm::vec3 position = glm::vec3(arg1[3]);


                            glm::mat4 rotationOnly = glm::mat4(1.0f);
                            rotationOnly[0] = glm::vec4(glm::normalize(glm::vec3(arg1[0])), 0.0f);
                            rotationOnly[1] = glm::vec4(glm::normalize(glm::vec3(arg1[1])), 0.0f);
                            rotationOnly[2] = glm::vec4(glm::normalize(glm::vec3(arg1[2])), 0.0f);



                            glm::mat4 rotated = initial_value * rotationOnly;


                            rotated[3] = glm::vec4(position, 1.0f);

                            global_dir.emplace_back(rotated);
                        }
                        }, (*this->value)[i]);  

                }











                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }





    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = false;

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};



class Node_func_matrix_z_look_up : public Node_template_func
{
public:
    glm::mat4 initial_value = glm::mat4(1.0f);
    Node_func_matrix_z_look_up(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }




    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {

                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;

                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            glm::vec3 position = glm::vec3(arg1[3]);

                            glm::vec3 z_dir = glm::vec3(0, 0, 1);
                            z_dir = arg1 * glm::vec4(z_dir,1.0f);

                            float rotation_angle = glm::dot(glm::vec3(0, 1, 0), z_dir);


                            Quaternion quaternion = Quaternion(rotation_angle, 0, 1, 0);

                            glm::mat4 rotated = arg1 * quaternion.quaternion_to_r_matrix();


                            rotated = glm::rotate(rotated, glm::radians(-90.0f), glm::vec3(0, 0, 1));
                            rotated = glm::rotate(rotated, glm::radians(-90.0f), glm::vec3(0, 1, 0));



                            global_dir.emplace_back(rotated);
                        }
                        }, (*this->value)[i]);

                }











                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }





    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = false;

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};







class Node_func_mesh_projection : public Node_template_func
{
public:
    

    Node_func_mesh_projection(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected) {}

    void evaluate() override
    {


        if (input_node.size() >= 2) {

            if ((*input_node[0]->value).size() > 0 && (*input_node[1]->value).size() > 0 )
            {

                std::vector<int> vertex_num;

                std::vector<glm::vec3> vertex_list;

                auto& it0 = (*input_node[0]->value)[0];
                auto& it1 = (*input_node[1]->value)[0];

                std::visit([&](auto&& arg1, auto&& arg2) 
                    {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;


                    if constexpr (
                        std::is_same_v<T1, shared_ptr<Object_setting>> &&
                        std::is_same_v<T2, shared_ptr<Object_setting>>
                        )
                    {

                        std::vector<glm::vec3> target_object_vertex;
                        std::vector<int> target_object_indices;

                        


                        for (auto& vertex : arg1->object.meshes[0].vertices)
                        {
                            target_object_vertex.emplace_back(vertex.Position);
                        }
                        for (auto& index : arg1->object.meshes[0].indices)
                        {
                            target_object_indices.emplace_back(index);
                        }


                        for (unsigned int i = 0; i < arg2->object.meshes[0].vertices.size(); i++)
                        {

                            real_act(target_object_vertex, target_object_indices, arg2->object.meshes[0].vertices[i].Position);

                        }






                    }
                    else
                    {
                        return;
                    }

                    }, it0, it1);  // 


                    this->value_vector_size = (*this->value).size();
                    evaluated = true;


                





            }









        }



    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


    void real_act(
        const std::vector<glm::vec3>& vertices,
        const std::vector<int>& indices,
        glm::vec3& point
    )
    {
        int closest_triangle = -1;
        glm::vec3 closest_position;
        findClosestPointOnMesh(vertices, indices, point, closest_triangle, closest_position);

        debug.set_point_group("closest_point", closest_position);

        point = closest_position;


    }


    void findClosestPointOnMesh(
        const std::vector<glm::vec3>& vertices,
        const std::vector<int>& indices,
        glm::vec3& point,
        int& outTriangleIndex,
        glm::vec3& outClosestPoint)
    {
        float minDistSq = std::numeric_limits<float>::max();
        glm::vec3 closest;

        for (size_t i = 0; i < indices.size(); i += 3) {
            if (indices[i] >= 60 || indices[i + 1] >= 60 || indices[i + 2] >= 60)
                continue;
            const glm::vec3& a = vertices[indices[i]];
            const glm::vec3& b = vertices[indices[i + 1]];
            const glm::vec3& c = vertices[indices[i + 2]];

            glm::vec3 q = closestPointOnTriangle(point, a, b, c);
            float distSq = glm::length(point - q);

            if (distSq < minDistSq) {
                minDistSq = distSq;
                closest = q;
                outTriangleIndex = static_cast<int>(i / 3);
                outClosestPoint = q;
            }
        }
    }

    glm::vec3 closestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        // Möller–Trumbore 기반 알고리즘
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ap = p - a;

        float d1 = glm::dot(ab, ap);
        float d2 = glm::dot(ac, ap);

        if (d1 <= 0.0f && d2 <= 0.0f) return a; // Bary(1,0,0)

        glm::vec3 bp = p - b;
        float d3 = glm::dot(ab, bp);
        float d4 = glm::dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return b; // Bary(0,1,0)

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + v * ab; // Bary(1 - v, v, 0)
        }

        glm::vec3 cp = p - c;
        float d5 = glm::dot(ab, cp);
        float d6 = glm::dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return c; // Bary(0,0,1)

        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + w * ac; // Bary(1 - w, 0, w)
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b); // Bary(0, 1 - w, w)
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w; // 내부
    }




};



/*

class Node_func_vertex_translate : public Node_template_func
{
public:
    std::vector<glm::mat4> translate_matrix;

    Node_func_translate(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {


                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            global_dir.emplace_back(glm::rotate(arg1, glm::radians(initial_value), glm::vec3(0, 1, 0)));
                        }
                        }, (*this->value)[i]);  // 

                }

                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }




private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = ImGui::SliderFloat("angle", &initial_value, 0, 360);

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};


*/




class Node_func_now_frame : public Node_template_func
{
public:

    int before_frame = 0;

    Node_func_now_frame(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {


        (this->value)->clear();
        (this->value)->push_back(frame_manager.now_frame);
    
        evaluated = true;

    }

private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            ImGui::Text("now frame : %d", frame_manager.now_frame);



        }
        ImGui::EndChild();


    }
};






#endif // ! NODE_FUNC_H
